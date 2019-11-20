package.path = "libs/?.lua;libs/?.luac"
package.cpath = "libs/?.so"

local trace = require "trace"

trace(_G)
