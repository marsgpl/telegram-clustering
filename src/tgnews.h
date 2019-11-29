#include <signal.h>

#include "shared/tgnews_lua.h"

#define ENTRY_POINT_FILE "logic/tgnews.luac"

#define PACKAGE_PATH "libs/?.luac;logic/?.luac;grams/?.lua"
#define PACKAGE_CPATH "libs/?.so"

#define USAGE "Usage:  tgnews TASK SRCDIR\n" \
"\n" \
"Telegram clustering challenge participant.\n" \
"Executes TASK on files from SRCDIR directory.\n" \
"The result will be sent to STDOUT in JSON format.\n" \
"\n" \
"Tasks:\n" \
"  languages    Isolate articles in English and Russian\n" \
"  news         Isolate news articles\n" \
"  categories   Group news articles by category\n" \
"  threads      Group similar news into threads\n" \
"  top          Sort threads by their relative importance\n"

#define fail(tpl, ...) { \
    fprintf(stderr, "Error: "); \
    fprintf(stderr, tpl, ##__VA_ARGS__); \
    fprintf(stderr, "\n"); \
    fflush(stderr); \
    return 1; \
}

#define fail_lua(L) { \
    fprintf(stderr, "Lua error: %s\n", lua_tostring(L, -1)); \
    fflush(stderr); \
    lua_pop(L, 1); \
    lua_trace_stack(L); \
    lua_close(L); \
    return 1; \
}

#define tgnews_lua_require(L, module_name) { \
    lua_pushcfunction(L, tgnews_lua_pcall_errmsg_handler); \
    lua_getglobal(L, "require"); \
    lua_pushstring(L, module_name); \
    if (lua_pcall(L, 1, 1, -3) != LUA_OK) fail_lua(L); \
    lua_setglobal(L, module_name); \
    lua_pop(L, 1); \
}

#define tgnews_lua_dofile(L, file_path) { \
    lua_pushcfunction(L, tgnews_lua_pcall_errmsg_handler); \
    if (luaL_loadfile(L, file_path) != LUA_OK) fail_lua(L); \
    if (lua_pcall(L, 0, 0, -2) != LUA_OK) fail_lua(L); \
    lua_pop(L, 1); \
}

int main(int argc, const char **argv);

static void tgnews_on_sigint(int sig);
static void tgnews_lua_stop_hook(lua_State *L, lua_Debug *ar);
static int tgnews_lua_pcall_errmsg_handler(lua_State *L);
