#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdint.h>
#include <time.h>

#include "utf8proc/utf8proc.h"
#include "tgnews_lua.h"

LUAMOD_API int luaopen_text(lua_State *L);

static int lua_text_parse_url(lua_State *L);
static int lua_text_strip_tags(lua_State *L);
static int lua_text_strip_chars(lua_State *L);
static int lua_text_replace_chars(lua_State *L);
static int lua_text_collapse_whitespace(lua_State *L);
static int lua_text_strip_whitespace(lua_State *L);
static int lua_text_normalize(lua_State *L);
static int lua_text_find_grams(lua_State *L);

static const luaL_Reg __index[] = {
    { "parse_url", lua_text_parse_url },
    { "strip_tags", lua_text_strip_tags },
    { "strip_chars", lua_text_strip_chars },
    { "replace_chars", lua_text_replace_chars },
    { "collapse_whitespace", lua_text_collapse_whitespace },
    { "strip_whitespace", lua_text_strip_whitespace },
    { "normalize", lua_text_normalize },
    { "find_grams", lua_text_find_grams },
    { NULL, NULL }
};
