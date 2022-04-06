#!/bin/bash

# test to see whether the output of solver.c matches the expected output

# $# is the number of command line arguments 
if [ $# -ne 2 ]
then 
    echo "Usage: ./run_server_tests.sh input expected_output (paths are relative to current directory)"
    exit 1
fi

# $1 is the first command line argument and $2 is the second
# compare outputs
./server $1 > result.out
diff -w result.out $2
if [ $? -eq 0 ]
then
    echo "Test passed"
    rm result.out
else 
    echo "Test failed"
    rm result.out
fi

