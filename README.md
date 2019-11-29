# tgnews

Telegram clustering challenge participant

## debug build

    TGNEWS_ROOT_DIR=/Users/marsgpl/projects/telegram/tgnews
    TGNEWS_INPUT_DIR=/Users/marsgpl/projects/telegram/tgnews-input
    docker build -t tgnews/builder .
    docker run --name tgnewsbuilder --detach --volume $TGNEWS_ROOT_DIR/:/tgnews/:rw --volume $TGNEWS_INPUT_DIR/:/tgnews/input/:ro tgnews/builder
    docker exec -it tgnewsbuilder bash
        apt update
        apt install -y build-essential libreadline-dev cmake
        alias ll='ls -Albh --color=auto --group-directories-first'
        alias re='cd /tgnews/src && make && cd /tgnews'
        alias rere='cd /tgnews/src && make clean all && cd /tgnews'
        alias tgnews='cd /tgnews/src && make -s logic && cd /tgnews && time ./tgnews'
        alias renorm='cd /tgnews/src && make -s logic && cd /tgnews && time ./tgnews normalize'
        alias norm='cd /tgnews && ./tgnews normalize'
        re
        tgnews languages input
        norm
    docker kill tgnewsbuilder
    docker container rm tgnewsbuilder
    docker image rm tgnews/builder

## etc

[details](https://contest.com/docs/data_clustering)

    find . -name .DS_Store -type f -delete
    find . -name \*.luac -type f -delete
    find . -name \*.o -type f -delete
    find . -name \*.so -type f -delete
    find . -name tgnews-\*.sock -type f -delete

    ~/projects/telegram/tgnews-raw/grams $ node ru2.js && node en2.js && ll ../../tgnews/grams
