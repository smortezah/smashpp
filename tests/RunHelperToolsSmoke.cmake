if(NOT DEFINED WORKDIR OR NOT DEFINED EXCLUDE_N OR NOT DEFINED INV_REP)
  message(FATAL_ERROR "WORKDIR, EXCLUDE_N, and INV_REP must be defined.")
endif()

file(REMOVE_RECURSE "${WORKDIR}")
file(MAKE_DIRECTORY "${WORKDIR}")

set(exclude_input "${WORKDIR}/exclude_n_input.seq")
file(WRITE "${exclude_input}" "ANCGNTN")

execute_process(
    COMMAND "${EXCLUDE_N}"
    INPUT_FILE "${exclude_input}"
    RESULT_VARIABLE exclude_result
    OUTPUT_VARIABLE exclude_output
    ERROR_VARIABLE exclude_stderr)

if(NOT exclude_result EQUAL 0)
  message(FATAL_ERROR
          "exclude_N failed with exit code ${exclude_result}\n"
          "stderr:\n${exclude_stderr}")
endif()

if(NOT exclude_output STREQUAL "ACGT")
  message(FATAL_ERROR "exclude_N produced unexpected output: ${exclude_output}")
endif()

set(inv_input "${WORKDIR}/inv_input.seq")
set(inv_output "${WORKDIR}/inv_output.seq")
file(WRITE "${inv_input}" "ACGTN\n")

execute_process(
    COMMAND "${INV_REP}" "${inv_input}" "${inv_output}"
    RESULT_VARIABLE inv_result
    OUTPUT_VARIABLE inv_stdout
    ERROR_VARIABLE inv_stderr)

if(NOT inv_result EQUAL 0)
  message(FATAL_ERROR
          "smashpp-inv-rep failed with exit code ${inv_result}\n"
          "stdout:\n${inv_stdout}\n"
          "stderr:\n${inv_stderr}")
endif()

if(NOT EXISTS "${inv_output}")
  message(FATAL_ERROR "smashpp-inv-rep did not create the output file.")
endif()

file(READ "${inv_output}" inv_content)
if(NOT inv_content STREQUAL "NACGT\n")
  message(FATAL_ERROR
          "smashpp-inv-rep produced unexpected output: ${inv_content}")
endif()
