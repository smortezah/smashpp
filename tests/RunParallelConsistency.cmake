if(NOT DEFINED WORKDIR OR NOT DEFINED SMASHPP OR NOT DEFINED REF OR NOT DEFINED TAR)
  message(FATAL_ERROR "WORKDIR, SMASHPP, REF, and TAR must be defined.")
endif()

file(REMOVE_RECURSE "${WORKDIR}")

function(run_consistency label)
  set(sequential_dir "${WORKDIR}/${label}-sequential")
  set(parallel_dir "${WORKDIR}/${label}-parallel")
  file(MAKE_DIRECTORY "${sequential_dir}")
  file(MAKE_DIRECTORY "${parallel_dir}")

  execute_process(
      COMMAND "${SMASHPP}" -n 1 ${ARGN} -fmt json -r "${REF}" -t "${TAR}"
      WORKING_DIRECTORY "${sequential_dir}"
      RESULT_VARIABLE sequential_result
      OUTPUT_VARIABLE sequential_stdout
      ERROR_VARIABLE sequential_stderr)

  if(NOT sequential_result EQUAL 0)
    message(FATAL_ERROR
            "Sequential ${label} consistency run failed with exit code ${sequential_result}\n"
            "stdout:\n${sequential_stdout}\n"
            "stderr:\n${sequential_stderr}")
  endif()

  execute_process(
      COMMAND "${SMASHPP}" -n 2 ${ARGN} -fmt json -r "${REF}" -t "${TAR}"
      WORKING_DIRECTORY "${parallel_dir}"
      RESULT_VARIABLE parallel_result
      OUTPUT_VARIABLE parallel_stdout
      ERROR_VARIABLE parallel_stderr)

  if(NOT parallel_result EQUAL 0)
    message(FATAL_ERROR
            "Parallel ${label} consistency run failed with exit code ${parallel_result}\n"
            "stdout:\n${parallel_stdout}\n"
            "stderr:\n${parallel_stderr}")
  endif()

  set(sequential_json "${sequential_dir}/ref.tar.json")
  set(parallel_json "${parallel_dir}/ref.tar.json")

  if(NOT EXISTS "${sequential_json}")
    message(FATAL_ERROR "Expected sequential JSON output was not created: ${sequential_json}")
  endif()
  if(NOT EXISTS "${parallel_json}")
    message(FATAL_ERROR "Expected parallel JSON output was not created: ${parallel_json}")
  endif()

  file(READ "${sequential_json}" sequential_content)
  file(READ "${parallel_json}" parallel_content)

  string(REGEX REPLACE "[ \t]*\"parameters\"[^\n]*\n" "" sequential_content
                       "${sequential_content}")
  string(REGEX REPLACE "[ \t]*\"parameters\"[^\n]*\n" "" parallel_content "${parallel_content}")

  if(NOT sequential_content STREQUAL parallel_content)
    message(FATAL_ERROR
            "Parallel ${label} run output differs from sequential run.\n"
            "sequential:\n${sequential_content}\n"
            "parallel:\n${parallel_content}")
  endif()
endfunction()

run_consistency(default)
run_consistency(multi -m 1 -th 20 -rm "1,0,0.01,0.95:2,0,0.02,0.95" -tm
                "1,0,0.01,0.95:2,0,0.02,0.95")
run_consistency(multi-mixed -m 1 -th 20 -rm "1,0,0.01,0.95:15,6,3,0,0.02,0.95" -tm
                "1,0,0.01,0.95:15,6,3,0,0.02,0.95")
run_consistency(sampled -d 3)
run_consistency(sampled-multi -d 3 -m 1 -th 20 -rm "1,0,0.01,0.95:2,0,0.02,0.95" -tm
                "1,0,0.01,0.95:2,0,0.02,0.95")
