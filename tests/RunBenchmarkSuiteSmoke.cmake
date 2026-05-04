if(NOT DEFINED WORKDIR OR NOT DEFINED SMASHPP OR NOT DEFINED BENCHMARK_SCRIPT)
  message(FATAL_ERROR "WORKDIR, SMASHPP, and BENCHMARK_SCRIPT must be defined.")
endif()

file(REMOVE_RECURSE "${WORKDIR}")
file(MAKE_DIRECTORY "${WORKDIR}")

set(benchmark_workdir "${WORKDIR}/benchmarks")
execute_process(
    COMMAND "${CMAKE_COMMAND}"
            "-DWORKDIR=${benchmark_workdir}"
            "-DSMASHPP=${SMASHPP}"
            "-DBASELINE_SMASHPP=${SMASHPP}"
            -DRUNS=2
            -DSMALL_BYTES=16384
            -DLARGE_BYTES=32768
            -P
            "${BENCHMARK_SCRIPT}"
    RESULT_VARIABLE benchmark_result
    OUTPUT_VARIABLE benchmark_stdout
    ERROR_VARIABLE benchmark_stderr)

if(NOT benchmark_result EQUAL 0)
  message(FATAL_ERROR
          "Benchmark suite smoke test failed with exit code ${benchmark_result}\n"
          "stdout:\n${benchmark_stdout}\n"
          "stderr:\n${benchmark_stderr}")
endif()

function(assert_line_count file expected)
  if(NOT EXISTS "${file}")
    message(FATAL_ERROR "Expected benchmark file was not created: ${file}")
  endif()

  file(READ "${file}" content)
  string(REGEX MATCHALL "\n" lines "${content}")
  list(LENGTH lines count)
  if(NOT count EQUAL expected)
    message(FATAL_ERROR "Expected ${file} to have ${expected} lines, got ${count}.")
  endif()
endfunction()

set(summary "${benchmark_workdir}/summary.csv")
set(comparison "${benchmark_workdir}/comparison.csv")

assert_line_count("${summary}" 57)
assert_line_count("${comparison}" 15)

file(READ "${summary}" summary_content)
if(NOT summary_content MATCHES "^executable,input,scenario,run,seconds,json_bytes\n")
  message(FATAL_ERROR "Benchmark summary header is missing or invalid.")
endif()
if(NOT summary_content MATCHES "current,large,sampled-multi-approx,2,[0-9.]+,[0-9]+")
  message(FATAL_ERROR "Benchmark summary is missing an expected scenario row.")
endif()

file(READ "${comparison}" comparison_content)
if(NOT comparison_content MATCHES "^input,scenario,current_median,baseline_median,speedup\n")
  message(FATAL_ERROR "Benchmark comparison header is missing or invalid.")
endif()
if(NOT comparison_content MATCHES "large,sampled-multi-approx,[0-9.]+,[0-9.]+,[0-9.]+")
  message(FATAL_ERROR "Benchmark comparison is missing an expected scenario row.")
endif()

