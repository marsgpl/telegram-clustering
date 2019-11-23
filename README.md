# tgnews

Telegram clustering challenge participant

## debug build

    TGNEWS_ROOT_DIR=/Users/marsgpl/projects/telegram/tgnews
    docker build -t tgnews/builder .
    docker run --name tgnewsbuilder --detach --volume $TGNEWS_ROOT_DIR/:/tgnews/:rw tgnews/builder
    docker exec -it tgnewsbuilder bash
        apt update
        apt install -y build-essential libreadline-dev
        alias ll='ls -Albh --color=auto --group-directories-first'
        alias re='cd /tgnews/src && make && cd /tgnews'
        alias rere='cd /tgnews/src && make clean all && cd /tgnews'
        alias tgnews='cd /tgnews/src && make -s logic && cd /tgnews && time ./tgnews'
        re
        tgnews languages input
    docker kill tgnewsbuilder
    docker container rm tgnewsbuilder
    docker image rm tgnews/builder

## etc

[details](https://contest.com/docs/data_clustering)

## clean

    find . -name .DS_Store -type f -delete
    find . -name \*.luac -type f -delete
    find . -name \*.o -type f -delete
    find . -name \*.so -type f -delete
    find . -name tgnews-\*.sock -type f -delete

## todo

    if function returns nil, es, en it must not fall in error (fs.traverse arg#1, thread.create arg#1)
