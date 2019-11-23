// socket domains:
lua_add_int_const(L, AF_UNIX);
lua_add_int_const(L, AF_LOCAL);
lua_add_int_const(L, AF_INET);
lua_add_int_const(L, AF_INET6);
lua_add_int_const(L, AF_IPX);
lua_add_int_const(L, AF_NETLINK);
lua_add_int_const(L, AF_X25);
lua_add_int_const(L, AF_AX25);
lua_add_int_const(L, AF_ATMPVC);
lua_add_int_const(L, AF_APPLETALK);
lua_add_int_const(L, AF_PACKET);
lua_add_int_const(L, AF_ALG);

// socket types:
lua_add_int_const(L, SOCK_STREAM);
lua_add_int_const(L, SOCK_DGRAM);
lua_add_int_const(L, SOCK_SEQPACKET);
lua_add_int_const(L, SOCK_RAW);
lua_add_int_const(L, SOCK_RDM);
lua_add_int_const(L, SOCK_PACKET);

// socket behavior:
lua_add_int_const(L, SOCK_NONBLOCK);
lua_add_int_const(L, SOCK_CLOEXEC);

// shutdown options:
lua_add_int_const(L, SHUT_RD);
lua_add_int_const(L, SHUT_WR);
lua_add_int_const(L, SHUT_RDWR);

// socket options:
lua_add_int_const(L, SO_ACCEPTCONN);
lua_add_int_const(L, SO_BINDTODEVICE);
lua_add_int_const(L, SO_BROADCAST);
lua_add_int_const(L, SO_BSDCOMPAT);
lua_add_int_const(L, SO_DEBUG);
lua_add_int_const(L, SO_DOMAIN);
lua_add_int_const(L, SO_ERROR);
lua_add_int_const(L, SO_DONTROUTE);
lua_add_int_const(L, SO_KEEPALIVE);
lua_add_int_const(L, SO_LINGER);
lua_add_int_const(L, SO_MARK);
lua_add_int_const(L, SO_OOBINLINE);
lua_add_int_const(L, SO_PASSCRED);
lua_add_int_const(L, SO_PEEK_OFF);
lua_add_int_const(L, SO_PEERCRED);
lua_add_int_const(L, SO_PRIORITY);
lua_add_int_const(L, SO_PROTOCOL);
lua_add_int_const(L, SO_RCVBUF);
lua_add_int_const(L, SO_RCVBUFFORCE);
lua_add_int_const(L, SO_RCVLOWAT);
lua_add_int_const(L, SO_SNDLOWAT);
lua_add_int_const(L, SO_RCVTIMEO);
lua_add_int_const(L, SO_SNDTIMEO);
lua_add_int_const(L, SO_REUSEADDR);
lua_add_int_const(L, SO_REUSEPORT);
lua_add_int_const(L, SO_RXQ_OVFL);
lua_add_int_const(L, SO_SNDBUF);
lua_add_int_const(L, SO_SNDBUFFORCE);
lua_add_int_const(L, SO_TIMESTAMP);
lua_add_int_const(L, SO_TYPE);
lua_add_int_const(L, SO_BUSY_POLL);
lua_add_int_const(L, O_NONBLOCK);

// epoll stuff
lua_add_int_const(L, EPOLLIN);
lua_add_int_const(L, EPOLLOUT);
lua_add_int_const(L, EPOLLRDHUP);
lua_add_int_const(L, EPOLLPRI);
lua_add_int_const(L, EPOLLERR);
lua_add_int_const(L, EPOLLHUP);
lua_add_int_const(L, EPOLLET);
lua_add_int_const(L, EPOLLONESHOT);
lua_add_int_const(L, EPOLLWAKEUP);
