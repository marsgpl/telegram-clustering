#include "lua_net.h"

#include "lua_net_ip4_tcp.c"
#include "lua_net_unix.c"
#include "lua_net_epoll.c"

LUAMOD_API int luaopen_net(lua_State *L) {
    lua_newmt(L, LUA_MT_NET_IP4_TCP_SOCKET, __ip4_tcp_socket_index, lua_net_ip4_tcp_socket_close);
    lua_newmt(L, LUA_MT_NET_UNIX_SOCKET, __unix_socket_index, lua_net_unix_socket_close);
    lua_newmt(L, LUA_MT_NET_EPOLL, __epoll_index, lua_net_epoll_stop);

    luaL_newlib(L, __index);

    luaL_newlib(L, __ip4_index);
        luaL_newlib(L, __ip4_tcp_index);
        lua_setfield(L, -2, "tcp");
    lua_setfield(L, -2, "ip4");

    luaL_newlib(L, __unix_index);
    lua_setfield(L, -2, "unix");

    lua_newtable(L);
        #include "lua_net_flags.c"
    lua_setfield(L, -2, "f");

    lua_newtable(L);
        #include "lua_net_errors.c"
    lua_setfield(L, -2, "e");

    return 1;
}

// arg#1 - buffer
// arg#2 - separator
// arg#3 - on_data
// res#1 = buffer after subtracting packets
// res#2 = disconnected (bool)
static int lua_net_splitby(lua_State *L) {
    int disconnected = 0;
    size_t buf_len, sep_len;

    const char *buf = lua_tolstring(L, 1, &buf_len);
    const char *separator = lua_tolstring(L, 2, &sep_len);

    if (sep_len == 0) lua_fail(L, "arg #2 must be separator with length > 0", -1);
    if (!lua_isfunction(L, 3)) lua_fail(L, "arg #3 must be on_data(packet)", -1);

    char sep = separator[0];
    char *sep_pos;
    int sep_index; // sep pos index in buf starting from 0
    int buf_len_changed = 0;

    while (buf_len > 0 && !disconnected) {
        sep_pos = strchr(buf, sep);

        if (sep_pos == NULL) { // sep not found
            break;
        } else {
            sep_index = sep_pos - buf;
            lua_pushvalue(L, -1); // on_data
            lua_pushlstring(L, buf, sep_index); // packet
            lua_call(L, 1, 1); // 1 arg (chunk), 1 result (disconnected)
            disconnected = lua_toboolean(L, -1);
            lua_pop(L, 1);
            buf = sep_pos + 1;
            buf_len -= sep_index + 1;
            buf_len_changed = 1;
        }
    }

    if (buf_len_changed) {
        lua_pushlstring(L, buf, buf_len);
    } else {
        lua_settop(L, 1); // leave in stack only buffer
    }

    lua_pushboolean(L, disconnected);

    return 2;
}

static uint64_t inc_id(void) {
    static volatile uint64_t id = 0;
    return __sync_add_and_fetch(&id, 1);
}

long int oct2dec(long int octal) {
    long int decimal = 0;
    int i = 0;

    while (octal) {
        decimal += (octal % 10) * pow(8, i++);
        octal /= 10;
    }

    return decimal;
}
