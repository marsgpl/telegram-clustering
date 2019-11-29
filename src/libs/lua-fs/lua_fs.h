#include <dirent.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "tgnews_lua.h"

#define PATH_SEPARATOR '/'

LUAMOD_API int luaopen_fs(lua_State *L);

static int lua_fs_traverse(lua_State *L);
static int lua_fs_traverse_worker(lua_State *L, const char *path, int *should_stop);
static int lua_fs_stat(lua_State *L);
static int lua_fs_readfile(lua_State *L);
static int lua_fs_writefile(lua_State *L);

static const luaL_Reg __index[] = {
    { "traverse", lua_fs_traverse },
    { "stat", lua_fs_stat },
    { "readfile", lua_fs_readfile },
    { "writefile", lua_fs_writefile },
    { NULL, NULL }
};
