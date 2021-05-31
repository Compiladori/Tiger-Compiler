#!/bin/bash
./tiger.exe > /dev/null
gcc -no-pie -g out.s runtime.o
./a.out
