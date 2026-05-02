if(NOT DEFINED SOURCE_DIR OR NOT DEFINED WORKDIR)
  message(FATAL_ERROR "SOURCE_DIR and WORKDIR must be defined.")
endif()
if(NOT DEFINED EXECUTABLE_SUFFIX)
  set(EXECUTABLE_SUFFIX "")
endif()
if(NOT DEFINED CONFIG OR CONFIG STREQUAL "")
  set(CONFIG Release)
endif()

set(expected_version "99.99-test")
set(build_dir "${WORKDIR}/build")

file(REMOVE_RECURSE "${WORKDIR}")
file(MAKE_DIRECTORY "${WORKDIR}")

execute_process(
    COMMAND "${CMAKE_COMMAND}"
            -S
            "${SOURCE_DIR}"
            -B
            "${build_dir}"
            -DCMAKE_BUILD_TYPE=Release
            -DBUILD_TESTING=OFF
            -DSMASHPP_ENABLE_OPENMP=OFF
            "-DSMASHPP_VERSION_OVERRIDE=${expected_version}"
    RESULT_VARIABLE configure_result
    OUTPUT_VARIABLE configure_stdout
    ERROR_VARIABLE configure_stderr)

if(NOT configure_result EQUAL 0)
  message(FATAL_ERROR
          "Version override configure failed with exit code ${configure_result}\n"
          "stdout:\n${configure_stdout}\n"
          "stderr:\n${configure_stderr}")
endif()

execute_process(
    COMMAND "${CMAKE_COMMAND}"
            --build
            "${build_dir}"
            --target
            smashpp
            --config
            "${CONFIG}"
            --parallel
            4
    RESULT_VARIABLE build_result
    OUTPUT_VARIABLE build_stdout
    ERROR_VARIABLE build_stderr)

if(NOT build_result EQUAL 0)
  message(FATAL_ERROR
          "Version override build failed with exit code ${build_result}\n"
          "stdout:\n${build_stdout}\n"
          "stderr:\n${build_stderr}")
endif()

set(candidate_executables
    "${build_dir}/smashpp${EXECUTABLE_SUFFIX}"
    "${build_dir}/${CONFIG}/smashpp${EXECUTABLE_SUFFIX}"
    "${build_dir}/Release/smashpp${EXECUTABLE_SUFFIX}")
set(smashpp "")
foreach(candidate ${candidate_executables})
  if(EXISTS "${candidate}")
    set(smashpp "${candidate}")
    break()
  endif()
endforeach()

if(smashpp STREQUAL "")
  message(FATAL_ERROR "Version override build did not create a smashpp executable.")
endif()

execute_process(
    COMMAND "${smashpp}" --version
    RESULT_VARIABLE version_result
    OUTPUT_VARIABLE version_stdout
    ERROR_VARIABLE version_stderr)

if(NOT version_result EQUAL 0)
  message(FATAL_ERROR
          "Version override executable failed with exit code ${version_result}\n"
          "stdout:\n${version_stdout}\n"
          "stderr:\n${version_stderr}")
endif()

set(version_output "${version_stdout}\n${version_stderr}")
if(NOT version_output MATCHES "Smash\\+\\+ ${expected_version}")
  message(FATAL_ERROR
          "Expected version override ${expected_version}, got:\n"
          "stdout:\n${version_stdout}\n"
          "stderr:\n${version_stderr}")
endif()

