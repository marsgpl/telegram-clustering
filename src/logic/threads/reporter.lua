


















-- local net = require "net"
-- local etc = require "etc"
-- local json = require "cjson"
-- local UnixSocketClient = require "UnixSocketClient"

-- local args = thread.args()
-- local reporter_for_workers = assert(net.unix.socket(1))
-- local reporter_for_reader = assert(net.unix.socket(1))
-- local task = require(args.task.path)
-- local epoll = assert(net.epoll())

-- local workers = {}
-- local sock_by_fd = {}
-- local final_report = {}
-- local workers_files_amount = 0
-- local reader_files_amount = 0

-- etc.unlink(args.routes.reporter_for_workers)
-- assert(reporter_for_workers:bind(args.routes.reporter_for_workers))
-- assert(reporter_for_workers:listen())

-- etc.unlink(args.routes.reporter_for_reader)
-- assert(reporter_for_reader:bind(args.routes.reporter_for_reader))
-- assert(reporter_for_reader:listen())

-- for i = 1, args.workers.amount do
--     local sock = assert(reporter_for_workers:accept(1))
--     local fd = sock:fd()

--     local worker = {
--         sock = sock,
--         index = i,
--         rbuf = "",
--     }

--     workers[fd] = worker
--     sock_by_fd[fd] = sock

--     assert(epoll:watch(fd, net.f.EPOLLET | net.f.EPOLLRDHUP | net.f.EPOLLIN))
-- end

-- local reader = assert(reporter_for_reader:accept(1))
-- local reader_rbuf = ""
-- sock_by_fd[reader:fd()] = reader

-- assert(epoll:watch(reader:fd(), net.f.EPOLLET | net.f.EPOLLRDHUP | net.f.EPOLLIN))

-- local timeout = 10000

-- local onhup = function(fd)
--     epoll:unwatch(fd)
--     sock_by_fd[fd]:close()

--     if not workers[fd] then -- reader died
--         epoll:stop()
--     end
-- end

-- local onread = function(fd)
--     local worker = workers[fd]
--     local sock = sock_by_fd[fd]

--     while true do
--         local chunk, es, en = sock:recv()

--         if chunk then
--             if #chunk == 0 then
--                 onhup(fd)
--                 break
--             else
--                 if worker then
--                     worker.rbuf = worker.rbuf .. chunk
--                 else -- reader
--                     reader_rbuf = reader_rbuf .. chunk
--                 end

--             end
--         elseif en == net.e.EWOULDBLOCK then
--             break -- no data atm
--         else -- error on socket
--             onhup(fd)
--             break
--         end
--     end

--     -- while true do
--     --     local chunk = assert(workers[1]:recv())
--     --     local file = { name="noob.html" }
--     --     local result = { lang="en", accept=true }

--     --     workers_files_amount = workers_files_amount + 1

--     --     if result.accept then
--     --         final_report = task(file, result)
--     --     end

--     --     if workers_files_amount >= reader_files_amount then
--     --         break
--     --     end
--     -- end
-- end

-- local onwrite = function(fd)
-- end

-- local onerror = function(fd, es, en)
--     if fd then -- socket error
--         onhup(fd)
--     else -- lua error
--         epoll:stop()
--     end
-- end

-- local ontimeout = function()
--     epoll:stop()
-- end

-- epoll:start(timeout, onread, onwrite, ontimeout, onerror, onhup)

-- print(json.encode(final_report))

-- for i in ipairs(final_report) do
--     local report = final_report[i]
--     print("lang: "..report.lang_code.."    articles: "..#report.articles)
-- end
