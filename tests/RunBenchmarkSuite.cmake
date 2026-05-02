if(NOT DEFINED WORKDIR OR NOT DEFINED SMASHPP)
  message(FATAL_ERROR "WORKDIR and SMASHPP must be defined.")
endif()
if(NOT DEFINED RUNS)
  set(RUNS 3)
endif()
if(RUNS LESS 1)
  message(FATAL_ERROR "RUNS must be at least 1.")
endif()
if(NOT DEFINED BASELINE_SMASHPP)
  set(BASELINE_SMASHPP "")
endif()

file(REMOVE_RECURSE "${WORKDIR}")
file(MAKE_DIRECTORY "${WORKDIR}")

function(write_input_set label ref_repeats tar_prefix_repeats tar_suffix_repeats)
  set(input_dir "${WORKDIR}/input-${label}")
  file(MAKE_DIRECTORY "${input_dir}")

  string(REPEAT "ACGTGATTACACCGTA" "${ref_repeats}" ref_seq)
  string(REPEAT "TGCACGTAATCGACGT" "${tar_prefix_repeats}" tar_prefix)
  string(REPEAT "ACGTGATTACACCGTA" "${tar_suffix_repeats}" tar_suffix)

  file(WRITE "${input_dir}/ref" "${ref_seq}")
  file(WRITE "${input_dir}/tar" "${tar_prefix}${tar_suffix}")
endfunction()

write_input_set(small 8192 4096 4096)
write_input_set(large 524288 262144 262144)

set(benchmark_labels current)
set(benchmark_executables "${SMASHPP}")
if(NOT BASELINE_SMASHPP STREQUAL "")
  if(NOT EXISTS "${BASELINE_SMASHPP}")
    message(FATAL_ERROR "BASELINE_SMASHPP does not exist: ${BASELINE_SMASHPP}")
  endif()
  list(APPEND benchmark_labels baseline)
  list(APPEND benchmark_executables "${BASELINE_SMASHPP}")
endif()

set(summary "${WORKDIR}/summary.csv")
file(WRITE "${summary}" "executable,input,scenario,run,seconds,json_bytes\n")

function(run_benchmark_case input_label scenario)
  set(ref_file "${WORKDIR}/input-${input_label}/ref")
  set(tar_file "${WORKDIR}/input-${input_label}/tar")

  list(LENGTH benchmark_labels executable_count)
  math(EXPR executable_last "${executable_count} - 1")

  foreach(executable_index RANGE 0 ${executable_last})
    list(GET benchmark_labels "${executable_index}" executable_label)
    list(GET benchmark_executables "${executable_index}" executable_path)
    set(case_dir "${WORKDIR}/${executable_label}/${input_label}-${scenario}")
    file(MAKE_DIRECTORY "${case_dir}")
    set(json_file "${case_dir}/ref.tar.json")

    message(STATUS
            "Benchmark ${executable_label}/${input_label}/${scenario}: ${RUNS} run(s)")
    foreach(run_index RANGE 1 ${RUNS})
      file(REMOVE "${json_file}")
      execute_process(
          COMMAND "${CMAKE_COMMAND}" -E time "${executable_path}" ${ARGN} -fmt json
                  -r "${ref_file}" -t "${tar_file}"
          WORKING_DIRECTORY "${case_dir}"
          RESULT_VARIABLE benchmark_result
          OUTPUT_VARIABLE benchmark_stdout
          ERROR_VARIABLE benchmark_stderr)

      if(NOT benchmark_result EQUAL 0)
        message(FATAL_ERROR
                "Benchmark ${executable_label}/${input_label}/${scenario} run ${run_index} "
                "failed with exit code ${benchmark_result}\nstdout:\n${benchmark_stdout}\n"
                "stderr:\n${benchmark_stderr}")
      endif()
      if(NOT EXISTS "${json_file}")
        message(FATAL_ERROR
                "Benchmark ${executable_label}/${input_label}/${scenario} run ${run_index} "
                "did not create ${json_file}.")
      endif()

      set(benchmark_output "${benchmark_stdout}\n${benchmark_stderr}")
      string(REGEX MATCH "Elapsed time \\(seconds\\): ([0-9.]+)" elapsed_match
                         "${benchmark_output}")
      if(CMAKE_MATCH_1)
        set(elapsed "${CMAKE_MATCH_1}")
      else()
        set(elapsed "unknown")
      endif()

      file(SIZE "${json_file}" json_size)
      file(APPEND "${summary}"
           "${executable_label},${input_label},${scenario},${run_index},${elapsed},${json_size}\n")
    endforeach()
  endforeach()
endfunction()

run_benchmark_case(small default -n 1 -nr -m 1 -th 20)
run_benchmark_case(small serial-multi -n 1 -nr -m 1 -th 20 -rm
                   "6,0,0.01,0.95:8,0,0.02,0.95" -tm
                   "6,0,0.01,0.95:8,0,0.02,0.95")
run_benchmark_case(small parallel -n 4 -nr -m 1 -th 20)
run_benchmark_case(small nonrect-filter -n 4 -nr -f 251 -ft hann -m 1 -th 20)
run_benchmark_case(small sampled-filter -n 4 -nr -d 4 -f 25 -m 1 -th 20)
run_benchmark_case(small sampled-multi -n 4 -nr -d 4 -f 25 -m 1 -th 20 -rm
                   "6,0,0.01,0.95:8,0,0.02,0.95" -tm
                   "6,0,0.01,0.95:8,0,0.02,0.95")
run_benchmark_case(small sampled-multi-approx -n 4 -nr -d 4 -f 25 -m 1 -th 20
                   --approx-sampled-models -rm "6,0,0.01,0.95:8,0,0.02,0.95" -tm
                   "6,0,0.01,0.95:8,0,0.02,0.95")
run_benchmark_case(small sketch -n 2 -nr -m 1 -th 20 -rm "20,10,5,0,0.01,0.95" -tm
                   "20,10,5,0,0.01,0.95")

run_benchmark_case(large default -n 1 -nr -m 1 -th 20)
run_benchmark_case(large parallel -n 4 -nr -m 1 -th 20)
run_benchmark_case(large serial-multi -n 1 -nr -m 1 -th 20 -rm
                   "6,0,0.01,0.95:8,0,0.02,0.95" -tm
                   "6,0,0.01,0.95:8,0,0.02,0.95")
run_benchmark_case(large sampled-multi -n 4 -nr -d 4 -f 25 -m 1 -th 20 -rm
                   "6,0,0.01,0.95:8,0,0.02,0.95" -tm
                   "6,0,0.01,0.95:8,0,0.02,0.95")
run_benchmark_case(large sampled-multi-approx -n 4 -nr -d 4 -f 25 -m 1 -th 20
                   --approx-sampled-models -rm "6,0,0.01,0.95:8,0,0.02,0.95" -tm
                   "6,0,0.01,0.95:8,0,0.02,0.95")
run_benchmark_case(large nonrect-filter -n 4 -nr -f 251 -ft hann -m 1 -th 20)

message(STATUS "Benchmark results written to ${summary}")
