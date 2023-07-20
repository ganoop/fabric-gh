#!/usr/bin/env bash

echo "Compiling gtest..."

for f in $GTEST_DIR/*.cc
do
    filename=$(basename -- "$f")
    filename="${filename%.*}"
    g++ $CPPFLAGS -o $GTEST_DST_DIR/$filename.o -I. $f
done
