if(NOT DEFINED WORKDIR OR NOT DEFINED SMASHPP)
  message(FATAL_ERROR "WORKDIR and SMASHPP must be defined.")
endif()

file(REMOVE_RECURSE "${WORKDIR}")
file(MAKE_DIRECTORY "${WORKDIR}")

set(run_a "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n")
set(run_c "CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC\n")

file(WRITE "${WORKDIR}/ref.fa" ">ref\n")
file(WRITE "${WORKDIR}/tar.fa" ">tar\n")
foreach(_i RANGE 1 8)
  file(APPEND "${WORKDIR}/ref.fa" "${run_a}")
  file(APPEND "${WORKDIR}/tar.fa" "${run_c}")
endforeach()

execute_process(
    COMMAND "${SMASHPP}" -fmt json -nr -n 1 -r "${WORKDIR}/ref.fa" -t "${WORKDIR}/tar.fa"
    WORKING_DIRECTORY "${WORKDIR}"
    RESULT_VARIABLE smashpp_result
    OUTPUT_VARIABLE smashpp_stdout
    ERROR_VARIABLE smashpp_stderr)

if(NOT smashpp_result EQUAL 0)
  message(FATAL_ERROR
          "Smash++ no-segments guidance run failed with exit code ${smashpp_result}\n"
          "stdout:\n${smashpp_stdout}\n"
          "stderr:\n${smashpp_stderr}")
endif()

set(json_file "${WORKDIR}/ref.fa.tar.fa.json")
if(NOT EXISTS "${json_file}")
  message(FATAL_ERROR "Expected JSON output was not created: ${json_file}")
endif()

file(READ "${json_file}" json_content)
if(NOT json_content MATCHES "\"reference\"[ \t\r\n]*:[ \t\r\n]*\"ref.fa\"")
  message(FATAL_ERROR "JSON output did not preserve the original reference name.")
endif()
if(NOT json_content MATCHES "\"target\"[ \t\r\n]*:[ \t\r\n]*\"tar.fa\"")
  message(FATAL_ERROR "JSON output did not preserve the original target name.")
endif()
if(NOT json_content MATCHES "\"positions\"[ \t\r\n]*:[ \t\r\n]*\\[[ \t\r\n]*\\]")
  message(FATAL_ERROR "JSON output did not contain an empty positions array.")
endif()

if(NOT smashpp_stderr MATCHES "no similar regions passed the current segmentation[ \t\r\n]+settings")
  message(FATAL_ERROR "Expected no-segments warning was not printed.")
endif()
if(NOT smashpp_stderr MATCHES "-th/--threshold")
  message(FATAL_ERROR "Expected warning to mention threshold tuning.")
endif()
if(NOT smashpp_stderr MATCHES "-m/--min-segment-size")
  message(FATAL_ERROR "Expected warning to mention minimum segment size tuning.")
endif()
if(NOT smashpp_stderr MATCHES "-fs/--filter-scale L")
  message(FATAL_ERROR "Expected warning to mention filter scale tuning.")
endif()
if(NOT smashpp_stderr MATCHES "-d/--sampling-step")
  message(FATAL_ERROR "Expected warning to mention sampling-step tuning.")
endif()
