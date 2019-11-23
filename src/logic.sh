#!/bin/bash

. shared/const.sh
. shared/helpers.sh

mkdir -p $LOGIC_DST_DIR || fail

cd $LOGIC_SRC_DIR || fail

for file in $(find . -maxdepth 99 -type f); do
    echo -e "\e[0;36mlogic:\e[0m $file"

    mkdir -p $LOGIC_DST_DIR/$(dirname $file) || fail
    $LUAC -o $LOGIC_DST_DIR/${file}c $file || fail
done
