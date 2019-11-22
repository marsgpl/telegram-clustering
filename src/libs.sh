#!/bin/sh

. helpers.sh

cd libs

for dir in $(find . -mindepth 0 -maxdepth 0 -type d); do
    cd "$dir"
    make && make install || exit 1
    cd ..
done
