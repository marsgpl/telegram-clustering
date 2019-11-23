local net = require "net"
local etc = require "etc"
local json = require "cjson"
local wait_until_connect_to_unix = require "helpers/wait_until_connect_to_unix"

local args = thread.args()
-- local tidx = math.tointeger(args.tidx)
local reader = assert(net.unix.socket())
local reporter = assert(net.unix.socket())

wait_until_connect_to_unix(reader, args.routes.reader)

wait_until_connect_to_unix(reporter, args.routes.reporter_for_workers)

local buf = ""
local all_files_rcvd = false

while not all_files_rcvd do
    local chunk = assert(reader:recv())

    if #chunk == 0 then
        break -- socket closed
    end

    buf = buf .. chunk

    while not all_files_rcvd do
        chunk, buf = etc.splitby(buf, args.packet_sep)

        if not chunk then
            break
        end

        if #chunk > 0 then
            local file = json.decode(chunk)

            if file.no_more_files then
                all_files_rcvd = true
            else
                -- trace(file)
            end
        end

        if #buf == 0 then
            break
        end
    end
end
