local sys = require "sys"
local json = require "cjson"
local UnixSocket = require "UnixSocket"
local UnixSocketClient = require "UnixSocketClient"

local c = class:UnixSocketServer {
    -- path
    -- packet_sep
    clients = {},
    round_robin_next_client_index = 1,
}:extends { UnixSocket }

function c:listen()
    sys.unlink(self.path) -- if path does not exist - silently ignore

    assert(self.sock:bind(self.path))
    assert(self.sock:listen())
end

function c:accept(amount)
    for index = 1, amount do
        local sock = assert(self.sock:accept(1)) -- 1 = non-blocking

        self.clients[index] = UnixSocketClient:new {
            sock = sock,
            path = self.path,
            packet_sep = self.packet_sep,
            connected = true,
        }
    end
end

function c:round_robin_next_client()
    local client = self.clients[self.round_robin_next_client_index]

    self.round_robin_next_client_index = self.round_robin_next_client_index + 1

    if self.round_robin_next_client_index > #self.clients then
        self.round_robin_next_client_index = 1
    end

    return client
end

function c:broadcast(packet)
    if type(packet) ~= "string" then
        packet = json.encode(packet) .. self.packet_sep
    end

    for _, client in ipairs(self.clients) do
        client:send(packet)
    end
end

function c:block_clients(state)
    for _, client in ipairs(self.clients) do
        client:block(state)
    end
end

return c
