#!/usr/bin/env bash
./echo_arg 209 > echo_out.txt
./echo_stdin < echo_stdin.c
./count 210 | wc -m # This should output 520
ls -S | ./echo_stdin
