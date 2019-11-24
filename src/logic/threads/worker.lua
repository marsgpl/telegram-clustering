local fs = require "fs"
local net = require "net"
local json = require "cjson"
local UnixSocketClient = require "UnixSocketClient"

local args = thread.args()
local task = require(args.task.path)

args.index = math.tointeger(args.index)

local readers = {}

for index = 1, args.threads.readers.amount do
    readers[index] = UnixSocketClient:new {
        packet_sep = args.packet_sep,
        path = args.routes.reader_for_workers:gsub("$index", index),
    }
end

local reporter = UnixSocketClient:new {
    packet_sep = args.packet_sep,
    path = args.routes.reporter_for_workers,
}

for _, reader in ipairs(readers) do
    reader:create()
    reader:block(true)
    reader:connect()
end

reporter:create()
reporter:block(true)
reporter:connect()

local epoll = assert(net.epoll())
local timeout = 10000
local reader_by_fd = {}
local readers_unfinished = #readers

for _, reader in ipairs(readers) do
    local fd = reader.sock:fd()
    reader_by_fd[fd] = reader
    reader:block(false)
    assert(epoll:watch(fd, net.f.EPOLLET | net.f.EPOLLRDHUP | net.f.EPOLLIN))
end

-- TODO: move json decoding in UnixSocketClient
local reader_on_packet = function(packet)
    packet = json.decode(packet)
    if not packet then return end

    if packet.result then
        return true -- communication finished
    elseif packet.file then
        local content = packet.file.content or assert(fs.readfile(packet.file.path))
        local result = task(content)

        result.file_name = packet.file.name

        reporter:send(result)
    end
end

local onhup = function(fd)
    assert(epoll:unwatch(fd))
    reader_by_fd[fd].sock:close()

    readers_unfinished = readers_unfinished - 1

    if readers_unfinished == 0 then
        epoll:stop()
    end
end

local onread = function(fd)
    local disconnected = reader_by_fd[fd]:recv(reader_on_packet)

    if disconnected then
        onhup(fd)
    end
end

local onwrite = function(_)
end

local onerror = function(fd, es, _)
    if fd then
        onhup(fd)
    else
        error(es)
    end
end

local ontimeout = function()
    epoll:stop()
end

assert(epoll:start(timeout, onread, onwrite, ontimeout, onerror, onhup))
