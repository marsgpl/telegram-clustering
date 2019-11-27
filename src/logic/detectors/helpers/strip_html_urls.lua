return function(str)
    return (str:gsub("<a[^>]-href=[^>]+>[^<]+</a>", ""))
end
