#!/bin/bash

make clean
make
./server &
./client localhost
make clean
