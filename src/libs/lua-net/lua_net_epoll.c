static int lua_net_epoll(lua_State *L) {
    lua_ud_socket *ep = (lua_ud_socket *)lua_newuserdata(L, sizeof(lua_ud_socket));

    if (ep == NULL) {
        lua_fail(L, "lua_ud_socket(epoll) alloc failed", 0);
    }

    ep->fd = epoll_create(1234); // size must be > 0 but ignored, lol
    ep->id = inc_id();

    if (ep->fd == -1) {
        lua_errno(L);
    }

    luaL_setmetatable(L, LUA_MT_NET_EPOLL);

    return 1;
}

static int lua_net_epoll_start(lua_State *L) {
    lua_ud_socket *ep = luaL_checkudata(L, 1, LUA_MT_NET_EPOLL);
    int timeout = luaL_optinteger(L, 2, -1); // milliseconds, -1 = inf

    if (!lua_isfunction(L, 3)) lua_fail(L, "arg #3 must be onread", -1);
    if (!lua_isfunction(L, 4)) lua_fail(L, "arg #4 must be onwrite", -1);
    if (!lua_isfunction(L, 5)) lua_fail(L, "arg #5 must be ontimeout", -1);
    if (!lua_isfunction(L, 6)) lua_fail(L, "arg #6 must be onerror", -1);
    if (!lua_isfunction(L, 7)) lua_fail(L, "arg #6 must be onhup", -1);

    int nfds, n, e, fd;
    struct epoll_event events[EPOLL_DEFAULT_QUEUE_SIZE];

    while (ep->fd > -1) {
        nfds = epoll_wait(ep->fd, events, EPOLL_DEFAULT_QUEUE_SIZE, timeout);

        if (nfds == -1) { // epoll_wait call error
            if (errno==EINTR) continue; // interrupted by signal, repeat wait
            else if (errno==EBADF) break; // epoll was closed
            else lua_errno(L); // error
        } else if (nfds == 0) { // timeout (no arguments)
            lua_pushvalue(L, 5);
            lua_epoll_pcall(L, 0);
        } else {
            for (n=0; n<nfds; ++n) {
                e = events[n].events;
                fd = events[n].data.fd;

                if (e & EPOLLERR) { // error (fd, es, en)
                    // syntax error: fd ⇒ nil
                    // epoll error: fd ⇒ number
                    lua_pushvalue(L, 6);
                    lua_pushinteger(L, fd);
                    lua_pushsockerr(L, fd); // adds 2 elements
                    lua_epoll_pcall(L, 3);
                }

                if (e & EPOLLOUT) { // can write (fd)
                    lua_pushvalue(L, 4);
                    lua_pushinteger(L, fd);
                    lua_epoll_pcall(L, 1);
                }

                if (e & EPOLLIN) { // can read (fd)
                    lua_pushvalue(L, 3);
                    lua_pushinteger(L, fd);
                    lua_epoll_pcall(L, 1);
                }

                if (e & EPOLLRDHUP || e & EPOLLHUP) { // conn drop (fd)
                    lua_pushvalue(L, 7);
                    lua_pushinteger(L, fd);
                    lua_epoll_pcall(L, 1);
                }
            }
        }
    }

    lua_pushboolean(L, 1);

    return 1;
}

static int lua_net_epoll_stop(lua_State *L) {
    lua_ud_socket *ep = luaL_checkudata(L, 1, LUA_MT_NET_EPOLL);

    if (ep->fd != -1) {
        close(ep->fd);
        ep->fd = -1;
    }

    return 0;
}

static int lua_net_epoll_watch(lua_State *L) {
    lua_ud_socket *ep = luaL_checkudata(L, 1, LUA_MT_NET_EPOLL);
    int fd = luaL_checkinteger(L, 2);
    int events = luaL_checkinteger(L, 3);

    struct epoll_event e = {0};

    e.events = events;
    e.data.fd = fd;

    int r = epoll_ctl(ep->fd, EPOLL_CTL_ADD, fd, &e);

    if (r == -1) {
        lua_errno(L);
    }

    lua_pushboolean(L, 1);

    return 1;
}

static int lua_net_epoll_unwatch(lua_State *L) {
    lua_ud_socket *ep = luaL_checkudata(L, 1, LUA_MT_NET_EPOLL);
    int fd = luaL_checkinteger(L, 2);

    int r = epoll_ctl(ep->fd, EPOLL_CTL_DEL, fd, NULL);

    if (r == -1) {
        lua_errno(L);
    }

    lua_pushboolean(L, 1);

    return 1;
}
