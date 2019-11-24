local etc = require "etc"
local get_meta_tags = require "detectors/get_meta_tags"

-- local filter_langs = {"ru", "en"}

return function(html)
    local meta_tags = get_meta_tags(html)
    local url = meta_tags["og:url"]
    local domain_zone = etc.parse_url(url or "").zone

io.write("domain_zone: ", trace.str(domain_zone))

    return {
        -- file_name
        accept = false,
        lang = "en",
    }
end

-- if og:url domain zone is equal to lang code - increase prob
-- if og:site_name is in corpus - increase prob
-- if og:title is in corpus - increase prob
-- if og:description is in corpus - increase prob
-- check for h1 to match og:title - or decrease weight of og:title
-- check for first p to match og:description - or decrease weight of og:title
