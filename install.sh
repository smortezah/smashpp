#!/bin/bash

# rm -fr build
# mkdir build
cd build
cmake ../src
make -j8
mv smashpp ..
cd ..
