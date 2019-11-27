return function(str)
    return (str
        :gsub("https?://[^ ]+", "")
        :gsub("www%.[^ ]+", "")
        :gsub("[a-zA-Z0-9_-]+%.[a-zA-Z0-9_-]+/", "")
    )
end
