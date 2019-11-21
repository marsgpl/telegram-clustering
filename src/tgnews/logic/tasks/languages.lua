local thread = require "thread"
local trace = require "trace"

-- trace(thread.id())
-- trace(thread.args())


--     local result = self:run_task()
--     local output = json.encode(result)
--     io.write(output):flush()



-- function c:run_task()
--     local task_name = self.task:gsub("[^%a]", ""):lower()

--     if #task_name == 0 then
--         error("task name must be [a-z]")
--     end

--     local task_path = self.base_task_path .. "/" .. task_name

--     local task_fu = require(task_path)

--     return task_fu(self)
-- end




-- local fs = require "fs"

-- local filter_langs = {"ru", "en"}

-- return function(cluster)
--     local result = {}
--     local article_bucket_by_lang = {}
--     local analyzed_docs = 0

--     for _, lang in ipairs(filter_langs) do
--         article_bucket_by_lang[lang] = {}

--         table.insert(result, {
--             lang_code = lang,
--             articles = article_bucket_by_lang[lang],
--         })
--     end

--     fs.traversedir(cluster.src_dir, function(file_name, file_path)
--         analyzed_docs = analyzed_docs + 1

--         local lang = "be"

--         if article_bucket_by_lang[lang] then
--             table.insert(article_bucket_by_lang[lang], file_name)
--         end

--         if analyzed_docs == 1000 then
--             return true
--         end
--     end)

--     return result
-- end


    -- if encoding not utf-8 - check only for ascii en
    -- if og:url domain zone is equal to lang code - increase prob
    -- if og:site_name is in corpus - increase prob
    -- if og:title is in corpus - increase prob
    -- if og:description is in corpus - increase prob
    -- check for h1 to match og:title - or decrease weight of og:title
    -- check for first p to match og:description - or decrease weight of og:title
-- function c:read_file(path)
--     local f = io.open(path, "r")
--     local content = f:read("*a")
--     f:close()
--     return content
-- end
-- stat = {
--         files = 0,
--         size = 0,
--         doctypes = {},
--         meta = {
--             charsets = {},
--             props = {},
--             prop_content = {},
--         },
--     }


--         local content = cluster:read_file(path)

--         local first_n = content:find("\n", 1, true)
--         local doctype = content:sub(1, first_n - 1)

--         for tag in content:gmatch("<meta[^>]+>") do
--             for key, val in tag:gmatch("([%a]+)=\"([^\"]+)\"") do
--                 val = val or "nil"

--                 if key == "charset" then
--                     stat.meta.charsets[val] = (stat.meta.charsets[val] or 0) + 1
--                 elseif key == "property" then
--                     stat.meta.props[val] = (stat.meta.props[val] or 0) + 1
--                 end
--             end
--         end
