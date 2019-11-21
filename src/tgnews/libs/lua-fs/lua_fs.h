#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>

#include "lauxlib.h"

#define LUA_FS_MT_DIR_ITERATOR "fs.mt.dir.iter"

#define PATH_SEPARATOR '/'

LUAMOD_API int luaopen_fs(lua_State *L);

static int lua_fs_readdir(lua_State *L);
static int lua_fs_readdir_iter(lua_State *L);
static int lua_fs_readdir__gc(lua_State *L);
static int lua_fs_isdir(lua_State *L);
static int lua_fs_traverse(lua_State *L);

static const luaL_Reg __index[] = {
    { "readdir", lua_fs_readdir },
    { "isdir", lua_fs_isdir },
    { "traverse", lua_fs_traverse },
    { NULL, NULL }
};
