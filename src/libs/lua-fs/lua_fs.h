#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "lauxlib.h"

#define LUA_MT_FS_DIR_ITER "mt.fs.dir.iter"

#define PATH_SEPARATOR '/'

#define lua_fail_f(L, errstr, errno, ...) { \
    lua_pushnil(L); \
    lua_pushfstring(L, errstr, ##__VA_ARGS__); \
    lua_pushinteger(L, errno); \
    return 3; \
}

LUAMOD_API int luaopen_fs(lua_State *L);

static int lua_fs_read_dir(lua_State *L);
static int lua_fs_read_dir_iter(lua_State *L);
static int lua_fs_read_dir__gc(lua_State *L);
static int lua_fs_traverse_dir(lua_State *L);
static int lua_fs_traverse_dir_worker(lua_State *L, const char *path, int *should_stop);
static int lua_fs_stat(lua_State *L);

static const luaL_Reg __index[] = {
    { "read_dir", lua_fs_read_dir },
    { "traverse_dir", lua_fs_traverse_dir },
    { "stat", lua_fs_stat },
    { NULL, NULL }
};
