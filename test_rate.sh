#!/bin/bash

MYDIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

res=$("${MYDIR}/test.sh")
succ=$(echo "${res}" | grep SUCCESS | wc -l)
failparse=$(echo "${res}" | grep "FAILED PARSING" | wc -l)
failcomp=$(echo "${res}" | grep "FAILED COMP" | wc -l)
total=$(echo "${res}" | wc -l)
ratio=$(echo "scale=1;${succ}/${total}*100"|bc)
echo "Success      : ${succ}"
echo "Fail parse   : ${failparse}"
echo "Fail compile : ${failcomp}"
echo "Success ratio: ${ratio}%"
