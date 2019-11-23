local filter_langs = {"ru", "en"}

return function(content)
    return {
        accept = true,
        lang = math.random(1, 2) == 1 and "ru" or "en",
    }
end


    -- if encoding not utf-8 - check only for ascii en
    -- if og:url domain zone is equal to lang code - increase prob
    -- if og:site_name is in corpus - increase prob
    -- if og:title is in corpus - increase prob
    -- if og:description is in corpus - increase prob
    -- check for h1 to match og:title - or decrease weight of og:title
    -- check for first p to match og:description - or decrease weight of og:title




--         local content = cluster:read_file(path)

--         local first_n = content:find("\n", 1, true)
--         local doctype = content:sub(1, first_n - 1)

--         for tag in content:gmatch("<meta[^>]+>") do
--             for key, val in tag:gmatch("([%a]+)=\"([^\"]+)\"") do
--                 val = val or "nil"

--                 if key == "charset" then
--                     stat.meta.charsets[val] = (stat.meta.charsets[val] or 0) + 1
--                 elseif key == "property" then
--                     stat.meta.props[val] = (stat.meta.props[val] or 0) + 1
--                 end
--             end
--         end
