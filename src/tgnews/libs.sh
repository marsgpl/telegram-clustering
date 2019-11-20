#!/bin/sh

for dir in $(find libs/* -maxdepth 1 -type d); do
    cd $dir && make && make install
done
