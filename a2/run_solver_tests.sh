#!/bin/bash

# test to see whether the output of solver.c matches the expected output

# $# is the number of command line arguments 
if [ $# -ne 2 ]
then 
    echo "Usage: ./run_solver_tests.sh input expected_output"
    exit 1
fi

# $1 is the first command line argument and $2 is the second
./solver $1 | sort > tmp.out
sort $2 | diff tmp.out - > diff.out
if [ $? -eq 0 ]
then
    echo "Test passed"
    rm tmp.out
else 
    echo "Test failed: check solver_sorted.out for output"
fi

rm diff.out
