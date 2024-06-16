#!/bin/bash

# cmake -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_C_COMPILER=clang -DCMAKE_EXPORT_COMPILE_COMMANDS=1 . -G Ninja -B build
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=1 . -G Ninja -B build
cp build/compile_commands.json ./compile_commands.json
