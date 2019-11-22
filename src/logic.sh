#!/bin/sh

. helpers.sh

LUAC=../lua-5.3.5/src/luac

mkdir -p ../../logic

cd logic

for file in $(find . -maxdepth 99 -type f); do
    mkdir -p ../../$(dirname $file)
    $LUAC -o ../../${file}c $file || exit 1
done
