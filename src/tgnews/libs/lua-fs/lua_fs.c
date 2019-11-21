#include "lua_fs.h"

LUAMOD_API int luaopen_fs(lua_State *L) {
    luaL_newmetatable(L, LUA_FS_MT_DIR_ITERATOR);
    lua_pushcfunction(L, lua_fs_readdir__gc);
    lua_setfield(L, -2, "__gc");
    lua_pop(L, 1);

    luaL_newlib(L, __index);

    return 1;
}

static int lua_fs_readdir(lua_State *L) {
    const char *path = luaL_checkstring(L, 1);

    DIR **d = (DIR **)lua_newuserdata(L, sizeof(DIR *));

    luaL_setmetatable(L, LUA_FS_MT_DIR_ITERATOR);

    *d = opendir(path);

    if (*d == NULL) {
        return luaL_error(L, "opendir failed for path %s: %s", path, strerror(errno));
    }

    lua_pushcclosure(L, lua_fs_readdir_iter, 1);

    return 1;
}

static int lua_fs_readdir_iter(lua_State *L) {
    DIR *d = *(DIR **)lua_touserdata(L, lua_upvalueindex(1));
    struct dirent *entry;

    // do {
        entry = readdir(d);
        if (entry == NULL) return 0; // end of dir
    // } while (!strcmp(".", entry->d_name) || !strcmp("..", entry->d_name));

    lua_pushstring(L, entry->d_name);

    return 1;
}

static int lua_fs_readdir__gc(lua_State *L) {
    DIR *d = *(DIR **)lua_touserdata(L, 1);

    if (d) {
        closedir(d);
    }

    return 0;
}

static int lua_fs_isdir(lua_State *L) {
    const char *path = luaL_checkstring(L, 1);

    struct stat statbuf;

    if (stat(path, &statbuf) != 0) {
        return luaL_error(L, "stat failed for path %s: %s", path, strerror(errno));
    }

    lua_pushboolean(L, S_ISDIR(statbuf.st_mode));

    return 1;
}

static int lua_fs_traverse(lua_State *L) {
    const char *path = luaL_checkstring(L, 1);

    if (!lua_isfunction(L, 2)) {
        return luaL_error(L, "arg #2 must be on_file");
    }

    struct dirent *ent;
    char subpath[PATH_MAX];
    size_t path_len;

    DIR *dir = opendir(path);

    if (dir == NULL) {
        return luaL_error(L, "opendir failed for path %s: %s", path, strerror(errno));
    }

    while((ent = readdir(dir))) {
        path_len = strlen(path) + 1 + strlen(ent->d_name);

        if (path_len > PATH_MAX) {
            return luaL_error(L, "path too long (%ld) %s%c%s", path_len, path, PATH_SEPARATOR, ent->d_name);
        }

        sprintf(subpath, "%s%c%s", path, PATH_SEPARATOR, ent->d_name);
printf("top: %d; subpath: %s\n", lua_gettop(L), subpath);

        if(ent->d_type == DT_DIR) {
            if(!strcmp("..", ent->d_name) || !strcmp(".", ent->d_name)) {
            } else {
                lua_fs_traverse(L);
            }
        } else if (strcmp(".DS_Store", ent->d_name)) {
            lua_pushvalue(L, 2);
            lua_pushstring(L, subpath);
            lua_pushstring(L, path);
            lua_call(L, 2, 0);
        }
    }

    return 0;
}
