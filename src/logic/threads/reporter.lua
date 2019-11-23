local net = require "net"
local etc = require "etc"
local json = require "cjson"

local args = thread.args()
local workers = {}
local reporter_for_workers = assert(net.unix.socket())
local reporter_for_reader = assert(net.unix.socket())

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
