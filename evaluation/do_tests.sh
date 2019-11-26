#!/bin/bash

# from stack overflow
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

for test in "${@:2}"; do
    $BOOM_SIM $DIR/$test > ${test}.result_boom &
    $ROCKET_SIM $DIR/$test > ${test}.result_rocket &
done
