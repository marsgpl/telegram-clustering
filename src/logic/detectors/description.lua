local meta_tags = require "detectors/meta_tags"

return function(info)
    if info.description then return end

    meta_tags(info)

    info.description = info.meta_tags["og:description"] or ""
end
