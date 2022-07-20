#!/bin/bash

# Parameters
BUILD_TYPE=Release
BUILD=build
PARALLEL=8

# Configure CMake
cmake -B $BUILD -DCMAKE_BUILD_TYPE=$BUILD_TYPE

# Build
cmake --build $BUILD --parallel $PARALLEL --config $BUILD_TYPE

# Move the executables to the main directory
mv $BUILD/smashpp .
mv $BUILD/smashpp-inv-rep .
mv $BUILD/exclude_N .
