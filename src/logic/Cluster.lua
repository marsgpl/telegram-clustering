local fs = require "fs"
local thread = require "thread"

local c = class:Cluster {
    packet_sep = "\1",
    routes = {
        reader = "tgnews-reader.sock",
        reporter_for_workers = "tgnews-reporter-w.sock",
        reporter_for_reader = "tgnews-reporter-r.sock",
    },
    threads = {
        reader = {
            read_files = false,
            file = "logic/threads/reader.luac",
            threads = {},
        },
        reporter = {
            file = "logic/threads/reporter.luac",
            threads = {},
        },
        workers = {
            amount = 8, -- as cpus cores
            file = "logic/threads/worker.luac",
            threads = {},
        },
    },
}

function c:init()
    self:check_task()
    self:check_src_dir()

    self:init_reader()
    self:init_workers()
    self:init_reporter()
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

function c:init_reader()
    local reader = self.threads.reader

    reader.args = {
        routes = self.routes,
        packet_sep = self.packet_sep,
        src_dir = self.src_dir,
        read_files = reader.read_files,
        workers = {
            amount = self.threads.workers.amount,
        },
    }

    local t, tid = assert(thread.start(reader.file, reader.args))
    reader.threads[tid] = t
end

function c:init_reporter()
    local reporter = self.threads.reporter

    reporter.args = {
        routes = self.routes,
        packet_sep = self.packet_sep,
        workers = {
            amount = self.threads.workers.amount,
        },
    }

    local t, tid = assert(thread.start(reporter.file, reporter.args))
    reporter.threads[tid] = t
end

function c:init_workers()
    local workers = self.threads.workers

    workers.args = {
        routes = self.routes,
        packet_sep = self.packet_sep,
        task = {
            name = self.task.name,
            path = self.task.path,
        },
        tidx = 0, -- thread index
    }

    for i = 1, workers.amount do
        workers.args.tidx = i
        local t, tid = assert(thread.start(workers.file, workers.args))
        workers.threads[tid] = t
    end
end

return c
