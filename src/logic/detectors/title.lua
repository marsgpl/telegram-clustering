local meta_tags = require "detectors/meta_tags"

return function(info)
    if info.title then return end

    meta_tags(info)

    info.title = info.meta_tags["og:title"] or ""
end
