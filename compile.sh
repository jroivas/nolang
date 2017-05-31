#!/bin/bash

CC=${CC:-gcc}
base=$(basename "$1" .nolang)
./nolang-pure $1 > "${base}_output.c"
extra=
lib=0
if ! grep main "${base}_output.c" > /dev/null ; then
    extra="-c -fPIC"
    lib=1
fi
${CC} ${extra} -O2 "${base}_output.c" -o "${base}.out"
if [ "${lib}" -eq 1 ]; then
    ${CC} -shared -Wl,-soname,"lib${base}.so.0" -o "lib${base}.so.0.0.1" "${base}.out"
fi
