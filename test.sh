#!/bin/bash

set -e
set -u

MYDIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

function success() {
    echo "++ SUCCESS        $1";
}

function failed() {
    echo "-- FAILED $1 $2";
}

ls "${MYDIR}/examples/"*.nolang | while read p; do
    # FIXME Path to binary
    if ! ./nolang-pure "$p" > tst.c 2> /dev/null ; then
        failed "PARSING" "$p"
    elif ! gcc tst.c -o tst.out 2> /dev/null ; then
        if ! gcc tst.c -c -o tst.out 2> /dev/null ; then
            failed "COPMILE" "$p"
        else
            success $p
        fi
    else
        success $p
    fi
done
