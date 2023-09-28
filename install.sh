#!/bin/bash

# Parameters
BUILD=build
BUILD_TYPE=Release
PARALLEL=8

# Remove the build directory if it exists
if [ -d "$BUILD" ]; then
  rm -rf $BUILD
fi

# Configure CMake
cmake -B $BUILD -DCMAKE_BUILD_TYPE=$BUILD_TYPE

# Build
cmake --build $BUILD --parallel $PARALLEL --config $BUILD_TYPE

# Move the executables to the main directory
mv $BUILD/smashpp .
mv $BUILD/smashpp-inv-rep .
mv $BUILD/exclude_N .
