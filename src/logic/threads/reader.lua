local fs = require "fs"
local UnixSocketServer = require "UnixSocketServer"
local UnixSocketClient = require "UnixSocketClient"
local read_file = require "helpers/read_file"

local args = thread.args()
local files_found = 0

local workers = UnixSocketServer:new {
    path = args.routes.reader,
    packet_sep = args.packet_sep,
}

local reporter = UnixSocketClient:new {
    path = args.routes.reporter_for_reader,
    packet_sep = args.packet_sep,
}

workers:block_until_accept(args.workers.amount)

-- TODO: split dirs to multi readers workers, so we can iterate parallel for huge file counts
local traverse_ok, traverse_error = fs.traverse(args.src_dir, function(file_name, file_path)
    local file = {
        name = file_name,
        path = file_path,
    }

    if args.read_files then
        file.content = assert(read_file(file.path))
    end

    files_found = files_found + 1

    local worker = workers:get_next_round_robin_client()

    -- TODO: async send while iterating through dir
    worker:block_until_send({
        file = file,
    })

    if args.files_limit and files_found >= args.files_limit then
        return true -- finish
    end
end)

reporter:block_until_connect()

-- TODO: next 2 calls can be async

workers:block_until_send_to_all({
    result = {
        files_found = files_found,
    }
})

reporter:block_until_send({
    result = {
        files_found = files_found,
    }
})

if not traverse_ok then
    error("traverse directory error: " .. traverse_error)
end

trace "reader READY"
