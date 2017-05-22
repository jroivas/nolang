#!/bin/bash

set -e
set -u

MYDIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

ls "${MYDIR}/examples/"*.nolang | while read p; do
    # FIXME Path to binary
    if ! ./nolang-pure "$p" > tst.c 2> /dev/null ; then
        echo "-- FAILED PARSING $p";
    elif ! gcc tst.c -o tst.out 2> /dev/null ; then
        echo "-- FAILED COMPILE $p";
    else
        echo "++ SUCCESS        $p";
    fi
done
