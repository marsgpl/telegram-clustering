local Cluster = require "logic/Cluster"

Cluster:new {
    task = args.task,
    src_dir = args.src_dir,
}
