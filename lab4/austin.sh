#!/bin/bash

echo "Start of file" > test_output.txt


./strip "Hello!!!! ?? ! This is cool!??" >> test_output.txt

./strip "??? ???" >> test_output.txt

./strip " " >> test_output.txt
./strip "AHHHHHH ??" >> test_output.txt

./greeting "Hello there cutie pie" "Austin" >> test_output.txt

./greeting "Good morrow sir" "Ifaz" >> test_output.txt

./greeting "Sup yo" "Tony" >> test_output.txt

./greeting "THIS IS A VERY LONG GREETING IT IS VERY LONG AND VERY COOL!" "Cool Guy" >> test_output.txt

./greeting " " " " >> test_output.txt

./greeting "Test with no name" " " >> test_output.txt

./greeting "" "NoText" >> test_output.txt

./greeting "SPACE" "Austin B Lackman" >> test_output.txt

./truncate 5 "function" >> test_output.txt

./truncate 0 "function" >> test_output.txt

./truncate -5 "function" >> test_output.txt

./truncate 100 "function" >> test_output.txt

./truncate 8 "function" >> test_output.txt

./compare "ABCD" "ABCD" >> test_output.txt

./compare "ONE" >> test_output.txt

./compare "ONE" "TWO" "THREE" >> test_output.txt

./compare >> test_output.txt

./compare "Canada" "America" >> test_output.txt

./compare "Canada" "Canada" >> test_output.txt
