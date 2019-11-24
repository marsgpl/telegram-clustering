local net = require "net"
local etc = require "etc"
local json = require "cjson"

local c = class:UnixSocketClient {
    connected = false,
    r_buf = "",
    w_buf = "",
    -- sock
    -- path
    -- packet_sep
}

function c:block_until_send(packet)
    if type(packet) ~= "string" then
        packet = json.encode(packet) .. self.packet_sep
    end

    self.w_buf = self.w_buf .. packet

    --
end

function c:block_until_connect()
    while not self.connected do
        local r, es, en = self.sock:connect(self.path)

        if r or en == net.e.EISCONN then
            self.connected = true
        elseif en == net.e.ENOENT -- .sock file is not created yet
            or en == net.e.EALREADY -- prev conn attempt in progress (why?)
            or en == net.e.ECONNREFUSED -- .sock file is created but not listening yet
            or en == net.e.EINPROGRESS -- can't connect immediately
            or en == net.e.EAGAIN -- can't connect immediately
        then
            etc.sleep(0.0001)
        else
            error("wait_until_connect_to_unix error: " .. es)
        end
    end
end

return c
