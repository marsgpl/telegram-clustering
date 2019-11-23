function isset {
    if [ -n "$1" ]; then
        return 0 # set
    else
        return 1 # not set
    fi
}

function fail {
    isset "$1" && sep=":" || sep=""

    echo -e "bash: \e[0;31mError$sep\e[0m $1" 1>&2
    echo -e "stack traceback:" 1>&2

    local i=1
    while [ -n "${FUNCNAME[$i]}" ]; do
        local src=${BASH_SOURCE[$i]}
        local line=${BASH_LINENO[$i-1]}
        local chunk=${FUNCNAME[$i-1]}
        echo -e "\t$src:$line: $chunk" 1>&2
        i=$[$i+1]
    done

    exit 1
}
