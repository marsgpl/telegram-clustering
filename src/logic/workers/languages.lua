local etc = require "etc"
local get_meta_tags = require "detectors/get_meta_tags"

-- local filter_langs = {"ru", "en"}

return function(html, path)
    local meta_tags = get_meta_tags(html)
    local url = meta_tags["og:url"]
    local domain_zone = etc.parse_url(url or "").zone

    if not meta_tags["og:description"] then
        io.write("html: ", trace.str(html))
        print(path)
    end

    return {
        -- file_name
        accept = false,
        lang = "en",
    }
end
