#!/bin/sh

LUAC=../lua-5.3.5/src/luac

mkdir -p ../../logic

for file in $(find logic -maxdepth 99 -type f); do
    $LUAC -s -o ../../${file}c $file
done
