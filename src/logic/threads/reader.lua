local fs = require "fs"
local net = require "net"
local etc = require "etc"
local json = require "cjson"
local read_file = require "helpers/read_file"
local wait_until_connect_to_unix = require "helpers/wait_until_connect_to_unix"

local args = thread.args()
local files_found = 0
local workers = {}
local reporter = assert(net.unix.socket())
local reader = assert(net.unix.socket())
local next_free_worker = 1

etc.unlink(args.routes.reader)
assert(reader:bind(args.routes.reader))
assert(reader:listen())

for i = 1, args.workers.amount do
    workers[i] = assert(reader:accept())
end

local t_r, t_es = fs.traverse(args.src_dir, function(file_name, file_path)
    local file = {
        name = file_name,
        path = file_path,
    }

    if args.read_files then
        file.content = assert(read_file(file.path))
    end

    files_found = files_found + 1

    assert(workers[next_free_worker]:send(json.encode({
        file = file,
    }) .. args.packet_sep))

    next_free_worker = next_free_worker + 1

    if next_free_worker > #workers then
        next_free_worker = 1
    end

    if args.files_limit and files_found >= args.files_limit then
        return true -- finish
    end
end)

for i = 1, args.workers.amount do
    assert(workers[i]:send(json.encode({
        result = {
            files_found = files_found,
        }
    }) .. args.packet_sep))
end

assert(wait_until_connect_to_unix(reporter, args.routes.reporter_for_reader))

assert(reporter:send(json.encode({
    result = {
        files_found = files_found,
    }
}) .. args.packet_sep))

if not t_r then
    error("traverse failed: " .. t_es)
end
