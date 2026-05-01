if(NOT DEFINED WORKDIR OR NOT DEFINED SMASHPP)
  message(FATAL_ERROR "WORKDIR and SMASHPP must be defined.")
endif()
if(NOT DEFINED RUNS)
  set(RUNS 3)
endif()
if(RUNS LESS 1)
  message(FATAL_ERROR "RUNS must be at least 1.")
endif()

file(REMOVE_RECURSE "${WORKDIR}")
file(MAKE_DIRECTORY "${WORKDIR}/input")

set(ref_file "${WORKDIR}/input/ref")
set(tar_file "${WORKDIR}/input/tar")

string(REPEAT "ACGTGATTACACCGTA" 8192 ref_seq)
string(REPEAT "TGCACGTAATCGACGT" 4096 tar_prefix)
string(REPEAT "ACGTGATTACACCGTA" 4096 tar_suffix)
file(WRITE "${ref_file}" "${ref_seq}")
file(WRITE "${tar_file}" "${tar_prefix}${tar_suffix}")

set(summary "${WORKDIR}/summary.csv")
file(WRITE "${summary}" "scenario,run,seconds,json_bytes\n")

function(run_benchmark_case label)
  set(case_dir "${WORKDIR}/${label}")
  file(MAKE_DIRECTORY "${case_dir}")
  set(json_file "${case_dir}/ref.tar.json")

  message(STATUS "Benchmark ${label}: ${RUNS} run(s)")
  foreach(run_index RANGE 1 ${RUNS})
    file(REMOVE "${json_file}")
    execute_process(
        COMMAND "${CMAKE_COMMAND}" -E time "${SMASHPP}" ${ARGN} -fmt json -r "${ref_file}"
                -t "${tar_file}"
        WORKING_DIRECTORY "${case_dir}"
        RESULT_VARIABLE benchmark_result
        OUTPUT_VARIABLE benchmark_stdout
        ERROR_VARIABLE benchmark_stderr)

    if(NOT benchmark_result EQUAL 0)
      message(FATAL_ERROR
              "Benchmark ${label} run ${run_index} failed with exit code ${benchmark_result}\n"
              "stdout:\n${benchmark_stdout}\n"
              "stderr:\n${benchmark_stderr}")
    endif()
    if(NOT EXISTS "${json_file}")
      message(FATAL_ERROR
              "Benchmark ${label} run ${run_index} did not create ${json_file}.")
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
    file(APPEND "${summary}" "${label},${run_index},${elapsed},${json_size}\n")
  endforeach()
endfunction()

run_benchmark_case(default -n 1 -nr -m 1 -th 20)
run_benchmark_case(serial-multi -n 1 -nr -m 1 -th 20 -rm "6,0,0.01,0.95:8,0,0.02,0.95" -tm
                   "6,0,0.01,0.95:8,0,0.02,0.95")
run_benchmark_case(parallel -n 4 -nr -m 1 -th 20)
run_benchmark_case(nonrect-filter -n 4 -nr -f 251 -ft hann -m 1 -th 20)
run_benchmark_case(sampled-filter -n 4 -nr -d 4 -f 25 -m 1 -th 20)
run_benchmark_case(sampled-multi -n 4 -nr -d 4 -f 25 -m 1 -th 20 -rm
                   "6,0,0.01,0.95:8,0,0.02,0.95" -tm
                   "6,0,0.01,0.95:8,0,0.02,0.95")
run_benchmark_case(sketch -n 2 -nr -m 1 -th 20 -rm "20,10,5,0,0.01,0.95" -tm
                   "20,10,5,0,0.01,0.95")

message(STATUS "Benchmark results written to ${summary}")
