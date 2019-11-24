#include "lua_etc.h"

LUAMOD_API int luaopen_etc(lua_State *L) {
    luaL_newlib(L, __index);
    return 1;
}

static int lua_etc_unlink(lua_State *L) {
    const char *path = luaL_checkstring(L, 1);

    int r = unlink(path);

    if (r == -1) {
        lua_errno(L);
    }

    lua_pushboolean(L, 1);

    return 1;
}

static int lua_etc_sleep(lua_State *L) {
    double input = luaL_optnumber(L, 1, 0);
    struct timespec t;

    t.tv_sec  = (long)input;
    t.tv_nsec = (long)((input - (long)input) * 1e9);

    if (nanosleep(&t, NULL) == -1) {
        lua_errno(L);
    }

    return 1;
}

// CLOCK_REALTIME
// CLOCK_REALTIME_COARSE
// CLOCK_MONOTONIC
// CLOCK_MONOTONIC_COARSE
// CLOCK_MONOTONIC_RAW
// CLOCK_BOOTTIME
// CLOCK_PROCESS_CPUTIME_ID
// CLOCK_THREAD_CPUTIME_ID
// old style:
    // #include <sys/time.h>
    // struct timeval tv;
    // gettimeofday(&tv, NULL);
    // lua_pushnumber(L, tv.tv_sec + tv.tv_usec * 1e-6);
static int lua_etc_microtime(lua_State *L) {
    struct timespec t;

    if (clock_gettime(CLOCK_REALTIME, &t) == -1) {
        lua_errno(L);
    }

    lua_pushnumber(L, t.tv_sec + t.tv_nsec * 1e-9);

    return 1;
}
