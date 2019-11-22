# tgnews

Telegram clustering challenge participant

## build

    TGNEWS_ROOT_DIR=/Users/marsgpl/projects/telegram/tgnews/
    docker build -t tgnews/builder .
    docker run --name tgnewsbuilder --detach --volume $TGNEWS_ROOT_DIR:/tgnews/:rw tgnews/builder
    docker exec -it tgnewsbuilder bash
        apt update
        apt install -y build-essential libreadline-dev
        cd /tgnews/src/tgnews
        make all
    docker kill tgnewsbuilder
    docker container rm tgnewsbuilder
    docker image rm tgnews/builder

    alias re='cd /tgnews/src/tgnews && make all && cd ../..'
    alias tgnews='cd /tgnews/src/tgnews && make -s logic && cd ../.. && time ./tgnews'

## etc

[details](https://contest.com/docs/data_clustering)

## release

    make clean
