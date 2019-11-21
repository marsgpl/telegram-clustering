# tgnews - clustering challenge

## builder

    docker build -t tgnews/builder .
    docker run --name tgnewsbuilder --detach --volume /Users/marsgpl/projects/telegram/tgnews/:/tgnews/:rw tgnews/builder
    docker exec -it tgnewsbuilder bash
        cd /tgnews
        apt update
        apt install -y build-essential libreadline-dev
        cd src/tgnews
        make lua && make && make libs && make logic
    docker kill tgnewsbuilder
    docker container rm tgnewsbuilder
    docker image rm tgnews/builder

    alias tgnews='cd /tgnews/src/tgnews && make -s logic && cd ../.. && time ./tgnews'
    alias re='cd /tgnews/src/tgnews && make all && cd ../..'

## etc

[details](https://contest.com/docs/data_clustering)

## release

    submission.zip
        src
        logic
        libs
        tgnews
        deb-packages.txt

    do not forget:
        - lua make clean before zipping
        - add -s option to libs/logic/tgnews?
        - uncomment result encoding to json
