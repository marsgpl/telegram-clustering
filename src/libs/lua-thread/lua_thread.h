#include <pthread.h>

#include "tgnews_lua.h"

#define LUA_MT_THREAD "mt.thread"

#define LUA_THREAD_ID_METAFIELD "_thread_id"
#define LUA_THREAD_ARGS_METAFIELD "_thread_args"

#define LUA_THREAD_STATE_RUNNING 0
#define LUA_THREAD_STATE_DEAD 1

#define lua_thread_require(L, module_name) { \
    lua_pushcfunction(L, lua_thread_pcall_errmsg_handler); \
    lua_getglobal(L, "require"); \
    lua_pushstring(L, module_name); \
    if (lua_pcall(L, 1, 1, -3) != LUA_OK) { \
        lua_thread_atpanic(L); \
        lua_thread_on_end(thread); \
    } \
    lua_setglobal(L, module_name); \
    lua_pop(L, 1); \
}

#define lua_thread_dofile(L, file_path) { \
    lua_pushcfunction(L, lua_thread_pcall_errmsg_handler); \
    if (luaL_loadfile(L, file_path) != LUA_OK) { \
        lua_thread_atpanic(L); \
        lua_thread_on_end(thread); \
    } \
    if (lua_pcall(L, 0, 0, -2) != LUA_OK) { \
        lua_thread_atpanic(L); \
        lua_thread_on_end(thread); \
    } \
    lua_pop(L, 1); \
}

#define lua_thread_on_end(thread) { \
    __sync_bool_compare_and_swap(&thread->state, LUA_THREAD_STATE_RUNNING, LUA_THREAD_STATE_DEAD); \
    pthread_exit(NULL); \
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
static int lua_thread_pcall_errmsg_handler(lua_State *L);

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
