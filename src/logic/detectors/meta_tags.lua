return function(info)
    if info.meta_tags then return end

    local tags = {}

    for tag in info.file.content:gmatch("<[Mm][Ee][Tt][Aa].->") do
        local property, content

        for key, val in tag:gmatch("([%a]+)=\"([^\"]+)\"") do
            if key == "property" then
                property = val
            elseif key == "content" then
                content = val
            else
                tags[key] = val
            end
        end

        if property and content then
            tags[property] = content
        end
    end

    info.meta_tags = tags
end
