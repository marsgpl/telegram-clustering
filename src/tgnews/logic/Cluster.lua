local thread = require "thread"
local json = require "cjson"
local fs = require "fs"

local c = class:Cluster {
    cores = 8,
    ram = 16, -- gigs
    task_base_path = "logic/tasks",
    task_ext = "luac",
    workers = {},
    worker_params = {
        master = {
            addr = { -- wait for conns
                ip = "127.0.0.1",
                port = 0,
            }
        },
        worker = {
            addr = { -- initiates conn
                ip = "127.0.0.1",
                port = 0,
            }
        }
    }
}

function c:init()
    self:init_router()
    self:init_workers()
end

function c:init_router()
end

function c:init_workers()
    local workers_amount = self.cores
    local worker_file_path = self.task_base_path .. "/" .. self.task .. "." .. self.task_ext

    -- file exists?
    assert(fs.stat(worker_file_path))

    for i = 1, workers_amount do
        local t, tid = assert(thread.start(worker_file_path, self.worker_params))
        self.workers[tid] = t
    end
end

return c
