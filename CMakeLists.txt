cmake_minimum_required(VERSION 4.0.0)

project(smashpp VERSION 26.04 LANGUAGES CXX)

include(GNUInstallDirs)

option(SMASHPP_ENABLE_OPENMP "Enable OpenMP support when available." ON)
option(SMASHPP_ENABLE_UNSAFE_FAST_MATH
       "Enable aggressive GNU floating-point optimizations." OFF)

if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

find_package(Threads REQUIRED)

function(smashpp_configure_target target_name)
  target_compile_features(${target_name} PRIVATE cxx_std_20)
  set_target_properties(${target_name} PROPERTIES CXX_EXTENSIONS OFF)

  if (MSVC)
    target_compile_options(${target_name} PRIVATE /W4)
  else()
    target_compile_options(${target_name} PRIVATE -Wall -Wextra)
  endif()
endfunction()

set(SMASHPP_SOURCES
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
    src/main.cpp)

add_executable(smashpp ${SMASHPP_SOURCES})
add_executable(smashpp-inv-rep src/invRep.cpp)
add_executable(exclude_N src/excludeN.cpp)

smashpp_configure_target(smashpp)
smashpp_configure_target(smashpp-inv-rep)
smashpp_configure_target(exclude_N)

target_link_libraries(smashpp PRIVATE Threads::Threads)

if (SMASHPP_ENABLE_OPENMP)
  find_package(OpenMP QUIET COMPONENTS CXX)
  if (OpenMP_CXX_FOUND)
    target_link_libraries(smashpp PRIVATE OpenMP::OpenMP_CXX)
  else()
    message(STATUS "OpenMP was not found; building smashpp without OpenMP.")
  endif()
endif()

if (SMASHPP_ENABLE_UNSAFE_FAST_MATH AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  target_compile_options(
      smashpp
      PRIVATE -ffast-math -fno-math-errno -funsafe-math-optimizations
              -ffinite-math-only)
endif()

install(TARGETS smashpp smashpp-inv-rep exclude_N
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES README.md LICENSE DESTINATION ${CMAKE_INSTALL_DOCDIR})
