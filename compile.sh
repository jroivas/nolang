#!/bin/bash

base=$(basename "$1" .nolang)
./nolang_pure $1 > "${base}_output.c"
gcc "${base}_output.c" -o "${base}.out"
