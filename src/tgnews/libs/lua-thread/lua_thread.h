#include <pthread.h>

#include "lualib.h"
#include "lauxlib.h"

#define LUA_MT_THREAD "mt.thread"

#define LUA_THREAD_ID_METAFIELD "_thread_id"
#define LUA_THREAD_ARGS_METAFIELD "_thread_args"

#define LUA_THREAD_STATE_RUNNING 0
#define LUA_THREAD_STATE_DEAD 1

#define lua_fail(L, errstr, errno) { \
    lua_pushnil(L); \
    lua_pushstring(L, errstr); \
    lua_pushinteger(L, errno); \
    return 3; \
}

#define lua_errno(L) { \
    lua_fail(L, strerror(errno), errno); \
}

#define lua_newmt(L, name, __index, __gc) { \
    luaL_newmetatable(L, name); \
    \
    luaL_newlib(L, __index); \
    lua_setfield(L, -2, "__index"); \
    \
    lua_pushcfunction(L, __gc); \
    lua_setfield(L, -2, "__gc"); \
    \
    lua_pop(L, 1); \
}

#define lua_trace_stack(L) { \
    int n = lua_gettop(L); \
    printf("stack size: %d\n", n); \
    int t; \
    for (int i=1; i<=n; ++i) { \
        t = lua_type(L,i); \
        printf("\t%d: %s: ", i, lua_typename(L,t)); \
        if (t==LUA_TNIL||t==LUA_TNONE) printf("nil"); \
        else if (t==LUA_TNUMBER) printf(LUA_NUMBER_FMT, lua_tonumber(L,i)); \
        else if (t==LUA_TSTRING) printf("%s", lua_tostring(L,i)); \
        else if (t==LUA_TBOOLEAN) printf(lua_toboolean(L,i) ? "true" : "false"); \
        else printf("%p", lua_topointer(L,i)); \
        printf("\n"); \
    } \
}

typedef struct lua_ud_thread {
    pthread_t thread;
    lua_State *L;
    uint64_t id; // thread unique id (unique in process scope)
    short state;
} lua_ud_thread;

LUAMOD_API int luaopen_thread(lua_State *L);

static int lua_thread_start(lua_State *L);
static int lua_thread_args(lua_State *L);
static int lua_thread_id(lua_State *L);

static int lua_thread_stop(lua_State *L);
static int lua_thread_join(lua_State *L);
static int lua_thread_gc(lua_State *L);

static uint64_t inc_id(void);
static void *lua_thread_create_worker(void *arg);
static void lua_thread_xcopy(lua_State *fromL, int fromIndex, lua_State *toL);
static int lua_thread_atpanic(lua_State *L);
static int lua_custom_traceback(lua_State *L);
static int lua_custom_pcall(lua_State *L, int narg, int nres);

static const luaL_Reg __index[] = {
    { "start", lua_thread_start },
    { "args", lua_thread_args },
    { "id", lua_thread_id },
    { NULL, NULL }
};

static const luaL_Reg __thread_index[] = {
    { "stop", lua_thread_stop },
    { "join", lua_thread_join },
    { NULL, NULL }
};
