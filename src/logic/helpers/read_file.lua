return function(path)
    local r, es

    r, es = io.open(path, "r")
    if not r then return r, es end

    local file = r

    r, es = file:read("*a")
    if not r then return r, es end

    local content = r

    file:close()

    return content
end
