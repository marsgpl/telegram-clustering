local fs = require "fs"
local UnixSocketServer = require "UnixSocketServer"
local UnixSocketClient = require "UnixSocketClient"

local args = thread.args()
local files_found = 0

args.index = math.tointeger(args.index)

local workers = UnixSocketServer:new {
    packet_sep = args.packet_sep,
    path = args.routes.reader_for_workers:gsub("$index", args.index),
}

local reporter = UnixSocketClient:new {
    packet_sep = args.packet_sep,
    path = args.routes.reporter_for_readers,
}

workers:create()
workers:block(true)
workers:listen()
workers:accept(args.threads.workers.amount)
workers:block_clients(true)

reporter:create()
reporter:block(true)
reporter:connect()

assert(fs.traverse(args.src_dir, function(file_name, file_path)
    local file = {
        name = file_name,
        path = file_path,
    }

    if args.read_files then
        file.content = assert(fs.readfile(file.path))
    end

    files_found = files_found + 1

    local worker = workers:round_robin_next_client()

    worker:send({
        file = file,
    })

    if args.files_limit and files_found >= args.files_limit then
        return true -- finish
    end
end))

workers:broadcast({
    result = {
        files_found = files_found,
    }
})

reporter:send({
    result = {
        files_found = files_found,
    }
})
