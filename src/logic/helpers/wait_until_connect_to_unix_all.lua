local net = require "net"
local etc = require "etc"

return function(socks, addr_tpl)
    local amount = #socks
    local unconnected = amount
    local connected = {}
    local addrs = {}

    for i = 1, amount do
        connected[i] = false
        addrs[i] = addr_tpl:gsub("$i", math.tointeger(i))
    end

    repeat
        for i = 1, amount do
            if not connected[i] then
                local r, es, en = socks[i]:connect(addrs[i])

                if r then
                    connected[i] = true
                    unconnected = unconnected - 1
                elseif en == net.e.ENOENT
                    or en == net.e.EINPROGRESS
                    or en == net.e.EALREADY
                    or en == net.e.ECONNREFUSED
                then
                    etc.sleep(0.0001)
                else
                    error("wait_until_connect_to error: " .. es)
                end
            end
        end
    until unconnected == 0
end
