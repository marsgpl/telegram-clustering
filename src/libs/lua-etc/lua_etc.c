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

static int is_alpha(const char *str, size_t len) {
    for (int i = 0; i<len; ++i) {
        if (!isalpha(str[i])) {
            return 0;
        }
    }

    return 1;
}

//                    |            | url_len
// schema://user:pass@host.zone:port/path?query#hash
// |                  |        |
// url                start    pos
static int lua_etc_parse_url(lua_State *L) {
    size_t url_len;
    const char *url = luaL_checklstring(L, 1, &url_len);

    char *pos, *posx;
    size_t len;
    int start = 0;
    char port[6];

    lua_newtable(L);

    pos = strchr(url, '#');

    if (pos != NULL) {
        len = url_len - (pos - url); // including #
        url_len -= len;

        lua_pushlstring(L, pos + 1, len - 1);
        lua_setfield(L, -2, "hash");
    }

    pos = strchr(url, '?');

    if (pos != NULL && pos - url < url_len) {
        len = url_len - (pos - url); // including ?
        url_len -= len;

        lua_pushlstring(L, pos + 1, len - 1);
        lua_setfield(L, -2, "query");
    }

    pos = strstr(url, "//");

    if (pos == url) {
        // auto-scheme url: //domain.com
        start += 2;
        url_len -= 2;
    } else {
        pos = strstr(url, "://");

        if (pos != NULL && pos - url < url_len) {
            len = pos - url;

            if (is_alpha(url, len)) {
                start += len + 3;
                url_len -= len + 3;

                lua_pushlstring(L, url, len);
                lua_setfield(L, -2, "schema");
            }
        }
    }

    pos = strchr(url + start, '/');

    if (pos != NULL && (pos - url - start) < url_len) {
        len = url_len - (pos - url - start);
        url_len -= len;

        lua_pushlstring(L, pos, len);
        lua_setfield(L, -2, "path");
    }

    pos = strchr(url + start, '@');

    if (pos != NULL && (pos - url - start) < url_len) {
        len = pos - url - start; // user:pass
        posx = strchr(url + start, ':');

        if (posx != NULL && posx < pos) {
            lua_pushlstring(L, url + start, posx - url - start);
            lua_setfield(L, -2, "user");

            lua_pushlstring(L, posx + 1, pos - posx - 1);
            lua_setfield(L, -2, "pass");
        } else {
            // no : sep for user/pass
            // treat as long username
            lua_pushlstring(L, url + start, len);
            lua_setfield(L, -2, "user");
        }

        start += len + 1; // + @
        url_len -= len + 1; // + @
    }

    pos = strchr(url + start, ':');

    if (pos != NULL && (pos - url - start) < url_len) {
        len = pos - url - start;

        lua_pushlstring(L, url + start, len);
        lua_setfield(L, -2, "host");

        len = url_len - len - 1; // port len

        if (len > 5) { // do not push invalid port
            // lua_pushlstring(L, pos + 1, len);
            // lua_setfield(L, -2, "port");
        } else {
            memcpy(port, pos + 1, len);

            lua_pushinteger(L, atoi(port));
            lua_setfield(L, -2, "port");
        }

        url_len -= len + 1;
    } else {
        lua_pushlstring(L, url + start, url_len);
        lua_setfield(L, -2, "host");
    }

    posx = NULL;

    while (1) {
        pos = strchr(url + start, '.');
        if (pos == NULL) break;
        len = pos - url - start;
        if (len >= url_len) break;
        start += len + 1;
        url_len -= len + 1;
        posx = pos;
    }

    if (posx != NULL) {
        lua_pushlstring(L, url + start, url_len);
        lua_setfield(L, -2, "zone");
    }

    return 1;
}
