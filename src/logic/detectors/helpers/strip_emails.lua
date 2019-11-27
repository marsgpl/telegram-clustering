return function(str)
    return (str:gsub("[%w-_.]+@[%w-_]+%.[%w-_]+", ""))
end
