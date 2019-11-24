local net = require "net"
local etc = require "etc"

local c = class:UnixSocketClient {
    connected = false,
    r_buf = "",
    w_buf = "",
    -- sock
    -- addr
}

function c:block_until_connect()
    while self.connected do
        local r, es, en = self.sock:connect(self.addr)

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
            return nil, "wait_until_connect_to_unix error: " .. es
        end
    end

    return true
end

return c






-- connect to multiple
-- return function(socks, addr_tpl)
--     local amount = #socks
--     local unconnected = amount
--     local connected = {}
--     local addrs = {}

--     for i = 1, amount do
--         connected[i] = false
--         addrs[i] = addr_tpl:gsub("$i", math.tointeger(i))
--     end

--     repeat
--         for i = 1, amount do
--             if not connected[i] then
--                 local r, es, en = socks[i]:connect(addrs[i])

--                 if r then
--                     connected[i] = true
--                     unconnected = unconnected - 1
--                 elseif en == net.e.ENOENT
--                     or en == net.e.EALREADY
--                     or en == net.e.ECONNREFUSED
--                     or en == net.e.EINPROGRESS
--                     or en == net.e.EAGAIN
--                 then
--                     etc.sleep(0.0001)
--                 else
--                     return nil, "wait_until_connect_to_unix_all error: " .. es
--                 end
--             end
--         end
--     until unconnected == 0

--     return true
-- end





