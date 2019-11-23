local fs = require "fs"
local thread = require "thread"

local c = class:Cluster {
    threads = {
        readers = {
            amount = 1,
            file = "logic/threads/reader.luac",
            args = {
                read_files = false,
            },
            threads = {},
        },
        workers = {
            amount = 8, -- as cpus cores
            file = "logic/threads/worker.luac",
            args = {},
            threads = {},
        },
        results = {
            amount = 1,
            file = "logic/threads/result.luac",
            args = {},
            threads = {},
        }
    },
}

function c:init()
    self:check_task()
    self:check_src_dir()

    self:init_readers()
    self:init_workers()
    self:init_results()
end

function c:check_task()
    self.task = {
        base_path = "logic/tasks",
        name = self.task,
        ext = "luac",
    }

    self.task.path = self.task.base_path .. "/" .. self.task.name .. "." .. self.task.ext

    local r, es = fs.stat(self.task.path)

    if not r or r.type ~= "file" then
        error("unknown task: " .. self.task .. "\n"
            .. (es or ("path is directory: " .. self.task.path)))
    end
end

function c:check_src_dir()
    local r, es = fs.stat(self.src_dir)

    if not r or r.type ~= "dir" then
        error("bad source_dir: " .. self.src_dir .. "\n"
            .. (es or ("path is not directory: " .. self.src_dir)))
    end
end

function c:init_readers()
    local readers = self.threads.readers

    readers.args.src_dir = self.src_dir

    for _ = 1, readers.amount do
        local t, tid = assert(thread.start(readers.file, readers.args))
        readers.threads[tid] = t
    end
end

function c:init_workers()
    local workers = self.threads.workers

    workers.args.task = {
        name = self.task.name,
        path = self.task.path,
    }

    for _ = 1, workers.amount do
        local t, tid = assert(thread.start(workers.file, workers.args))
        workers.threads[tid] = t
    end
end

function c:init_results()
    local results = self.threads.results

    results.args.readers = {
        amount = self.threads.readers.amount,
    }

    for _ = 1, results.amount do
        local t, tid = assert(thread.start(results.file, results.args))
        results.threads[tid] = t
    end
end

return c
