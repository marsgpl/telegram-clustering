# tgnews - clustering challenge

## builder

    docker build -t tgnews/builder .
    docker run --name tgnewsbuilder --detach --volume /Users/marsgpl/etc/tgnews/:/tgnews/:rw tgnews/builder
    docker exec -it tgnewsbuilder bash
        cd /tgnews
        apt update
        apt install -y build-essential libreadline-dev
        cd src/lua-5.3.5
        make linux
        cd ../tgnews
        make && make install
    docker kill tgnewsbuilder
    docker container rm tgnewsbuilder
    docker image rm tgnews/builder

## etc

[details](https://contest.com/docs/data_clustering)

## release

    submission.zip
        tgnews
        src
        deb-packages.txt
        *
