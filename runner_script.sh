#!/bin/bash
mkdir build
cd build
cmake .. && make -j
cd ..
./build/bin/tester 100000 1234
