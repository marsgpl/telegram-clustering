local net = require "net"
local etc = require "etc"
local json = require "cjson"

local args = thread.args()
local workers = {}
local reporter_for_workers = assert(net.unix.socket())
local reporter_for_reader = assert(net.unix.socket())
local task = require(args.task.path)
local workers_files_amount = 0
local reader_files_amount = 0

etc.unlink(args.routes.reporter_for_workers)
assert(reporter_for_workers:bind(args.routes.reporter_for_workers))
assert(reporter_for_workers:listen())

etc.unlink(args.routes.reporter_for_reader)
assert(reporter_for_reader:bind(args.routes.reporter_for_reader))
assert(reporter_for_reader:listen())

for i = 1, args.workers.amount do
    workers[i] = assert(reporter_for_workers:accept())
end

local reader = assert(reporter_for_reader:accept())

local final_report = {}

while true do
    local chunk = assert(workers[1]:recv())
    local file = { name="noob.html" }
    local result = { lang="en", accept=true }

    workers_files_amount = workers_files_amount + 1

    if result.accept then
        final_report = task(file, result)
    end

    if workers_files_amount >= reader_files_amount then
        break
    end
end

print(json.encode(final_report))

for i in ipairs(final_report) do
    local report = final_report[i]
    print("lang: "..report.lang_code.."    articles: "..#report.articles)
end
