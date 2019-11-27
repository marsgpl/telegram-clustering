local url = require "detectors/url"

return function(info)
    if info.domain_zone then return end

    url(info)

    info.domain_zone = info.url.zone or ""
end
