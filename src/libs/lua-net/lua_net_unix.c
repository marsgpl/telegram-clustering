// arg#1 - nonblock - 1 == socket is nonblocking, 0 or nil == blocking
static int lua_net_unix_socket(lua_State *L) {
    int nonblock = luaL_optinteger(L, 1, 0) ? SOCK_NONBLOCK : 0; // default: blocking

    lua_ud_unix_socket *sock = (lua_ud_unix_socket *)lua_newuserdata(L, sizeof(lua_ud_unix_socket));

    if (sock == NULL) {
        lua_fail(L, "lua_ud_unix_socket alloc failed", 0);
    }

    sock->fd = socket(AF_UNIX, SOCK_STREAM | nonblock, 0);
    sock->id = inc_id();
    sock->addr = NULL;

    if (sock->fd == -1) {
        lua_errno(L);
    }

    luaL_setmetatable(L, LUA_MT_NET_UNIX_SOCKET);

    return 1;
}

static int lua_net_unix_socket_fd(lua_State *L) {
    lua_ud_unix_socket *sock = luaL_checkudata(L, 1, LUA_MT_NET_UNIX_SOCKET);

    lua_pushinteger(L, sock->fd);

    return 1;
}

static int lua_net_unix_socket_id(lua_State *L) {
    lua_ud_unix_socket *sock = luaL_checkudata(L, 1, LUA_MT_NET_UNIX_SOCKET);

    lua_pushinteger(L, sock->id);

    return 1;
}

static int lua_net_unix_socket_path(lua_State *L) {
    lua_ud_unix_socket *sock = luaL_checkudata(L, 1, LUA_MT_NET_UNIX_SOCKET);

    lua_pushstring(L, sock->addr);

    return 1;
}

static int lua_net_unix_socket_close(lua_State *L) {
    lua_ud_unix_socket *sock = luaL_checkudata(L, 1, LUA_MT_NET_UNIX_SOCKET);

    if (sock->fd != -1) {
        close(sock->fd);
        sock->fd = -1;
    }

    if (sock->addr != NULL) {
        unlink(sock->addr);
        free(sock->addr);
        sock->addr = NULL;
    }

    // struct sockaddr_un addr = {0};
    // socklen_t addr_len = sizeof(addr);
    // int r = getsockname(sock->fd, (struct sockaddr *)&addr, &addr_len);
    // if (r==0 && strlen(addr.sun_path) > 0) {
    //     unlink(addr.sun_path);
    // }

    return 0;
}

// arg#1 - path - nil==/tmp/lua-net.sock
// arg#2 - mode - nil==666
static int lua_net_unix_socket_bind(lua_State *L) {
    lua_ud_unix_socket *sock = luaL_checkudata(L, 1, LUA_MT_NET_UNIX_SOCKET);
    size_t path_len;
    const char *path = luaL_optlstring(L, 2, UNIX_SOCKET_BIND_DEFAULT_ADDR, &path_len);
    long int mode = oct2dec(luaL_optinteger(L, 3, UNIX_SOCKET_BIND_DEFAULT_MODE));

    if (sock->fd == -1) {
        lua_fail(L, "socket id#%d is dead", 0, sock->id);
    }

    int r;
    struct sockaddr_un addr = {0};

    addr.sun_family = AF_UNIX;

    if (path_len > sizeof(addr.sun_path) - 1) {
        lua_fail(L, "unix socket path '%s' is too long: %I > %I", 0,
            path,
            (lua_Integer)path_len,
            (lua_Integer)(sizeof(addr.sun_path) - 1));
    }

    strncpy(addr.sun_path, path, path_len);

    r = bind(sock->fd, (struct sockaddr *)&addr, sizeof(addr));

    if (r == -1) {
        lua_errno(L);
    }

    sock->addr = malloc(path_len+1);

    if (sock->addr == NULL) {
        lua_fail(L, "malloc failed for sock->addr", 0);
    }

    strncpy(sock->addr, path, path_len);

    sock->addr[path_len] = 0;

    r = chmod(path, (mode_t)mode);

    if (r == -1) {
        lua_errno(L);
    }

    lua_settop(L, 1);

    return 1;
}

// arg#1 - path
static int lua_net_unix_socket_connect(lua_State *L) {
    lua_ud_unix_socket *sock = luaL_checkudata(L, 1, LUA_MT_NET_UNIX_SOCKET);
    size_t path_len;
    const char *path = luaL_optlstring(L, 2, UNIX_SOCKET_BIND_DEFAULT_ADDR, &path_len);

    if (sock->fd == -1) {
        lua_fail(L, "socket id#%d is dead", 0, sock->id);
    }

    int r;
    struct sockaddr_un addr = {0};

    addr.sun_family = AF_UNIX;

    if (path_len > sizeof(addr.sun_path) - 1) {
        lua_fail(L, "unix socket path '%s' is too long: %I > %I", 0,
            path,
            (lua_Integer)path_len,
            (lua_Integer)(sizeof(addr.sun_path) - 1));
    }

    strncpy(addr.sun_path, path, path_len);

    r = connect(sock->fd, (struct sockaddr *)&addr, sizeof(addr));

    if (r == -1) {
        lua_errno(L);
    } else {
        lua_settop(L, 1);
        return 1;
    }
}

// arg#1 - backlog - nil==1024
static int lua_net_unix_socket_listen(lua_State *L) {
    lua_ud_unix_socket *sock = luaL_checkudata(L, 1, LUA_MT_NET_UNIX_SOCKET);
    int backlog = luaL_optinteger(L, 2, UNIX_SOCKET_LISTEN_DEFAULT_BACKLOG);

    if (sock->fd == -1) {
        lua_fail(L, "socket id#%d is dead", 0, sock->id);
    }

    int r = listen(sock->fd, backlog);

    if (r == -1) {
        lua_errno(L);
    } else {
        lua_settop(L, 1);
        return 1;
    }
}

// arg#1 - nonblock - 1 == socket is nonblocking, 0 or nil == blocking
static int lua_net_unix_socket_accept(lua_State *L) {
    lua_ud_unix_socket *sock = luaL_checkudata(L, 1, LUA_MT_NET_UNIX_SOCKET);
    int nonblock = luaL_optinteger(L, 2, 0) ? SOCK_NONBLOCK : 0; // default: blocking

    if (sock->fd == -1) {
        lua_fail(L, "socket id#%d is dead", 0, sock->id);
    }

    int fd = accept(sock->fd, NULL, NULL);

    if (fd == -1) {
        lua_errno(L);
    }

    if (nonblock) { // crutch for missing accept4
        if (fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK) == -1) {
            lua_errno(L);
        }
    }

    lua_ud_unix_socket *client = (lua_ud_unix_socket *)lua_newuserdata(L, sizeof(lua_ud_unix_socket));

    if (client == NULL) {
        close(fd);
        lua_fail(L, "lua_ud_unix_socket alloc failed", 0);
    }

    client->fd = fd;
    client->id = inc_id();
    client->addr = NULL;

    luaL_setmetatable(L, LUA_MT_NET_UNIX_SOCKET);

    return 1;
}

static int lua_net_unix_socket_recv(lua_State *L) {
    lua_ud_unix_socket *sock = luaL_checkudata(L, 1, LUA_MT_NET_UNIX_SOCKET);

    if (sock->fd == -1) {
        lua_fail(L, "socket id#%d is dead", 0, sock->id);
    }

    char read_buf[UNIX_SOCKET_RECV_BUF_LEN];
    ssize_t n;

    do { n = recv(sock->fd, (void *)read_buf, UNIX_SOCKET_RECV_BUF_LEN, MSG_NOSIGNAL); }
    while (n == -1 && errno==EINTR); // if EINTR - continue

    if (n == -1) {
        lua_errno(L);
    } else {
        lua_pushlstring(L, read_buf, n);
        lua_pushnumber(L, n); // bytes rcvd
        return 2;
    }
}

// arg#1 - msg - string
static int lua_net_unix_socket_send(lua_State *L) {
    lua_ud_unix_socket *sock = luaL_checkudata(L, 1, LUA_MT_NET_UNIX_SOCKET);

    if (sock->fd == -1) {
        lua_fail(L, "socket id#%d is dead", 0, sock->id);
    }

    size_t msg_len;
    const char *msg = luaL_checklstring(L, 2, &msg_len);
    ssize_t n;

    do { n = send(sock->fd, msg, msg_len, MSG_NOSIGNAL); }
    while (n == -1 && errno==EINTR); // if EINTR - continue

    if (n == -1) {
        lua_errno(L);
    } else {
        lua_pushnumber(L, n); // bytes sent
        return 1;
    }
}

// arg#1 - flag (integer const)
// arg#2 - value
static int lua_net_unix_socket_set(lua_State *L) {
    lua_ud_unix_socket *sock = luaL_checkudata(L, 1, LUA_MT_NET_UNIX_SOCKET);
    int optname = luaL_checkinteger(L, 2);

    if (sock->fd == -1) {
        lua_fail(L, "socket id#%d is dead", 0, sock->id);
    }

    int fd = sock->fd;
    int r, optval, flags;
    double optdval;
    struct linger optlinger;
    struct timeval opttv;

    switch (optname) {
        case O_NONBLOCK:
            optval = luaL_checkinteger(L, 3);
            flags = fcntl(fd, F_GETFL);
            r = fcntl(fd, F_SETFL, optval ? (flags | O_NONBLOCK) : (flags & ~O_NONBLOCK));
            break;
        case SO_RCVTIMEO:
        case SO_SNDTIMEO:
            optdval = luaL_optnumber(L, 3, 0);
            opttv.tv_sec = (int)optdval;
            opttv.tv_usec = (int)((optdval-opttv.tv_sec)*1e6);
            r = setsockopt(fd, SOL_SOCKET, optname, &opttv, sizeof(opttv));
            break;
        case SO_LINGER:
            optlinger.l_onoff = luaL_checkinteger(L, 3);
            optlinger.l_linger = luaL_checkinteger(L, 4);
            r = setsockopt(fd, SOL_SOCKET, optname, &optlinger, sizeof(optlinger));
            break;
        default:
            optval = luaL_checkinteger(L, 3);
            r = setsockopt(fd, SOL_SOCKET, optname, &optval, sizeof(optval));
            break;
    }

    if (r == -1) {
        lua_errno(L);
    } else {
        lua_settop(L, 1);
        return 1;
    }
}
