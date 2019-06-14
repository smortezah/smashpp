#!/bin/bash

#rm -fr build
#mkdir build
cd build
cmake ../src
make -j4
mv smashpp ..
cd ..