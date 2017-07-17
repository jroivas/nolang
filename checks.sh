#!/bin/bash

MYDIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

cppcheck --error-exitcode=1 "${MYDIR}/3pp" "${MYDIR}/src"
