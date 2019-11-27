#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

#include "tgnews_lua.h"

LUAMOD_API int luaopen_sys(lua_State *L);

static int lua_sys_unlink(lua_State *L);
static int lua_sys_sleep(lua_State *L);
static int lua_sys_microtime(lua_State *L);

static const luaL_Reg __index[] = {
    { "unlink", lua_sys_unlink },
    { "sleep", lua_sys_sleep },
    { "microtime", lua_sys_microtime },
    { NULL, NULL }
};
