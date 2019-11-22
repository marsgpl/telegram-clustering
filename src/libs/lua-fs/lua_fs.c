#include "lua_fs.h"

LUAMOD_API int luaopen_fs(lua_State *L) {
    luaL_newmetatable(L, LUA_MT_FS_DIR_ITER);
    lua_pushcfunction(L, lua_fs_read_dir__gc);
    lua_setfield(L, -2, "__gc");
    lua_pop(L, 1);

    luaL_newlib(L, __index);

    return 1;
}

static int lua_fs_stat(lua_State *L) {
    const char *path = luaL_checkstring(L, 1);

    struct stat statbuf;

    if (stat(path, &statbuf) != 0) {
        lua_fail_f(L, "stat failed for path %s: %s", errno, path, strerror(errno));
    }

    lua_newtable(L);

    int is_file = S_ISREG(statbuf.st_mode);
    int is_dir = S_ISDIR(statbuf.st_mode);

    lua_pushstring(L, is_file ? "file" : is_dir ? "dir" : "?");
    lua_setfield(L, -2, "type");

    if (is_file) {
        lua_pushnumber(L, statbuf.st_size);
        lua_setfield(L, -2, "size");
    }

    return 1;
}

static int lua_fs_read_dir(lua_State *L) {
    const char *path = luaL_checkstring(L, 1);

    DIR **d = (DIR **)lua_newuserdata(L, sizeof(DIR *));

    luaL_setmetatable(L, LUA_MT_FS_DIR_ITER);

    *d = opendir(path);

    if (*d == NULL) {
        return luaL_error(L, "opendir failed for path %s: %s", path, strerror(errno));
    }

    lua_pushcclosure(L, lua_fs_read_dir_iter, 1);

    return 1;
}

static int lua_fs_read_dir_iter(lua_State *L) {
    DIR *d = *(DIR **)lua_touserdata(L, lua_upvalueindex(1));
    struct dirent *entry;

    do {
        entry = readdir(d);
        if (entry == NULL) return 0; // end of dir
    } while (!strcmp(".", entry->d_name) || !strcmp("..", entry->d_name));

    lua_pushstring(L, entry->d_name);
    lua_pushboolean(L, entry->d_type == DT_DIR); // is dir?

    return 1;
}

static int lua_fs_read_dir__gc(lua_State *L) {
    DIR *d = *(DIR **)lua_touserdata(L, 1);

    if (d) {
        closedir(d);
    }

    return 0;
}

static int lua_fs_traverse_dir(lua_State *L) {
    const char *path = luaL_checkstring(L, 1);
    int should_stop = 0;

    if (!lua_isfunction(L, 2)) {
        return luaL_error(L, "arg #2 must be on_file");
    }

    return lua_fs_traverse_dir_worker(L, path, &should_stop);
}

static int lua_fs_traverse_dir_worker(lua_State *L, const char *base_path, int *should_stop) {
    char full_path[PATH_MAX];
    size_t full_path_len;
    DIR *dir;
    struct dirent *entry;

    dir = opendir(base_path);

    if (dir == NULL) {
        return luaL_error(L, "opendir failed for path %s: %s", base_path, strerror(errno));
    }

    while((entry = readdir(dir))) {
        if (*should_stop) {
            return 0;
        }

        full_path_len = strlen(base_path) + 1 + strlen(entry->d_name);

        if (full_path_len > PATH_MAX) {
            return luaL_error(L, "path too long (%ld) %s%c%s",
                full_path_len, base_path, PATH_SEPARATOR, entry->d_name);
        }

        sprintf(full_path, "%s%c%s", base_path, PATH_SEPARATOR, entry->d_name);

        if (entry->d_type == DT_DIR) {
            if (strcmp(".", entry->d_name) && strcmp("..", entry->d_name)) {
                lua_fs_traverse_dir_worker(L, full_path, should_stop);
            }
        } else if (strcmp(".DS_Store", entry->d_name)) {
            lua_pushvalue(L, 2);
            lua_pushstring(L, entry->d_name);
            lua_pushstring(L, full_path);
            lua_call(L, 2, 1);
            *should_stop = lua_toboolean(L, -1);
            lua_pop(L, 1);
        }
    }

    return 0;
}
