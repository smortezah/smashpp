cmake_minimum_required(VERSION 3.5)

project(smashpp)

if (NOT EXISTS ${CMAKE_BINARY_DIR}/CMakeCache.txt)
  if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
  endif()
endif()

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED 14)

SET(CMAKE_CXX_FLAGS "-pthread -Wall" CACHE STRING "")
set(CMAKE_CXX_FLAGS_RELEASE "-O3" CACHE STRING "")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g" CACHE STRING "")

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffast-math -fno-math-errno -funsafe-math-optimizations -ffinite-math-only -fforce-addr -fbranch-target-load-optimize")
endif()

add_executable(smashpp 
  src/par.cpp
  src/fcm.cpp
  src/tbl64.cpp
  src/tbl32.cpp
  src/cmls4.cpp
  src/logtbl8.cpp
  src/filter.cpp
  src/segment.cpp
  src/output.cpp
  src/color.cpp
  src/svg.cpp
  src/vizpaint.cpp
  src/main.cpp
)

find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    target_link_libraries(smashpp PRIVATE OpenMP::OpenMP_CXX)
endif()

add_executable(smashpp-inv-rep src/invRep.cpp)

add_executable(exclude_N src/excludeN.cpp)
