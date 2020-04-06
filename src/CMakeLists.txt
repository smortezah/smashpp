cmake_minimum_required(VERSION 3.9)
project(smashpp)

if (NOT EXISTS ${CMAKE_BINARY_DIR}/CMakeCache.txt)
  if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
  endif()
endif()

# set(CMAKE_C_COMPILER /usr/local/gcc-8.2/bin/gcc-8.2)
# set(CMAKE_CXX_COMPILER /usr/local/gcc-8.2/bin/g++-8.2)

# set(CMAKE_C_COMPILER C:\\mingw\\mingw64\\bin\\x86_64-w64-mingw32-gcc.exe)
# set(CMAKE_CXX_COMPILER C:\\MinGW\\bin\\g++.exe)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -O3 -Wall -ffast-math -fno-math-errno -funsafe-math-optimizations -ffinite-math-only -fbranch-target-load-optimize -fforce-addr -march=native")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -pthread -Wall -O0 -g")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -pthread -O3 -pg") # mem prof

add_executable(smashpp 
  par.cpp
  fcm.cpp
  tbl64.cpp
  tbl32.cpp
  cmls4.cpp
  logtbl8.cpp
  filter.cpp
  segment.cpp
  output.cpp
  color.cpp
  svg.cpp
  vizpaint.cpp
  main.cpp
)

target_compile_features(smashpp PRIVATE cxx_std_14)

find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    target_link_libraries(smashpp PRIVATE OpenMP::OpenMP_CXX)
endif()

add_executable(smashpp-inv-rep invRep.cpp)

add_executable(exclude_N excludeN.cpp)