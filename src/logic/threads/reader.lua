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
        file.content = read_file(file.path)
    end

    files_found = files_found + 1

    workers[next_free_worker]:send(json.encode(file) .. args.packet_sep)
    next_free_worker = next_free_worker + 1
    if next_free_worker > #workers then next_free_worker = 1 end

    return files_found >= 1000
end)

for i = 1, args.workers.amount do
    workers[i]:send(json.encode({
        no_more_files = true,
    }) .. args.packet_sep)
end

wait_until_connect_to_unix(reporter, args.routes.reporter_for_reader)

if not t_r then
    error("traverse failed: " .. t_es)
end
