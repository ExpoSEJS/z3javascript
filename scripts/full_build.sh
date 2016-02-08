#!/bin/bash -e
./build_z3.sh
./build_z3_std.sh
./build_bindings.sh
./build_em_bindings.sh
./build_ref_bindings.sh
./compile_to_es5.sh