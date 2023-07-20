#!/usr/bin/env bash

ROOT="${BASEDIR:=$(dirname $0)}"
echo "ROOT: $ROOT"
PROTO_SRC_DIR=$ROOT/src

CODEGEN_DIR=$ROOT/_build
PROTO_DST_DIR=$CODEGEN_DIR/src
LIBS="-lprotobuf -lpthread"
SRC_DIRS="src"

CPPFLAGS="-O3 -std=c++17 -c -fPIC"
ALL_OBJS=""

mkdir -p $CODEGEN_DIR

if [[ "$OSTYPE" == "darwin"* ]]; then
    # Mac OSX
    CPPFLAGS="${CPPFLAGS} -stdlib=libc++"
fi

for f in $PROTO_SRC_DIR/*.proto
do
    echo "Processing proto $f"
    protoc -I=$ROOT --cpp_out=$CODEGEN_DIR $f
    filename=$(basename -- "$f")
    filename="${filename%.*}"
    obj_path=$PROTO_DST_DIR/$filename.o
    g++ $CPPFLAGS -o $obj_path -I=$ROOT -I$CODEGEN_DIR -I. $PROTO_DST_DIR/$filename.pb.cc
    ALL_OBJS="$ALL_OBJS $obj_path"
done

# cc files in SRC_DIRS
for src_d in $SRC_DIRS
do
    dst_dir=$CODEGEN_DIR/$src_d
    mkdir -p $dst_dir
    for f in $src_d/*.cc
    do
        echo "Compiling cc $f"
        filename=$(basename -- "$f")
        filename="${filename%.*}"
        obj_path=$dst_dir/$filename.o
        g++ $CPPFLAGS -o $obj_path -I$CODEGEN_DIR -I. -I$ROOT $f
        ALL_OBJS="$ALL_OBJS $obj_path"
    done
done