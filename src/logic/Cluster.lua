local fs = require "fs"
local thread = require "thread"

local c = class:Cluster {
    packet_sep = "\n",
    routes = {
        -- reader listens for workers
        reader_for_workers = "tgnews-rea-wor-$index.sock",
        -- reporter listens for readers
        reporter_for_readers = "tgnews-rep-rea.sock",
        -- reporter listens for workers
        reporter_for_workers = "tgnews-rep-wor.sock",
    },
    threads = {
        readers = {
            amount = 1, -- as many as input dirs amount (source_dir)
            file = "logic/threads/reader.luac",
            read_files = false, -- true if workers are on different machine
            -- files_limit = 1, -- stop after reading N files
        },
        workers = {
            amount = 1, -- as many as cpus cores (or less)
            file = "logic/threads/worker.luac",
            timeout = 999999999,
        },
        reporter = {
            file = "logic/threads/reporter.luac",
            timeout = 999999999,
        },
    },
    resources = {
        threads = {},
    },
    -- task
    -- src_dir
}

function c:init()
    self:check_task()
    self:check_src_dir()

    self:init_readers()
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

function c:init_readers()
    local conf = self.threads.readers

    local args = {
        index = 0, -- thread index, changed while iter
        routes = self.routes,
        threads = self.threads,
        packet_sep = self.packet_sep,
        read_files = conf.read_files,
        files_limit = conf.files_limit,
        src_dir = self.src_dir,
    }

    for index = 1, conf.amount do
        args.index = index
        local t, tid = assert(thread.start(conf.file, args))
        self.resources.threads[tid] = t
    end
end

function c:init_workers()
    local conf = self.threads.workers

    local args = {
        index = 0, -- thread index, changed while iter
        routes = self.routes,
        threads = self.threads,
        packet_sep = self.packet_sep,
        task = {
            name = self.task,
            path = self:get_task_path_for_worker(true),
        },
        timeout = conf.timeout,
    }

    for index = 1, conf.amount do
        args.index = index
        local t, tid = assert(thread.start(conf.file, args))
        self.resources.threads[tid] = t
    end
end

function c:init_reporter()
    local conf = self.threads.reporter

    local args = {
        routes = self.routes,
        threads = self.threads,
        packet_sep = self.packet_sep,
        task = {
            name = self.task,
            path = self:get_task_path_for_reporter(true),
        },
        timeout = conf.timeout,
    }

    local t, tid = assert(thread.start(conf.file, args))
    self.resources.threads[tid] = t
end

return c
