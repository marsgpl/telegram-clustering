#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

#include "tgnews_lua.h"

LUAMOD_API int luaopen_etc(lua_State *L);

static int lua_etc_unlink(lua_State *L);
static int lua_etc_sleep(lua_State *L);
static int lua_etc_microtime(lua_State *L);
static int lua_etc_parse_url(lua_State *L);

static int is_alpha(const char *str, size_t len);

static const luaL_Reg __index[] = {
    { "unlink", lua_etc_unlink },
    { "sleep", lua_etc_sleep },
    { "microtime", lua_etc_microtime },
    { "parse_url", lua_etc_parse_url },
    { NULL, NULL }
};
