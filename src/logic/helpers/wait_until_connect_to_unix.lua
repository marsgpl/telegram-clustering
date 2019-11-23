local net = require "net"
local etc = require "etc"

return function(sock, addr)
    local connected = false

    repeat
        local r, es, en = sock:connect(addr)

        if r then
            connected = true
        elseif en == net.e.ENOENT
            or en == net.e.EINPROGRESS
            or en == net.e.EALREADY
            or en == net.e.ECONNREFUSED
        then
            etc.sleep(0.0001)
        else
            error("wait_until_connect_to error: " .. es)
        end
    until connected
end
