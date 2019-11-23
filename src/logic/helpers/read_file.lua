return function(path)
    local f = io.open(path, "r")
    local content = f:read("*a")
    f:close()
    return content
end
