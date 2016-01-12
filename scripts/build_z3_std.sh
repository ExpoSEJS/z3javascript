#!/bin/bash -e
cd z3

if [ -e build ]; then
    rm -rf ./build/
fi

python scripts/mk_make.py --x86 --githash=$(git rev-parse HEAD) --staticlib
cd build

make
cp libz3.so* ../../compiled/