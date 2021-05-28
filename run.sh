#!/bin/bash
# ./tiger.exe
gcc -no-pie -g out.s runtime.o
./a.out
echo $?