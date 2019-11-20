-- nil
-- boolean
-- number
-- string
-- function
-- userdata
-- thread
-- table

-- LUA_TNONE
-- LUA_TNIL
-- LUA_TNUMBER
-- LUA_TBOOLEAN
-- LUA_TSTRING
-- LUA_TTABLE
-- LUA_TFUNCTION
-- LUA_TUSERDATA
-- LUA_TTHREAD
-- LUA_TLIGHTUSERDATA

local trace
local trace_essence

-- trace(...)
trace = setmetatable({}, {
    __call = function(self, ...)
        io.write(self.str(...)):flush()
    end
})

-- local str = trace.str(...)
trace.str = function(...)
    local r = {} -- result
    local n = select("#", ...) -- args num
    local cache = {}

    for i = 1, n do
        local ess = select(i, ...)
        table.insert(r, trace_essence(ess, cache, 0, "ess"))
    end

    return table.concat(r)
end

trace.use_colors = true

trace.tab_width = 4

trace.color = {
    _G = "\27[1;33m", -- bold yellow
    meta = "\27[0;33m", -- yellow
    string = "\27[0;31m", -- red
    number = "\27[0;34m", -- blue
    reset = "\27[0m",
}

-- ess: what to trace
-- cache: remembered tables to avoid infinite trace in same table
-- tabs: indent; how many tabs (spaces x4) add before line
-- mode: type of trace (ess,key,value)
trace_essence = function(ess, cache, tabs, mode)
    local r = setmetatable({}, { __index=table }) -- result
    local t = type(ess)

    if mode ~= "value" then
        r:insert(string.rep(" ", tabs * trace.tab_width))
    end

    if t == "string" then
        if trace.use_colors then
            r:insert(trace.color.string)
        end

        r:insert((string.format("%q", ess):gsub("\n", "10")))

        if trace.use_colors then
            r:insert(trace.color.reset)
        end
    elseif t == "number" then
        if trace.use_colors then
            r:insert(trace.color.number)
        end

        r:insert(tostring(ess))

        if trace.use_colors then
            r:insert(trace.color.reset)
        end
    elseif t == "table" and ess == _G then
        if trace.use_colors then
            r:insert(trace.color._G)
        end

        r:insert("_G: ")
        r:insert(tostring(ess))

        if trace.use_colors then
            r:insert(trace.color.reset)
        end
    else
        r:insert(tostring(ess))
    end

    if t ~= "string" then
        local mt = getmetatable(ess)

        if mt then
            r:insert(" ")

            if trace.use_colors then
                r:insert(trace.color.meta)
            end

            r:insert("[meta")
            r:insert(tostring(mt))
            r:insert("]")

            if trace.use_colors then
                r:insert(trace.color.reset)
            end
        end
    end

    if t == "table" then
        if cache[ess] then
            r:insert(" *")
        elseif mode == "key" then
            r:insert(" { ... }")
        else
            cache[ess] = true

            for k,v in pairs(ess) do
                r:insert("\n")
                r:insert(trace_essence(k, cache, tabs+1, "key"))
                r:insert(" = ")
                r:insert(trace_essence(v, cache, tabs+1, "value"))
            end
        end
    end

    if mode == "ess" then
        r:insert("\n")
    end

    return r:concat()
end

return trace
