#!/bin/bash

cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=1 . -G Ninja -B build
cp build/compile_commands.json ./compile_commands.json
