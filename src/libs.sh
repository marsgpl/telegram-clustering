#!/bin/bash

. shared/const.sh
. shared/helpers.sh

task=$1

mkdir -p $LIBS_DST_DIR || fail

cd $LIBS_SRC_DIR || fail

for dir in $(find . -mindepth 1 -maxdepth 1 -type d); do
    echo -e "\e[0;35mlib:\e[0m $(basename $dir)"

    cd $dir || fail
    make $task || fail
    cd .. || fail
done
