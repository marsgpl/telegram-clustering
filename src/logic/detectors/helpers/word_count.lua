local text = require "text"
local space = require "data/space"

return function(str)
    if #str == 0 then
        return 0
    end

    return #text.strip_chars(str, space, true) + 1
end
