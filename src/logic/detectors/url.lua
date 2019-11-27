local text = require "text"
local meta_tags = require "detectors/meta_tags"

return function(info)
    if info.url then return end

    meta_tags(info)

    local url = info.meta_tags["og:url"] or ""

    info.url = text.parse_url(url)

    info.url.full = url
end
