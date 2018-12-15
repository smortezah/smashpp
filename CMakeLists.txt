project(smashpp)
cmake_minimum_required(VERSION 3.3.2)
# set(CMAKE_BUILD_TYPE RelWithDebInfo)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -pthread -O3 -Wall")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -pthread -O3 -Wall")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -pthread -O3 -pg") # mem prof
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -pthread -O3 -fopenmp -D_GLIBCXX_PARALLEL")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -pthread -O3")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -pthread")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -pthread")

add_executable(
  smashpp  src/main.cpp
           src/def.hpp
           src/exception.hpp
           src/par.hpp
           src/assert.hpp
           src/time.hpp
           src/file.hpp
           src/container.hpp
           src/number.hpp
           src/naming.hpp
           src/mdlpar.hpp
           src/string.hpp
           src/fcm.cpp
           src/tbl64.cpp
           src/tbl32.cpp
           src/cmls4.cpp
           src/logtbl8.cpp
           src/filter.cpp
           src/segment.cpp
           src/svg.cpp
           src/vizpaint.cpp
           src/vizdef.hpp
)


#add_executable(smashpp-inv-rep src/invRep.cpp)

#add_executable(excule_N src/excludeN.cpp)

#add_executable(bench    src/bench.cpp)

#file(GLOB SOURCE_FILES "src/*.cpp" "src/*.hpp" "src/*.c")
#add_executable(smashpp ${SOURCE_FILES})