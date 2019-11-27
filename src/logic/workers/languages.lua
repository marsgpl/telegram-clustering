local language = require "detectors/language"

local filter_langs = {
    en = true,
    ru = true,
}

return function(file)
    local info = {
        file = file,
    }

    language(info)

    return {
        accept = filter_langs[info.language],
        name = info.file.name,
        language = info.language,
    }
end
