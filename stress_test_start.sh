#!/bin/bash
g++ stress_test/stress_test.cpp -o ./bin/stress  --std=c++11 -pthread  
./bin/stress
