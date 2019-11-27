local net = require "net"
local json = require "cjson"
local UnixSocketServer = require "UnixSocketServer"

local args = thread.args()
local task = require(args.task.path)
local final_report = {}
local readers_files = 0
local workers_files = 0

local readers = UnixSocketServer:new {
    packet_sep = args.packet_sep,
    path = args.routes.reporter_for_readers,
}

local workers = UnixSocketServer:new {
    packet_sep = args.packet_sep,
    path = args.routes.reporter_for_workers,
}

readers:create()
readers:block(true)
readers:listen()
readers:accept(args.threads.readers.amount)

workers:create()
workers:block(true)
workers:listen()
workers:accept(args.threads.workers.amount)

local epoll = assert(net.epoll())
local timeout = args.timeout or 10000
local reader_by_fd = {}
local worker_by_fd = {}
local readers_unfinished = args.threads.readers.amount

for _, reader in ipairs(readers.clients) do
    local fd = reader.sock:fd()
    reader_by_fd[fd] = reader
    assert(epoll:watch(fd, net.f.EPOLLET | net.f.EPOLLRDHUP | net.f.EPOLLIN))
end

for _, worker in ipairs(workers.clients) do
    local fd = worker.sock:fd()
    worker_by_fd[fd] = worker
    assert(epoll:watch(fd, net.f.EPOLLET | net.f.EPOLLRDHUP | net.f.EPOLLIN))
end

local reader_on_packet = function(packet)
    packet = json.decode(packet)
    if not packet then return end

    if packet.result then
        readers_files = readers_files + packet.result.files_found
        return true
    end
end

local worker_on_packet = function(packet)
    packet = json.decode(packet)
    if not packet then return end

    workers_files = workers_files + 1

    if packet.accept then
        final_report = task(packet)
    end

    if readers_files == workers_files then
        return true
    end
end

local onhup = function(fd)
    assert(epoll:unwatch(fd))

    local reader = reader_by_fd[fd]
    local worker = worker_by_fd[fd]

    if reader then
        reader.sock:close()
        readers_unfinished = readers_unfinished - 1
    elseif worker then
        worker.sock:close()
    end

    if readers_unfinished == 0 and readers_files == workers_files then
        epoll:stop()
    end
end

local onread = function(fd)
    local disconnected

    local reader = reader_by_fd[fd]
    local worker = worker_by_fd[fd]

    if reader then
        disconnected = reader:recv(reader_on_packet)
    elseif worker then
        disconnected = worker:recv(worker_on_packet)
    end

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

print(json.encode(final_report))
