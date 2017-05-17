#!/bin/bash

set -e
set -u

MYDIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

ls "${MYDIR}/examples/"*.nolang | while read p; do
    # FIXME Path to binary
    if ! ./nolang-pure "$p" > tst.c ; then
        echo "-- FAILED $p";
    else
        gcc tst.c -o tst.out ||  echo "-- FAILED $p";
    fi
done
