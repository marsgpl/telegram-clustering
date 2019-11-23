#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <signal.h>
#include <math.h>
#include <netdb.h>

#include "tgnews_lua.h"

#define LUA_MT_NET_IP4_TCP_SOCKET "mt.net.ip4.tcp.socket"
#define LUA_MT_NET_UNIX_SOCKET "mt.net.unix.socket"
#define LUA_MT_NET_EPOLL "mt.net.epoll"

#define IP4_LOCALHOST "127.0.0.1"
#define IP4_TCP_SOCKET_BIND_DEFAULT_INTERFACE IP4_LOCALHOST
#define IP4_TCP_SOCKET_BIND_DEFAULT_PORT 0
#define IP4_TCP_SOCKET_LISTEN_DEFAULT_BACKLOG 1024
#define IP4_TCP_SOCKET_RECV_BUF_LEN 4096

#define UNIX_SOCKET_BIND_DEFAULT_ADDR "/tmp/lua-net.sock"
#define UNIX_SOCKET_BIND_DEFAULT_MODE 666
#define UNIX_SOCKET_LISTEN_DEFAULT_BACKLOG 1024
#define UNIX_SOCKET_RECV_BUF_LEN 4096

#define EPOLL_DEFAULT_QUEUE_SIZE 65536

typedef struct lua_ud_socket {
    int fd; // file descriptor
    uint64_t id; // unique id (unique across lua_State)
} lua_ud_socket;

typedef struct lua_ud_unix_socket {
    int fd;
    uint64_t id;
    char *addr;
} lua_ud_unix_socket;

LUAMOD_API int luaopen_net(lua_State *L);

static int lua_net_ip4_tcp_socket(lua_State *L);
static int lua_net_ip4_tcp_socket_fd(lua_State *L);
static int lua_net_ip4_tcp_socket_id(lua_State *L);
static int lua_net_ip4_tcp_socket_close(lua_State *L); // == __gc
static int lua_net_ip4_tcp_socket_bind(lua_State *L);
static int lua_net_ip4_tcp_socket_connect(lua_State *L);
static int lua_net_ip4_tcp_socket_listen(lua_State *L);
static int lua_net_ip4_tcp_socket_accept(lua_State *L);
static int lua_net_ip4_tcp_socket_recv(lua_State *L);
static int lua_net_ip4_tcp_socket_send(lua_State *L);
static int lua_net_ip4_tcp_socket_set(lua_State *L);

static int lua_net_unix_socket(lua_State *L);
static int lua_net_unix_socket_fd(lua_State *L);
static int lua_net_unix_socket_id(lua_State *L);
static int lua_net_unix_socket_path(lua_State *L);
static int lua_net_unix_socket_close(lua_State *L);
static int lua_net_unix_socket_bind(lua_State *L);
static int lua_net_unix_socket_connect(lua_State *L);
static int lua_net_unix_socket_listen(lua_State *L);
static int lua_net_unix_socket_accept(lua_State *L);
static int lua_net_unix_socket_recv(lua_State *L);
static int lua_net_unix_socket_send(lua_State *L);
static int lua_net_unix_socket_set(lua_State *L);

static uint64_t inc_id(void);
long int oct2dec(long int octal);

static const luaL_Reg __index[] = {
    { NULL, NULL }
};

static const luaL_Reg __ip4_index[] = {
    { NULL, NULL }
};

static const luaL_Reg __ip4_tcp_index[] = {
    { "socket", lua_net_ip4_tcp_socket },
    { NULL, NULL }
};

static const luaL_Reg __ip4_tcp_socket_index[] = {
    { "fd", lua_net_ip4_tcp_socket_fd },
    { "id", lua_net_ip4_tcp_socket_id },
    { "close", lua_net_ip4_tcp_socket_close },
    { "bind", lua_net_ip4_tcp_socket_bind },
    { "connect", lua_net_ip4_tcp_socket_connect },
    { "listen", lua_net_ip4_tcp_socket_listen },
    { "accept", lua_net_ip4_tcp_socket_accept },
    { "recv", lua_net_ip4_tcp_socket_recv },
    { "send", lua_net_ip4_tcp_socket_send },
    { "set", lua_net_ip4_tcp_socket_set },
    { NULL, NULL }
};

static const luaL_Reg __unix_index[] = {
    { "socket", lua_net_unix_socket },
    { NULL, NULL }
};

static const luaL_Reg __unix_socket_index[] = {
    { "fd", lua_net_unix_socket_fd },
    { "id", lua_net_unix_socket_id },
    { "path", lua_net_unix_socket_path },
    { "close", lua_net_unix_socket_close },
    { "bind", lua_net_unix_socket_bind },
    { "connect", lua_net_unix_socket_connect },
    { "listen", lua_net_unix_socket_listen },
    { "accept", lua_net_unix_socket_accept },
    { "recv", lua_net_unix_socket_recv },
    { "send", lua_net_unix_socket_send },
    { "set", lua_net_unix_socket_set },
    { NULL, NULL }
};
