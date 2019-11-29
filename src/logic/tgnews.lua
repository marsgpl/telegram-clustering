local Cluster = require "Cluster"
local normalizer = require "workers/normalizer"

if args.task == "normalize" then
    normalizer(args.src_dir)
else
    Cluster:new {
        task = args.task,
        src_dir = args.src_dir,
    }
end
