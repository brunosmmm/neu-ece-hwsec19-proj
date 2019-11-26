#!/bin/bash

# from stack overflow
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

for test in "${@:2}"; do
    echo "Launching evaluation $evaluation"
    $BOOM_SIM $DIR/$test 2>/dev/null 1> ${test}.result_boom &
    $ROCKET_SIM $DIR/$test 2>/dev/null 1> ${test}.result_rocket &
done
