return function(str)
    return (str
        :gsub("https?://[^ ]+", "")
        :gsub("[^ ]-www%.[^ ]+", "")
        :gsub("[^ ]-[%w-_]+%.[%w-_]+/[^ ]-", "")
    )
end
