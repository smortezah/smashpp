if(NOT DEFINED WORKDIR OR NOT DEFINED SMASHPP)
  message(FATAL_ERROR "WORKDIR and SMASHPP must be defined.")
endif()

file(REMOVE_RECURSE "${WORKDIR}")
file(MAKE_DIRECTORY "${WORKDIR}/sampled-filter")
file(MAKE_DIRECTORY "${WORKDIR}/multi-sampled-filter")
file(MAKE_DIRECTORY "${WORKDIR}/non-rect-filter")
file(MAKE_DIRECTORY "${WORKDIR}/sampled-segment")
file(MAKE_DIRECTORY "${WORKDIR}/large-step")
file(MAKE_DIRECTORY "${WORKDIR}/exact-sampled-models")
file(MAKE_DIRECTORY "${WORKDIR}/approx-sampled-models")

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

function(assert_file_content file expected)
  if(NOT EXISTS "${file}")
    message(FATAL_ERROR "Expected file was not created: ${file}")
  endif()

  file(READ "${file}" content)
  if(NOT content STREQUAL expected)
    message(FATAL_ERROR "Expected ${file} content to match the non-rectangular filter baseline.")
  endif()
endfunction()

function(assert_files_differ lhs rhs)
  if(NOT EXISTS "${lhs}")
    message(FATAL_ERROR "Expected file was not created: ${lhs}")
  endif()
  if(NOT EXISTS "${rhs}")
    message(FATAL_ERROR "Expected file was not created: ${rhs}")
  endif()

  file(READ "${lhs}" lhs_content)
  file(READ "${rhs}" rhs_content)
  if(lhs_content STREQUAL rhs_content)
    message(FATAL_ERROR "Expected ${lhs} and ${rhs} to differ.")
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

write_inputs("${WORKDIR}/multi-sampled-filter")
execute_process(
    COMMAND "${SMASHPP}" -n 1 -sp -sf -nr -d 3 -f 1 -th 20 -m 1
            -rm "1,0,0.01,0.95:2,0,0.02,0.95"
            -tm "1,0,0.01,0.95:2,0,0.02,0.95"
            -r ref -t tar
    WORKING_DIRECTORY "${WORKDIR}/multi-sampled-filter"
    RESULT_VARIABLE multi_sampled_filter_result
    OUTPUT_VARIABLE multi_sampled_filter_stdout
    ERROR_VARIABLE multi_sampled_filter_stderr)

if(NOT multi_sampled_filter_result EQUAL 0)
  message(FATAL_ERROR
          "Multi-model sampled filter run failed with exit code ${multi_sampled_filter_result}\n"
          "stdout:\n${multi_sampled_filter_stdout}\n"
          "stderr:\n${multi_sampled_filter_stderr}")
endif()

assert_line_count("${WORKDIR}/multi-sampled-filter/0.ref.tar.prf" 4)
assert_line_count("${WORKDIR}/multi-sampled-filter/0.ref.tar.fil" 4)
assert_line_count("${WORKDIR}/multi-sampled-filter/1.ref.tar.prf" 4)
assert_line_count("${WORKDIR}/multi-sampled-filter/1.ref.tar.fil" 4)

set(non_rect_seq "ACGTACGTACGTACGTACGTACGTACGT")
file(WRITE "${WORKDIR}/non-rect-filter/ref" "${non_rect_seq}")
file(WRITE "${WORKDIR}/non-rect-filter/tar" "${non_rect_seq}")
execute_process(
    COMMAND "${SMASHPP}" -n 1 -sp -sf -nr -dp -f 5 -ft hann -m 1 -th 20
            -rm "1,0,0.01,0.95" -tm "1,0,0.01,0.95"
            -r ref -t tar
    WORKING_DIRECTORY "${WORKDIR}/non-rect-filter"
    RESULT_VARIABLE non_rect_filter_result
    OUTPUT_VARIABLE non_rect_filter_stdout
    ERROR_VARIABLE non_rect_filter_stderr)

if(NOT non_rect_filter_result EQUAL 0)
  message(FATAL_ERROR
          "Non-rectangular filter baseline run failed with exit code ${non_rect_filter_result}\n"
          "stdout:\n${non_rect_filter_stdout}\n"
          "stderr:\n${non_rect_filter_stderr}")
endif()

set(expected_regular_filter [=[2.11
1.43
0.488
0.0328
0.033
0.0757
0.0327
0.0063
0.033
0.0759
0.0328
0.00653
0.033
0.0757
0.0756
0.033
0.0759
0.0757
0.0327
0.0063
0.0759
0.0757
0.0327
0.0063
0.033
0.0757
0.0327
0.282
1
]=])
set(expected_inverted_filter [=[1.08
0.362
0.0368
0.0368
0.0864
0.0368
0.00616
0.0368
0.0864
0.0864
0.0368
0.0864
0.0864
0.0368
0.00616
0.0864
0.0864
0.0368
0.00616
0.0368
0.0864
0.0368
0.00616
0.0368
0.0864
0.0368
0.00616
0.282
1
]=])
assert_file_content("${WORKDIR}/non-rect-filter/0.ref.tar.fil" "${expected_regular_filter}")
assert_file_content("${WORKDIR}/non-rect-filter/1.ref.tar.fil" "${expected_inverted_filter}")

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

assert_file_size("${WORKDIR}/sampled-segment/0.ref.tar.s0" 10)
assert_file_size("${WORKDIR}/sampled-segment/1.ref.tar.s0" 10)

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

set(varied_ref_unit "ACGTTGCATAGGCTAACGATTCGACCGTAGCTTACGGATCGTACCTAGGTTACGATCGCATTA")
set(varied_tar_unit "TGCACGTAACCTGATCGTAGGCTTACGATTCGGCATAGCTACCGTTAAGCTCGATGGCATACG")
string(REPEAT "${varied_ref_unit}" 128 varied_ref)
string(REPEAT "${varied_tar_unit}" 128 varied_tar)
file(WRITE "${WORKDIR}/exact-sampled-models/ref" "${varied_ref}")
file(WRITE "${WORKDIR}/exact-sampled-models/tar" "${varied_tar}")
file(WRITE "${WORKDIR}/approx-sampled-models/ref" "${varied_ref}")
file(WRITE "${WORKDIR}/approx-sampled-models/tar" "${varied_tar}")
execute_process(
    COMMAND "${SMASHPP}" -n 1 -sp -sf -nr -d 7 -f 5 -th 20 -m 1
            -rm "3,0,0.01,0.95:6,0,0.02,0.95"
            -tm "3,0,0.01,0.95:6,0,0.02,0.95"
            -r ref -t tar
    WORKING_DIRECTORY "${WORKDIR}/exact-sampled-models"
    RESULT_VARIABLE exact_sampled_models_result
    OUTPUT_VARIABLE exact_sampled_models_stdout
    ERROR_VARIABLE exact_sampled_models_stderr)

if(NOT exact_sampled_models_result EQUAL 0)
  message(FATAL_ERROR
          "Exact sampled multi-model run failed with exit code ${exact_sampled_models_result}\n"
          "stdout:\n${exact_sampled_models_stdout}\n"
          "stderr:\n${exact_sampled_models_stderr}")
endif()

execute_process(
    COMMAND "${SMASHPP}" -n 1 --approx-sampled-models -sp -sf -nr -d 7 -f 5 -th 20 -m 1
            -rm "3,0,0.01,0.95:6,0,0.02,0.95"
            -tm "3,0,0.01,0.95:6,0,0.02,0.95"
            -r ref -t tar
    WORKING_DIRECTORY "${WORKDIR}/approx-sampled-models"
    RESULT_VARIABLE approx_sampled_models_result
    OUTPUT_VARIABLE approx_sampled_models_stdout
    ERROR_VARIABLE approx_sampled_models_stderr)

if(NOT approx_sampled_models_result EQUAL 0)
  message(FATAL_ERROR
          "Approximate sampled multi-model run failed with exit code ${approx_sampled_models_result}\n"
          "stdout:\n${approx_sampled_models_stdout}\n"
          "stderr:\n${approx_sampled_models_stderr}")
endif()

assert_line_count("${WORKDIR}/exact-sampled-models/0.ref.tar.prf" 1152)
assert_line_count("${WORKDIR}/exact-sampled-models/0.ref.tar.fil" 1153)
assert_line_count("${WORKDIR}/exact-sampled-models/1.ref.tar.prf" 1152)
assert_line_count("${WORKDIR}/exact-sampled-models/1.ref.tar.fil" 1153)
assert_line_count("${WORKDIR}/approx-sampled-models/0.ref.tar.prf" 1152)
assert_line_count("${WORKDIR}/approx-sampled-models/0.ref.tar.fil" 1153)
assert_line_count("${WORKDIR}/approx-sampled-models/1.ref.tar.prf" 1152)
assert_line_count("${WORKDIR}/approx-sampled-models/1.ref.tar.fil" 1153)
assert_files_differ("${WORKDIR}/exact-sampled-models/0.ref.tar.prf"
                    "${WORKDIR}/approx-sampled-models/0.ref.tar.prf")
assert_files_differ("${WORKDIR}/exact-sampled-models/1.ref.tar.prf"
                    "${WORKDIR}/approx-sampled-models/1.ref.tar.prf")
