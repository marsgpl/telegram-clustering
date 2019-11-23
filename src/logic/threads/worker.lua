local net = require "net"
local etc = require "etc"
local json = require "cjson"
local read_file = require "helpers/read_file"
local wait_until_connect_to_unix = require "helpers/wait_until_connect_to_unix"

local args = thread.args()
local reader = assert(net.unix.socket())
local reporter = assert(net.unix.socket())
local task = require(args.task.path)

args.index = math.tointeger(args.index) -- thread index

assert(wait_until_connect_to_unix(reader, args.routes.reader))
assert(wait_until_connect_to_unix(reporter, args.routes.reporter_for_workers))

local buf = ""
local all_files_found = false

while not all_files_found do
    local chunk = assert(reader:recv())

    if #chunk == 0 then
        break -- socket closed
    end

    buf = buf .. chunk

    while not all_files_found do
        chunk, buf = etc.splitby(buf, args.packet_sep)

        if not chunk then
            break -- sep not found
        end

        if #chunk > 0 then
            local data = json.decode(chunk)

            if data.result then
                all_files_found = true
            elseif data.file then
                local content = data.file.content or assert(read_file(data.file.path))
                local result = task(content)

                assert(reporter:send(json.encode({
                    file = {
                        name = data.file.name,
                    },
                    result = result,
                }) .. args.packet_sep))
            end
        end

        if #buf == 0 then
            break -- no more bytes after sep
        end
    end
end
