# cmake_minimum_required(VERSION 2.8.12)
project(smashpp)
# set(CMAKE_BUILD_TYPE RelWithDebInfo)

# set(CMAKE_C_COMPILER /usr/local/gcc-8.2/bin/gcc-8.2)
# set(CMAKE_CXX_COMPILER /usr/local/gcc-8.2/bin/g++-8.2)

# set(CMAKE_C_COMPILER C:\\mingw-w64\\mingw64\\bin\\x86_64-w64-mingw32-gcc.exe)
# set(CMAKE_CXX_COMPILER C:\\mingw-w64\\mingw64\\bin\\x86_64-w64-mingw32-g++.exe)
# set(CMAKE_C_COMPILER C:\\mingw\\mingw64\\bin\\x86_64-w64-mingw32-gcc.exe)
# set(CMAKE_CXX_COMPILER C:\\mingw\\mingw64\\bin\\x86_64-w64-mingw32-g++.exe)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -pthread -O3 -Wall -ffast-math -fno-math-errno -funsafe-math-optimizations -ffinite-math-only -fbranch-target-load-optimize -fforce-addr -march=native")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -pthread -Wall -O0 -g")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -pthread -O3 -pg") # mem prof
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -pthread -O3 -fopenmp -D_GLIBCXX_PARALLEL")

add_executable(smashpp 
  par.cpp
  fcm.cpp
  tbl64.cpp
  tbl32.cpp
  cmls4.cpp
  logtbl8.cpp
  filter.cpp
  segment.cpp
  color.cpp
  svg.cpp
  vizpaint.cpp
  main.cpp
)

#add_executable(smashpp-inv-rep src/invRep.cpp)

#add_executable(excule_N src/excludeN.cpp)