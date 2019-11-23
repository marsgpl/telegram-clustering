#!/bin/bash

. shared/const.sh
. shared/helpers.sh

mkdir -p $LOGIC_DST_DIR || fail

cd $LOGIC_SRC_DIR || fail

for file in $(find . -maxdepth 99 -type f); do
    echo -e "logic: \e[0;36m$file\e[0m"

    mkdir -p $LOGIC_DST_DIR/$(dirname $file) || fail
    $LUAC -o $LOGIC_DST_DIR/${file}c $file || fail
done
