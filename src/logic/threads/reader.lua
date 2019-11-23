local fs = require "fs"
local read_file = require "helpers/read_file"

local args = thread.args()
local files = 0

-- TODO: connect to result
-- TODO: connect to all workers

local t_r, t_es = fs.traverse(args.src_dir, function(file_name, file_path)
    local file = {
        name = file_name,
        path = file_path,
    }

    if args.read_files then
        file.content = read_file(file.path)
    end

    files = files + 1

    -- TODO: round robin send to next worker: file

    return files >= 10
end)

-- TODO: send to result: files

if not t_r then
    error("traverse failed: " .. t_es)
end
