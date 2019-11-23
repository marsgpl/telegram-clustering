#include "lua_fs.h"

LUAMOD_API int luaopen_fs(lua_State *L) {
    luaL_newlib(L, __index);
    return 1;
}

// arg#1 - string - path to stat
static int lua_fs_stat(lua_State *L) {
    const char *path = luaL_checkstring(L, 1);
    struct stat statbuf;

    if (stat(path, &statbuf) != 0) {
        tgnews_lua_fail(L, "stat failed for path '%s': %s", errno, path, strerror(errno));
    }

    lua_newtable(L);

    if (S_ISREG(statbuf.st_mode)) {
        lua_pushstring(L, "file");
        lua_setfield(L, -2, "type");

        lua_pushinteger(L, statbuf.st_size);
        lua_setfield(L, -2, "size");
    } else if (S_ISDIR(statbuf.st_mode)) {
        lua_pushstring(L, "dir");
        lua_setfield(L, -2, "type");
    }

    return 1;
}

// arg#1 - string - dir path to traverse
// arg#2 - function(file_name, file_path) - callback function to call for every file found
static int lua_fs_traverse(lua_State *L) {
    const char *path = luaL_checkstring(L, 1);
    int should_stop = 0;

    if (!lua_isfunction(L, 2)) {
        tgnews_lua_fail(L, "arg #2 must be function(file_name, file_path)", -1);
    }

    return lua_fs_traverse_worker(L, path, &should_stop);
}

static int lua_fs_traverse_worker(lua_State *L, const char *base_path, int *should_stop) {
    char full_path[PATH_MAX];
    size_t full_path_len;
    DIR *dir;
    struct dirent *entry;

    dir = opendir(base_path);

    if (dir == NULL) {
        tgnews_lua_fail(L, "opendir failed for path '%s': %s", errno, base_path, strerror(errno));
    }

    while((entry = readdir(dir))) {
        if (*should_stop) break;

        // full_path_len does not count nul, PATH_MAX does
        full_path_len = strlen(base_path) + 1 + strlen(entry->d_name);

        if (full_path_len > PATH_MAX - 1) {
            tgnews_lua_fail(L, "path '%s%c%s' is too long: %I > %I", 0,
                base_path, PATH_SEPARATOR, entry->d_name,
                (lua_Integer)full_path_len,
                (lua_Integer)(PATH_MAX - 1));
        }

        sprintf(full_path, "%s%c%s", base_path, PATH_SEPARATOR, entry->d_name);

        if (entry->d_type == DT_DIR) {
            if (strcmp(".", entry->d_name) && strcmp("..", entry->d_name)) {
                lua_fs_traverse_worker(L, full_path, should_stop);
            }
        } else if (entry->d_type == DT_REG) {
            if (strcmp(".DS_Store", entry->d_name)) {
                lua_pushvalue(L, 2);
                lua_pushstring(L, entry->d_name);
                lua_pushstring(L, full_path);
                lua_call(L, 2, 1);
                *should_stop = lua_toboolean(L, -1);
                lua_pop(L, 1);
            }
        }
    }

    lua_pushboolean(L, 1);
    return 1;
}
