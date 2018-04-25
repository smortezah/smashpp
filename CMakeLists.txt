cmake_minimum_required(VERSION 3.3.2)
#project(smashpp)
SET(CMAKE_BUILD_TYPE Release)
#SET(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -pthread -Wall")

add_executable(smashpp  src/main.cpp src/fcm.cpp src/tbl64.cpp src/tbl32.cpp
        src/cmls4.cpp src/logtbl8.cpp src/def.hpp src/par.hpp)

#add_executable(bench    src/bench.cpp)

#file(GLOB SOURCE_FILES "src/*.cpp" "src/*.hpp" "src/*.c")
#add_executable(smashpp ${SOURCE_FILES})