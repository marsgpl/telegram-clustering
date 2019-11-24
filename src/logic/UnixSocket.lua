local net = require "net"

local c = class:UnixSocket {
    -- sock
}

function c:create()
    self.sock = assert(net.unix.socket(1)) -- 1 = non-blocking
end

function c:block(state)
    assert(self.sock:set(net.f.O_NONBLOCK, state and 0 or 1))
end

return c
