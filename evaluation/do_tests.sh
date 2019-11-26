#!/bin/bash

BOOM_SIM=simulator-example-SimonBoomConfig
ROCKET_SIM=simulator-example-SimonRocketConfig

for test in $@; do
    ./$BOOM_SIM $test > ${test}.result &
    ./$ROCKET_SIM $test > ${test}.result &
done
