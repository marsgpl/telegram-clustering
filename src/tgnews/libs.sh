#!/bin/sh

for dir in $(find libs -mindepth 1 -maxdepth 1 -type d); do
    cd $dir || exit 1
    make && make install || exit 1
    cd ../..
done
