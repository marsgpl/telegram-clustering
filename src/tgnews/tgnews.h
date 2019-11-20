#include <stdio.h>
#include <string.h>
#include <signal.h>

#include "lauxlib.h"
#include "lualib.h"

#define ENTRY_POINT_FILE "logic/tgnews.luac"

#define TASK_LANGUAGES "languages"
#define TASK_NEWS "news"
#define TASK_CATEGORIES "categories"
#define TASK_THREADS "threads"
#define TASK_TOP "top"

#define USAGE "\nUsage:  tgnews TASK SRCDIR\n" \
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
    fprintf(stderr, tpl, ##__VA_ARGS__); \
    fprintf(stderr, "\n"); \
    fflush(stderr); \
    return 1; \
}

#define fail_lua(L) { \
    fail("Error: %s", lua_tostring(L, -1)); \
    lua_pop(L, 1); \
    lua_close(L); \
    return 1; \
}

int main(int argc, const char **argv);
static int tgnews_lua_call_handler(lua_State *L);
static void tgnews_on_sigint(int sig);
static void tgnews_lua_stop(lua_State *L, lua_Debug *ar);
