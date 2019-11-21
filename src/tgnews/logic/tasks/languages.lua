local fs = require "fs"
local Document = require "logic/Document"

local filter_langs = {"ru", "en"}

return function(cluster)
    local result = {}
    local analyzed_docs = 0
    local article_bucket_by_lang = {}

    for _, lang in ipairs(filter_langs) do
        article_bucket_by_lang[lang] = {}

        table.insert(result, {
            lang_code = lang,
            articles = article_bucket_by_lang[lang],
        })
    end

    fs.traverse(cluster.src_dir, function(subpath, path)
        print("subpath", subpath)
        print("path", path)
        os.exit()
    end)

    cluster:traverse_src_dir(function(file_path, file_name, ctx)
        local doc = Document:new {
            file_name = file_name,
            file_path = file_path,
        }

        local lang = doc:detect_language(filter_langs)

        analyzed_docs = analyzed_docs + 1

        if article_bucket_by_lang[lang] then
            table.insert(article_bucket_by_lang[lang], file_name)
        end

        if analyzed_docs == 10000 then
            ctx.stop = true
        end

        print("analyzed_docs", analyzed_docs)
    end, {})

    return result
end
