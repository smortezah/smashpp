cmake_minimum_required(VERSION 4.0.0)

set(SMASHPP_VERSION_OVERRIDE "" CACHE STRING "Override the version embedded in Smash++")
set(SMASHPP_VERSION "0.0.0-dev")

if (SMASHPP_VERSION_OVERRIDE)
  set(SMASHPP_VERSION "${SMASHPP_VERSION_OVERRIDE}")
elseif(DEFINED ENV{GITHUB_REF_TYPE} AND "$ENV{GITHUB_REF_TYPE}" STREQUAL "tag")
  set(SMASHPP_VERSION "$ENV{GITHUB_REF_NAME}")
  string(REGEX REPLACE "^v" "" SMASHPP_VERSION "${SMASHPP_VERSION}")
else()
  find_package(Git QUIET)

  if (Git_FOUND AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/.git")
    execute_process(
        COMMAND "${GIT_EXECUTABLE}" describe --tags --exact-match
                --match "v[0-9]*" --match "[0-9]*"
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        OUTPUT_VARIABLE SMASHPP_GIT_VERSION
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
        RESULT_VARIABLE SMASHPP_GIT_VERSION_RESULT)

    if (NOT SMASHPP_GIT_VERSION_RESULT EQUAL 0)
      execute_process(
          COMMAND "${GIT_EXECUTABLE}" describe --tags --dirty --always
                  --match "v[0-9]*" --match "[0-9]*"
          WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
          OUTPUT_VARIABLE SMASHPP_GIT_VERSION
          OUTPUT_STRIP_TRAILING_WHITESPACE
          ERROR_QUIET
          RESULT_VARIABLE SMASHPP_GIT_VERSION_RESULT)
    endif()

    if (SMASHPP_GIT_VERSION_RESULT EQUAL 0 AND SMASHPP_GIT_VERSION)
      string(REGEX REPLACE "^v" "" SMASHPP_VERSION "${SMASHPP_GIT_VERSION}")
    endif()
  endif()
endif()

project(smashpp LANGUAGES CXX)

include(GNUInstallDirs)

set(SMASHPP_GENERATED_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/version.hpp.in"
    "${SMASHPP_GENERATED_INCLUDE_DIR}/smashpp/version.hpp"
    @ONLY)
message(STATUS "Smash++ version: ${SMASHPP_VERSION}")

option(SMASHPP_ENABLE_OPENMP "Enable OpenMP support when available." ON)
option(SMASHPP_ENABLE_SANITIZERS
       "Enable address and undefined behavior sanitizers." OFF)
option(SMASHPP_ENABLE_UNSAFE_FAST_MATH
       "Enable aggressive GNU floating-point optimizations." OFF)
option(BUILD_TESTING "Enable the Smash++ test suite." ON)

if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

find_package(Threads REQUIRED)

add_subdirectory(third_party/nlohmann_json)

function(smashpp_configure_target target_name)
  target_compile_features(${target_name} PRIVATE cxx_std_20)
  target_include_directories(${target_name} PRIVATE "${SMASHPP_GENERATED_INCLUDE_DIR}")
  set_target_properties(${target_name} PROPERTIES CXX_EXTENSIONS OFF)

  if (MSVC)
    target_compile_options(${target_name} PRIVATE /W4)
  else()
    target_compile_options(${target_name} PRIVATE -Wall -Wextra)
  endif()

  if (SMASHPP_ENABLE_SANITIZERS)
    if (MSVC)
      message(FATAL_ERROR
              "SMASHPP_ENABLE_SANITIZERS is not supported with MSVC.")
    endif()
    if (NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
      message(FATAL_ERROR
              "SMASHPP_ENABLE_SANITIZERS requires Clang or GCC.")
    endif()

    target_compile_options(
        ${target_name} PRIVATE -fsanitize=address,undefined
                               -fno-omit-frame-pointer)
    target_link_options(
        ${target_name} PRIVATE -fsanitize=address,undefined
                               -fno-omit-frame-pointer)
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

target_link_libraries(smashpp PRIVATE Threads::Threads nlohmann_json::nlohmann_json)

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

if(BUILD_TESTING)
  enable_testing()

  add_test(NAME smashpp-help COMMAND $<TARGET_FILE:smashpp> --help)
  set_tests_properties(smashpp-help PROPERTIES PASS_REGULAR_EXPRESSION "SYNOPSIS")

  add_test(NAME smashpp-version COMMAND $<TARGET_FILE:smashpp> --version)
  set_tests_properties(smashpp-version PROPERTIES PASS_REGULAR_EXPRESSION "Smash\\+\\+ .+")

  add_test(NAME smashpp-viz-help COMMAND $<TARGET_FILE:smashpp> viz --help)
  set_tests_properties(smashpp-viz-help PROPERTIES PASS_REGULAR_EXPRESSION "SYNOPSIS")

  add_test(
      NAME smashpp-missing-format
      COMMAND ${CMAKE_COMMAND}
              "-DEXECUTABLE=$<TARGET_FILE:smashpp>"
              -DARG_0=-fmt
              "-DEXPECTED_TEXT=output format not specified"
              -P
              "${CMAKE_CURRENT_SOURCE_DIR}/tests/ExpectCommandFailure.cmake")

  add_test(
      NAME smashpp-viz-missing-output
      COMMAND ${CMAKE_COMMAND}
              "-DEXECUTABLE=$<TARGET_FILE:smashpp>"
              -DARG_0=viz
              -DARG_1=-o
              "-DEXPECTED_TEXT=output image name not specified"
              -P
              "${CMAKE_CURRENT_SOURCE_DIR}/tests/ExpectCommandFailure.cmake")

  add_test(
      NAME smashpp-example-smoke
      COMMAND ${CMAKE_COMMAND}
              "-DWORKDIR=${CMAKE_CURRENT_BINARY_DIR}/tests/smashpp-example-smoke"
              "-DSMASHPP=$<TARGET_FILE:smashpp>"
              "-DREF=${CMAKE_CURRENT_SOURCE_DIR}/example/ref"
              "-DTAR=${CMAKE_CURRENT_SOURCE_DIR}/example/tar"
              -P
              "${CMAKE_CURRENT_SOURCE_DIR}/tests/RunSmashppSmoke.cmake")

  add_test(
      NAME helper-tools-smoke
      COMMAND ${CMAKE_COMMAND}
              "-DWORKDIR=${CMAKE_CURRENT_BINARY_DIR}/tests/helper-tools-smoke"
              "-DEXCLUDE_N=$<TARGET_FILE:exclude_N>"
              "-DINV_REP=$<TARGET_FILE:smashpp-inv-rep>"
              -P
              "${CMAKE_CURRENT_SOURCE_DIR}/tests/RunHelperToolsSmoke.cmake")

  add_test(
      NAME smashpp-input-normalization
      COMMAND ${CMAKE_COMMAND}
              "-DWORKDIR=${CMAKE_CURRENT_BINARY_DIR}/tests/smashpp-input-normalization"
              "-DSMASHPP=$<TARGET_FILE:smashpp>"
              -P
              "${CMAKE_CURRENT_SOURCE_DIR}/tests/RunInputNormalization.cmake")

  add_test(
      NAME smashpp-parallel-consistency
      COMMAND ${CMAKE_COMMAND}
              "-DWORKDIR=${CMAKE_CURRENT_BINARY_DIR}/tests/smashpp-parallel-consistency"
              "-DSMASHPP=$<TARGET_FILE:smashpp>"
              "-DREF=${CMAKE_CURRENT_SOURCE_DIR}/example/ref"
              "-DTAR=${CMAKE_CURRENT_SOURCE_DIR}/example/tar"
              -P
              "${CMAKE_CURRENT_SOURCE_DIR}/tests/RunParallelConsistency.cmake")

  add_test(
      NAME smashpp-invalid-base
      COMMAND ${CMAKE_COMMAND}
              "-DWORKDIR=${CMAKE_CURRENT_BINARY_DIR}/tests/smashpp-invalid-base"
              "-DSMASHPP=$<TARGET_FILE:smashpp>"
              -P
              "${CMAKE_CURRENT_SOURCE_DIR}/tests/RunInvalidBaseFailure.cmake")

  add_test(
      NAME smashpp-inv-rep-usage
      COMMAND ${CMAKE_COMMAND}
              "-DEXECUTABLE=$<TARGET_FILE:smashpp-inv-rep>"
              "-DEXPECTED_TEXT=Usage: smashpp-inv-rep"
              -P
              "${CMAKE_CURRENT_SOURCE_DIR}/tests/ExpectCommandFailure.cmake")
endif()

install(TARGETS smashpp smashpp-inv-rep exclude_N
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES README.md LICENSE DESTINATION ${CMAKE_INSTALL_DOCDIR})
