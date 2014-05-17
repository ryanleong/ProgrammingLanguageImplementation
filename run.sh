#!/bin/bash

# compile
make

# run wiz
./wiz -p "$1" > out.temp

# run oz
cd oz
make
cd ..
./oz/oz out.temp 

# delete out.temp
rm -f out.temp
