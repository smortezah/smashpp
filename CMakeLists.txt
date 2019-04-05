# cmake_minimum_required(VERSION 2.8.12)
project(smashpp)
# set(CMAKE_BUILD_TYPE RelWithDebInfo)

# set(CMAKE_C_COMPILER /usr/local/gcc-8.2/bin/gcc-8.2)
# set(CMAKE_CXX_COMPILER /usr/local/gcc-8.2/bin/g++-8.2)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -pthread -O3 -Wall -march=native")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -pthread -O3 -pg") # mem prof
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -pthread -O3 -fopenmp -D_GLIBCXX_PARALLEL")

add_executable(
  smashpp  
           src/par.cpp
           src/fcm.cpp
           src/tbl64.cpp
           src/tbl32.cpp
           src/cmls4.cpp
           src/logtbl8.cpp
           src/filter.cpp
           src/segment.cpp
           src/color.cpp
           src/svg.cpp
           src/vizpaint.cpp
           src/main.cpp
)


#add_executable(smashpp-inv-rep src/invRep.cpp)

#add_executable(excule_N src/excludeN.cpp)

#add_executable(bench    src/bench.cpp)

#file(GLOB SOURCE_FILES "src/*.cpp" "src/*.hpp" "src/*.c")
#add_executable(smashpp ${SOURCE_FILES})