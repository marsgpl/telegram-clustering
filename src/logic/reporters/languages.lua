local report = {}
local lang_index = {}

return function(file, result)
    local lang = result.lang

    if not lang then
        return report
    end

    if not lang_index[lang] then
        table.insert(report, {
            lang_code = lang,
            articles = {},
        })

        lang_index[lang] = #report
    end

    table.insert(report[lang_index[lang]].articles, file.name)

    return report
end
