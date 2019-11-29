local text = require "text"
local fs = require "fs"

return function(src)
    local dst = src-- .. ".nml"
    local content = assert(fs.readfile(src))
    local len_before = #content
    content = text.normalize(content)
    local len_after = #content

    io.write("bfore: ", trace.str(len_before))
    io.write("after: ", trace.str(len_after))
    io.write("delta: ", trace.str(len_before - len_after))

    assert(fs.writefile(dst, content))
end
