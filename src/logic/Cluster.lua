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
            files_limit = 5,
            file = "logic/threads/reader.luac",
            threads = {},
        },
        reporter = {
            file = "logic/threads/reporter.luac",
            threads = {},
        },
        workers = {
            amount = 6, -- as cpus cores
            file = "logic/threads/worker.luac",
            threads = {},
        },
    },
    -- task
    -- src_dir
}

function c:init()
    self:check_task()
    self:check_src_dir()

    self:init_reader()
    self:init_workers()
    self:init_reporter()
end

function c:get_task_path_for_worker(for_require)
    if for_require then
        return "workers/" .. self.task
    else
        return "logic/workers/" .. self.task .. ".luac"
    end
end

function c:get_task_path_for_reporter(for_require)
    if for_require then
        return "reporters/" .. self.task
    else
        return "logic/reporters/" .. self.task .. ".luac"
    end
end

function c:check_task()
    local path = self:get_task_path_for_worker()
    local r, es = fs.stat(path)

    if r then
        path = self:get_task_path_for_reporter()
        r, es = fs.stat(path)
    end

    if not r or r.type ~= "file" then
        error("unknown task: " .. self.task .. "\n"
            .. (es or ("path is directory: " .. path)))
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

    local args = {
        routes = self.routes,
        packet_sep = self.packet_sep,
        src_dir = self.src_dir,
        read_files = reader.read_files,
        files_limit = reader.files_limit,
        workers = {
            amount = self.threads.workers.amount,
        },
    }

    local t, tid = assert(thread.start(reader.file, args))
    reader.threads[tid] = t
end

function c:init_reporter()
    local reporter = self.threads.reporter

    local args = {
        routes = self.routes,
        packet_sep = self.packet_sep,
        task = {
            name = self.task,
            path = self:get_task_path_for_reporter(true),
        },
        workers = {
            amount = self.threads.workers.amount,
        },
    }

    local t, tid = assert(thread.start(reporter.file, args))
    reporter.threads[tid] = t
end

function c:init_workers()
    local workers = self.threads.workers

    local args = {
        routes = self.routes,
        packet_sep = self.packet_sep,
        task = {
            name = self.task,
            path = self:get_task_path_for_worker(true),
        },
        index = 0, -- thread index
    }

    for i = 1, workers.amount do
        args.index = i
        local t, tid = assert(thread.start(workers.file, args))
        workers.threads[tid] = t
    end
end

return c
