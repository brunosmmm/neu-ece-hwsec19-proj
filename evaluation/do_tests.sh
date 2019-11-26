#!/bin/bash

# from stack overflow
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

for test in "${@:1}"; do
    echo "Launching $test"
    $BOOM_SIM $DIR/$test 2>/dev/null 1> ${test}.result_boom &
    $ROCKET_SIM $DIR/$test 2>/dev/null 1> ${test}.result_rocket &
done

# wait for completion
echo "Waiting for completion..."
wait

rm -rf boom.results rocket.results
for test in "${@:1}"; do
    sed -r 's/[^0-9]*([0-9]+)\s+cycles,\s*([0-9]+).*$/\1, \2/' $test.result_boom >> boom.results
    sed -r 's/[^0-9]*([0-9]+)\s+cycles,\s*([0-9]+).*$/\1, \2/' $test.result_rocket >> rocket.results
done

echo "Done"
