#!/usr/bin/env bash

TEST_DIRS="tests"

ROOT=$(dirname "$0")
rm -rf $ROOT/_build
GTEST_DIR=$ROOT/gtest
source $ROOT/build.sh

GTEST_DST_DIR=$CODEGEN_DIR/gtest

mkdir -p $GTEST_DST_DIR

echo "Compiling gtest..."

for f in $GTEST_DIR/*.cc
do
    filename=$(basename -- "$f")
    filename="${filename%.*}"
    g++ $CPPFLAGS -o $GTEST_DST_DIR/$filename.o -I. $f
done


failed=0
for d in $TEST_DIRS; do
    TEST_DST_DIR=$CODEGEN_DIR/$d
    mkdir -p $TEST_DST_DIR
    for f in $d/*.cc
    do
        echo "Compiling test $f"
        filename=$(basename -- "$f")
        filename="${filename%.*}"
        g++ $CPPFLAGS -o $TEST_DST_DIR/$filename.o -I$CODEGEN_DIR -I. $f
        g++ -g -o $TEST_DST_DIR/$filename $(ls $GTEST_DST_DIR/*.o) $ALL_OBJS $TEST_DST_DIR/$filename.o $LIBS
        ./$TEST_DST_DIR/$filename
        [ $? -ne 0 ] && failed=1
    done
done

exit $failed