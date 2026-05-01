if(NOT DEFINED WORKDIR OR NOT DEFINED SMASHPP OR NOT DEFINED REF OR NOT DEFINED TAR)
  message(FATAL_ERROR "WORKDIR, SMASHPP, REF, and TAR must be defined.")
endif()

file(REMOVE_RECURSE "${WORKDIR}")
file(MAKE_DIRECTORY "${WORKDIR}/sequential")
file(MAKE_DIRECTORY "${WORKDIR}/parallel")

execute_process(
    COMMAND "${SMASHPP}" -n 1 -fmt json -r "${REF}" -t "${TAR}"
    WORKING_DIRECTORY "${WORKDIR}/sequential"
    RESULT_VARIABLE sequential_result
    OUTPUT_VARIABLE sequential_stdout
    ERROR_VARIABLE sequential_stderr)

if(NOT sequential_result EQUAL 0)
  message(FATAL_ERROR
          "Sequential consistency run failed with exit code ${sequential_result}\n"
          "stdout:\n${sequential_stdout}\n"
          "stderr:\n${sequential_stderr}")
endif()

execute_process(
    COMMAND "${SMASHPP}" -n 2 -fmt json -r "${REF}" -t "${TAR}"
    WORKING_DIRECTORY "${WORKDIR}/parallel"
    RESULT_VARIABLE parallel_result
    OUTPUT_VARIABLE parallel_stdout
    ERROR_VARIABLE parallel_stderr)

if(NOT parallel_result EQUAL 0)
  message(FATAL_ERROR
          "Parallel consistency run failed with exit code ${parallel_result}\n"
          "stdout:\n${parallel_stdout}\n"
          "stderr:\n${parallel_stderr}")
endif()

set(sequential_json "${WORKDIR}/sequential/ref.tar.json")
set(parallel_json "${WORKDIR}/parallel/ref.tar.json")

if(NOT EXISTS "${sequential_json}")
  message(FATAL_ERROR "Expected sequential JSON output was not created: ${sequential_json}")
endif()
if(NOT EXISTS "${parallel_json}")
  message(FATAL_ERROR "Expected parallel JSON output was not created: ${parallel_json}")
endif()

file(READ "${sequential_json}" sequential_content)
file(READ "${parallel_json}" parallel_content)

string(REGEX REPLACE "[ \t]*\"parameters\"[^\n]*\n" "" sequential_content "${sequential_content}")
string(REGEX REPLACE "[ \t]*\"parameters\"[^\n]*\n" "" parallel_content "${parallel_content}")

if(NOT sequential_content STREQUAL parallel_content)
  message(FATAL_ERROR
          "Parallel run output differs from sequential run.\n"
          "sequential:\n${sequential_content}\n"
          "parallel:\n${parallel_content}")
endif()
