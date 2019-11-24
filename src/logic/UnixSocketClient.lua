local net = require "net"
local etc = require "etc"
local json = require "cjson"
local UnixSocket = require "UnixSocket"

-- TODO: r_buf, w_buf must be as userdata in c to avoid extra copying
local c = class:UnixSocketClient {
    -- path
    -- packet_sep
    connected = false,
    r_buf = "",
    w_buf = "",
}:extends { UnixSocket }

function c:connect()
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
            error(es)
        end
    end
end

function c:send(packet)
    if type(packet) ~= "string" then
        packet = json.encode(packet) .. self.packet_sep
    end

    self.w_buf = self.w_buf .. packet

    self:send_w_buf()
end

-- TODO: make async
function c:send_w_buf()
    local len = assert(self.sock:send(self.w_buf))
    assert(len == #self.w_buf)
    self.w_buf = ""
end

function c:recv(on_packet)
    local disconnected = false
    local sock = self.sock
    local sep = self.packet_sep

    while not disconnected do
        local chunk, es, en = sock:recv()

        if chunk then
            if #chunk == 0 then
                disconnected = "0 bytes rcvd"
            else
                self.r_buf, disconnected = net.splitby(self.r_buf .. chunk, sep, on_packet)
            end
        elseif en == net.e.EAGAIN then
            break -- no data atm
        else -- error on socket
            disconnected = es
        end
    end

    return disconnected
end

return c
