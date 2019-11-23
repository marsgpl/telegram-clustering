local etc = require "etc"

local mute = false

local to_fixed_len = function(number, len, symbol)
    local str = tostring(number)
    local add_symbols = len - #str

    if add_symbols > 0 then
        str = str .. symbol:rep(add_symbols)
    end

    return str
end

return function(...)
    if mute then return end

    local msg = setmetatable({}, { __index = table })

    msg:insert(to_fixed_len(etc.microtime(), 15, "0"))

    local n = select("#", ...)

    for i = 1, n do
        local var = select(i, ...)

        if i == 1 then
            var = trace.color.number .. tostring(var) .. trace.color.reset
        end

        msg:insert(tostring(var))
    end

    io.write(msg:concat(" ") .. "\n"):flush()
end
