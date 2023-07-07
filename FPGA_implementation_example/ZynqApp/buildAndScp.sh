#!/bin/bash

arm-linux-gnueabihf-g++ main.cpp -std=c++17 -o main -O3 -pthread -IXDMA_API/API/include/
scp main node_106:
