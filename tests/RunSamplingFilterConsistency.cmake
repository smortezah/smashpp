if(NOT DEFINED WORKDIR OR NOT DEFINED SMASHPP)
  message(FATAL_ERROR "WORKDIR and SMASHPP must be defined.")
endif()

file(REMOVE_RECURSE "${WORKDIR}")
file(MAKE_DIRECTORY "${WORKDIR}/sampled-filter")
file(MAKE_DIRECTORY "${WORKDIR}/sampled-segment")
file(MAKE_DIRECTORY "${WORKDIR}/large-step")

set(seq "AAAAAAAAAAA")

function(write_inputs workdir)
  file(WRITE "${workdir}/ref" "${seq}")
  file(WRITE "${workdir}/tar" "${seq}")
endfunction()

function(assert_line_count file expected)
  if(NOT EXISTS "${file}")
    message(FATAL_ERROR "Expected file was not created: ${file}")
  endif()

  file(READ "${file}" content)
  string(REGEX MATCHALL "\n" lines "${content}")
  list(LENGTH lines count)
  if(NOT count EQUAL expected)
    message(FATAL_ERROR "Expected ${file} to have ${expected} lines, got ${count}.")
  endif()
endfunction()

function(assert_file_size file expected)
  if(NOT EXISTS "${file}")
    message(FATAL_ERROR "Expected file was not created: ${file}")
  endif()

  file(SIZE "${file}" size)
  if(NOT size EQUAL expected)
    message(FATAL_ERROR "Expected ${file} to have ${expected} bytes, got ${size}.")
  endif()
endfunction()

write_inputs("${WORKDIR}/sampled-filter")
execute_process(
    COMMAND "${SMASHPP}" -n 1 -sp -sf -nr -d 3 -f 1 -th 20 -m 1
            -rm "1,0,0.01,0.95" -tm "1,0,0.01,0.95"
            -r ref -t tar
    WORKING_DIRECTORY "${WORKDIR}/sampled-filter"
    RESULT_VARIABLE sampled_filter_result
    OUTPUT_VARIABLE sampled_filter_stdout
    ERROR_VARIABLE sampled_filter_stderr)

if(NOT sampled_filter_result EQUAL 0)
  message(FATAL_ERROR
          "Sampled filter consistency run failed with exit code ${sampled_filter_result}\n"
          "stdout:\n${sampled_filter_stdout}\n"
          "stderr:\n${sampled_filter_stderr}")
endif()

assert_line_count("${WORKDIR}/sampled-filter/0.ref.tar.prf" 4)
assert_line_count("${WORKDIR}/sampled-filter/0.ref.tar.fil" 4)
assert_line_count("${WORKDIR}/sampled-filter/1.ref.tar.prf" 4)
assert_line_count("${WORKDIR}/sampled-filter/1.ref.tar.fil" 4)

write_inputs("${WORKDIR}/sampled-segment")
execute_process(
    COMMAND "${SMASHPP}" -n 1 -ss -nr -dp -d 3 -f 1 -th 20 -m 1
            -rm "1,0,0.01,0.95" -tm "1,0,0.01,0.95"
            -r ref -t tar
    WORKING_DIRECTORY "${WORKDIR}/sampled-segment"
    RESULT_VARIABLE sampled_segment_result
    OUTPUT_VARIABLE sampled_segment_stdout
    ERROR_VARIABLE sampled_segment_stderr)

if(NOT sampled_segment_result EQUAL 0)
  message(FATAL_ERROR
          "Sampled segment consistency run failed with exit code ${sampled_segment_result}\n"
          "stdout:\n${sampled_segment_stdout}\n"
          "stderr:\n${sampled_segment_stderr}")
endif()

assert_file_size("${WORKDIR}/sampled-segment/0.ref.tar.s0" 11)
assert_file_size("${WORKDIR}/sampled-segment/1.ref.tar.s0" 11)

write_inputs("${WORKDIR}/large-step")
execute_process(
    COMMAND "${SMASHPP}" -n 1 -sp -sf -ss -nr -dp -d 20 -f 100 -th 20 -m 1
            -rm "1,0,0.01,0.95" -tm "1,0,0.01,0.95"
            -r ref -t tar
    WORKING_DIRECTORY "${WORKDIR}/large-step"
    RESULT_VARIABLE large_step_result
    OUTPUT_VARIABLE large_step_stdout
    ERROR_VARIABLE large_step_stderr)

if(NOT large_step_result EQUAL 0)
  message(FATAL_ERROR
          "Large sampling-step consistency run failed with exit code ${large_step_result}\n"
          "stdout:\n${large_step_stdout}\n"
          "stderr:\n${large_step_stderr}")
endif()

assert_line_count("${WORKDIR}/large-step/0.ref.tar.prf" 1)
assert_line_count("${WORKDIR}/large-step/0.ref.tar.fil" 1)
assert_file_size("${WORKDIR}/large-step/0.ref.tar.s0" 11)
