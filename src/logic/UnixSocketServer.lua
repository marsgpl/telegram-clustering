local net = require "net"
local etc = require "etc"
local json = require "cjson"
local UnixSocketClient = require "UnixSocketClient"

local c = class:UnixSocketServer {
    listening = false,
    clients = {},
    next_round_robin_client_index = 1,
    -- sock
    -- path
    -- packet_sep
}

function c:listen()
    if self.listening then
        return
    end

    etc.unlink(self.path)

    self.sock = self.sock or assert(net.unix.socket(1))

    assert(self.sock:bind(self.path))
    assert(self.sock:listen())

    self.listening = true
end

function c:block_until_accept(amount)
    if not self.listening then
        self:listen()
    end

    assert(self.sock:set(net.f.O_NONBLOCK, 0))

    for index = 1, amount do
        local sock = assert(self.sock:accept(1))

        self.clients[index] = UnixSocketClient:new {
            connected = true,
            sock = sock,
            path = self.path,
            packet_sep = self.packet_sep,
        }
    end

    assert(self.sock:set(net.f.O_NONBLOCK, 1))
end

function c:get_next_round_robin_client()
    local client = self.clients[self.next_round_robin_client_index]

    self.next_round_robin_client_index = self.next_round_robin_client_index + 1

    if self.next_round_robin_client_index > #self.clients then
        self.next_round_robin_client_index = 1
    end

    return client
end

function c:block_until_send_to_all(packet)
    if type(packet) ~= "string" then
        packet = json.encode(packet) .. self.packet_sep
    end

    for index = 1, #self.clients do
        self.clients[index]:block_until_send(packet)
    end
end

return c
