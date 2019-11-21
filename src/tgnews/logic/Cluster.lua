local fs = require "fs"
local json = require "json"

local c = class:Cluster {
    cpus = 8, -- cores
    ram = 16, -- gigs
    base_task_path = "logic/tasks",
}

function c:init()
    local result = self:run_task()
    local output = json.encode(result)
    io.write(output):flush()
trace(result)
end

function c:run_task()
    local task_name = self.task:gsub("[^%a]", ""):lower()

    if #task_name == 0 then
        error("task name must be [a-z]")
    end

    local task_path = self.base_task_path .. "/" .. task_name

    local task_fu = require(task_path)

    return task_fu(self)
end

function c:traverse_src_dir(...)
    return self:traverse_dir(self.src_dir, ...)
end

function c:traverse_dir(dir, on_file, ctx)
    for file_name in fs.readdir(dir) do
        if ctx and ctx.stop then return end

        if not self.ignore_files[file_name] then
            local file_path = dir .. "/" .. file_name

            if fs.isdir(file_path) then
                self:traverse_dir(file_path, on_file, ctx)
            else
                on_file(file_path, file_name, ctx)
            end
        end
    end
end

return c
