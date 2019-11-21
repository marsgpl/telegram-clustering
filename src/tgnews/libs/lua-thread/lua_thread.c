#include "lua_thread.h"

LUAMOD_API int luaopen_thread(lua_State *L) {
    lua_newmt(L, LUA_MT_THREAD, __thread_index, lua_thread_gc);

    luaL_newlib(L, __index);
    return 1;
}

// arg#1 - string - file to execute
// arg#2 - table - meta args
// arg#3 - table - meta keys to copy
static int lua_thread_start(lua_State *L) {
    int r;

    luaL_checkstring(L, 1);

    lua_ud_thread *thread = (lua_ud_thread *)lua_newuserdata(L, sizeof(lua_ud_thread));

    if (!thread) {
        lua_fail(L, "lua_ud_thread alloc failed", 0);
    }

    thread->L = luaL_newstate();

    if (!thread->L) {
        lua_fail(L, "luaL_newstate alloc failed", 0);
    }

    thread->id = inc_id();
    thread->state = LUA_THREAD_STATE_RUNNING;

    lua_atpanic(thread->L, lua_thread_atpanic);
    luaL_openlibs(thread->L);

    lua_getglobal(L, "package");
    lua_getfield(L, -1, "path");
    lua_getfield(L, -2, "cpath");

    lua_getglobal(thread->L, "package");
    lua_pushstring(thread->L, luaL_checkstring(L, -1));
    lua_setfield(thread->L, -2, "cpath");
    lua_pushstring(thread->L, luaL_checkstring(L, -2));
    lua_setfield(thread->L, -2, "path");

    lua_pop(thread->L, 1);
    lua_pop(L, 3);

    // meta id
    lua_pushnumber(thread->L, thread->id);
    lua_setfield(thread->L, LUA_REGISTRYINDEX, LUA_THREAD_ID_METAFIELD);

    // meta args
    if (lua_istable(L, 2)) {
        lua_thread_xcopy(L, 2, thread->L);
    } else {
        lua_newtable(thread->L);
    }
    lua_setfield(thread->L, LUA_REGISTRYINDEX, LUA_THREAD_ARGS_METAFIELD);

    // meta keys to copy - example - { "_zmq_ctx" }
    if (lua_istable(L, 3)) {
        lua_pushnil(L);
        while (lua_next(L, 3) != 0) {
            r = lua_getfield(L, LUA_REGISTRYINDEX, lua_tostring(L, -1));
            if (r==LUA_TLIGHTUSERDATA) {
                lua_pushlightuserdata(thread->L, lua_touserdata(L, -1));
                lua_setfield(thread->L, LUA_REGISTRYINDEX, lua_tostring(L, -2));
            }

            lua_pop(L, 2); // +1 for lua_next, +1 for lua_getfield(registry)
        }
    }

    // push file path at the top of the stack
    lua_pushstring(thread->L, lua_tostring(L, 1));

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    r = pthread_create(&thread->thread, &attr, lua_thread_create_worker, thread);

    pthread_attr_destroy(&attr);

    if (r != 0) {
        lua_close(thread->L);
        lua_fail(L, "thread creation failed", r);
    }

    luaL_setmetatable(L, LUA_MT_THREAD);

    lua_pushnumber(L, thread->id);

    return 2;
}

static int lua_thread_args(lua_State *L) {
    lua_getfield(L, LUA_REGISTRYINDEX, LUA_THREAD_ARGS_METAFIELD);
    return 1;
}

static int lua_thread_id(lua_State *L) {
    lua_getfield(L, LUA_REGISTRYINDEX, LUA_THREAD_ID_METAFIELD);
    return 1;
}

static int lua_thread_stop(lua_State *L) {
    lua_ud_thread *thread = luaL_checkudata(L, 1, LUA_MT_THREAD);

    if (__sync_or_and_fetch(&thread->state, 0) == LUA_THREAD_STATE_RUNNING) {
        pthread_detach(thread->thread);
        pthread_cancel(thread->thread);
        __sync_bool_compare_and_swap(&thread->state, LUA_THREAD_STATE_RUNNING, LUA_THREAD_STATE_DEAD);
    }

    lua_settop(L, 1);
    return 1;
}

static int lua_thread_join(lua_State *L) {
    lua_ud_thread *thread = luaL_checkudata(L, 1, LUA_MT_THREAD);

    if (__sync_or_and_fetch(&thread->state, 0) == LUA_THREAD_STATE_RUNNING) {
        pthread_join(thread->thread, NULL);
    }

    lua_settop(L, 1);
    return 1;
}

static int lua_thread_gc(lua_State *L) {
    lua_ud_thread *thread = luaL_checkudata(L, 1, LUA_MT_THREAD);

    if (__sync_or_and_fetch(&thread->state, 0) == LUA_THREAD_STATE_RUNNING) {
        pthread_join(thread->thread, NULL);
    }

    if (thread->L) {
        lua_close(thread->L);
        thread->L = NULL;
    }

    return 0;
}

static uint64_t inc_id(void) {
    static volatile uint64_t id = 0;
    return __sync_add_and_fetch(&id, 1);
}

static void *lua_thread_create_worker(void *arg) {
    int r;

    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);

    lua_ud_thread *thread = (lua_ud_thread *)arg;
    lua_State *L = thread->L;

    r = luaL_loadfilex(L, lua_tostring(L, 1), "bt");

    if (r != LUA_OK) {
        lua_thread_atpanic(L);
    } else {
        r = lua_custom_pcall(L, 0, 0);

        if (r != LUA_OK) {
            lua_thread_atpanic(L);
        }
    }

    __sync_bool_compare_and_swap(&thread->state, LUA_THREAD_STATE_RUNNING, LUA_THREAD_STATE_DEAD);

    pthread_exit(NULL);
}

static int lua_thread_atpanic(lua_State *L) {
    lua_getfield(L, LUA_REGISTRYINDEX, LUA_THREAD_ID_METAFIELD);
    fprintf(stderr, "lua thread #%zu: PANIC: %s\n", (size_t)lua_tonumber(L, -1), lua_tostring(L, -2));
    return 0;
}

// LUA_TNONE
// LUA_TNIL
// LUA_TNUMBER
// LUA_TBOOLEAN
// LUA_TSTRING
// LUA_TTABLE
// LUA_TFUNCTION
// LUA_TUSERDATA
// LUA_TTHREAD
// LUA_TLIGHTUSERDATA
static void lua_thread_xcopy(lua_State *fromL, int fromIndex, lua_State *toL) {
    int type = lua_type(fromL, fromIndex);
    const char *str;
    size_t len;
    int pos;

    if (type == LUA_TNUMBER) {
        lua_pushnumber(toL, lua_tonumber(fromL, fromIndex));
    } else if (type == LUA_TBOOLEAN) {
        lua_pushboolean(toL, lua_toboolean(fromL, fromIndex));
    } else if (type == LUA_TSTRING) {
        str = lua_tolstring(fromL, fromIndex, &len);
        lua_pushlstring(toL, str, len);
    } else if (type == LUA_TFUNCTION
        || type == LUA_TUSERDATA
        || type == LUA_TTHREAD
        || type == LUA_TLIGHTUSERDATA
    ) {
        lua_pushstring(toL, "not supported");
    } else if (type == LUA_TTABLE) {
        lua_newtable(toL);

        lua_pushnil(fromL);
        while (lua_next(fromL, fromIndex) != 0) {
            pos = lua_gettop(fromL);
            lua_thread_xcopy(fromL, pos - 1, toL);
            lua_thread_xcopy(fromL, pos, toL);
            lua_settable(toL, -3);
            lua_pop(fromL, 1);
        }
    } else { // LUA_TNONE, LUA_TNIL
        lua_pushnil(toL);
    }
}

static int lua_custom_traceback(lua_State *L) {
    const char *msg = lua_tostring(L, 1);
    if (msg) {
        luaL_traceback(L, L, msg, 1);
    }
    return 1;
}

static int lua_custom_pcall(lua_State *L, int narg, int nres) {
    int base = lua_gettop(L) - narg; // traceback index
    lua_pushcfunction(L, lua_custom_traceback);
    lua_insert(L, base);
    int r = lua_pcall(L, narg, nres, base);
    lua_remove(L, base);
    return r;
}
