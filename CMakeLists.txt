cmake_minimum_required(VERSION 3.3.2)
project(smashpp)
SET(CMAKE_BUILD_TYPE Release)
#SET(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -pthread")

file(GLOB SOURCE_FILES "src/*.hpp" "src/*.cpp" "src/*.h")
add_executable(smashpp ${SOURCE_FILES})