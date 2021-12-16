cmake_minimum_required(VERSION 3.5)
# cmake_minimum_required(VERSION 3.9)
project(smashpp)

if (NOT EXISTS ${CMAKE_BINARY_DIR}/CMakeCache.txt)
  if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
  endif()
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -O3 -Wall")
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffast-math -fno-math-errno -funsafe-math-optimizations -ffinite-math-only -fforce-addr -fbranch-target-load-optimize")
endif()
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -Wall -O0 -g")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -O3 -pg") # mem prof

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

find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    target_link_libraries(smashpp PRIVATE OpenMP::OpenMP_CXX)
endif()

add_executable(smashpp-inv-rep invRep.cpp)

add_executable(exclude_N excludeN.cpp)
