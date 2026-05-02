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
if(NOT DEFINED SMALL_BYTES)
  set(SMALL_BYTES 131072)
endif()
if(NOT DEFINED LARGE_BYTES)
  set(LARGE_BYTES 268435456)
endif()

file(REMOVE_RECURSE "${WORKDIR}")
file(MAKE_DIRECTORY "${WORKDIR}")

function(size_to_repeats label size_bytes out_ref_repeats out_tar_prefix_repeats
         out_tar_suffix_repeats)
  if(size_bytes LESS 16)
    message(FATAL_ERROR "${label} benchmark size must be at least 16 bytes.")
  endif()

  math(EXPR remainder "${size_bytes} % 16")
  if(NOT remainder EQUAL 0)
    message(FATAL_ERROR
            "${label} benchmark size must be divisible by 16 bytes.")
  endif()

  math(EXPR ref_repeats "${size_bytes} / 16")
  math(EXPR tar_prefix_repeats "${ref_repeats} / 2")
  math(EXPR tar_suffix_repeats "${ref_repeats} - ${tar_prefix_repeats}")

  set(${out_ref_repeats} "${ref_repeats}" PARENT_SCOPE)
  set(${out_tar_prefix_repeats} "${tar_prefix_repeats}" PARENT_SCOPE)
  set(${out_tar_suffix_repeats} "${tar_suffix_repeats}" PARENT_SCOPE)
endfunction()

function(write_repeated path unit repeats)
  set(remaining "${repeats}")
  set(chunk_repeats 65536)

  while(remaining GREATER 0)
    if(remaining GREATER chunk_repeats)
      set(current_repeats "${chunk_repeats}")
    else()
      set(current_repeats "${remaining}")
    endif()

    string(REPEAT "${unit}" "${current_repeats}" chunk)
    file(APPEND "${path}" "${chunk}")
    math(EXPR remaining "${remaining} - ${current_repeats}")
  endwhile()
endfunction()

function(write_input_set label size_bytes)
  set(input_dir "${WORKDIR}/input-${label}")
  file(MAKE_DIRECTORY "${input_dir}")

  size_to_repeats("${label}" "${size_bytes}" ref_repeats tar_prefix_repeats
                  tar_suffix_repeats)
  message(STATUS "Writing ${label} benchmark input files: ${size_bytes} bytes each")

  file(WRITE "${input_dir}/ref" "")
  write_repeated("${input_dir}/ref" "ACGTGATTACACCGTA" "${ref_repeats}")
  file(WRITE "${input_dir}/tar" "")
  write_repeated("${input_dir}/tar" "TGCACGTAATCGACGT" "${tar_prefix_repeats}")
  write_repeated("${input_dir}/tar" "ACGTGATTACACCGTA" "${tar_suffix_repeats}")
endfunction()

function(seconds_to_micros seconds out_micros)
  if(NOT seconds MATCHES "^([0-9]+)(\\.([0-9]+))?$")
    message(FATAL_ERROR "Cannot parse benchmark elapsed time: ${seconds}")
  endif()

  set(whole "${CMAKE_MATCH_1}")
  set(fraction "${CMAKE_MATCH_3}")
  string(SUBSTRING "${fraction}000000" 0 6 fraction_padded)
  math(EXPR micros "${whole} * 1000000 + ${fraction_padded}")
  set(${out_micros} "${micros}" PARENT_SCOPE)
endfunction()

function(micros_to_seconds micros out_seconds)
  math(EXPR whole "${micros} / 1000000")
  math(EXPR fraction "${micros} % 1000000")
  string(LENGTH "000000${fraction}" fraction_len)
  math(EXPR fraction_start "${fraction_len} - 6")
  string(SUBSTRING "000000${fraction}" "${fraction_start}" 6 fraction_padded)
  set(${out_seconds} "${whole}.${fraction_padded}" PARENT_SCOPE)
endfunction()

function(speedup_ratio baseline_micros current_micros out_ratio)
  if(current_micros EQUAL 0)
    set(${out_ratio} "inf" PARENT_SCOPE)
    return()
  endif()

  math(EXPR scaled "(${baseline_micros} * 100 + ${current_micros} / 2) / ${current_micros}")
  math(EXPR whole "${scaled} / 100")
  math(EXPR fraction "${scaled} % 100")
  string(LENGTH "00${fraction}" fraction_len)
  math(EXPR fraction_start "${fraction_len} - 2")
  string(SUBSTRING "00${fraction}" "${fraction_start}" 2 fraction_padded)
  set(${out_ratio} "${whole}.${fraction_padded}" PARENT_SCOPE)
endfunction()

function(median_micros out_median)
  set(sorted)

  foreach(value ${ARGN})
    set(inserted FALSE)
    list(LENGTH sorted sorted_len)

    if(sorted_len EQUAL 0)
      list(APPEND sorted "${value}")
    else()
      math(EXPR sorted_last "${sorted_len} - 1")
      foreach(idx RANGE 0 ${sorted_last})
        list(GET sorted "${idx}" existing)
        if(value LESS existing)
          list(INSERT sorted "${idx}" "${value}")
          set(inserted TRUE)
          break()
        endif()
      endforeach()

      if(NOT inserted)
        list(APPEND sorted "${value}")
      endif()
    endif()
  endforeach()

  list(LENGTH sorted sorted_len)
  if(sorted_len EQUAL 0)
    message(FATAL_ERROR "Cannot calculate median of an empty benchmark result list.")
  endif()

  math(EXPR middle "${sorted_len} / 2")
  math(EXPR odd "${sorted_len} % 2")
  if(odd)
    list(GET sorted "${middle}" median)
  else()
    math(EXPR lower_idx "${middle} - 1")
    list(GET sorted "${lower_idx}" lower)
    list(GET sorted "${middle}" upper)
    math(EXPR median "(${lower} + ${upper}) / 2")
  endif()

  set(${out_median} "${median}" PARENT_SCOPE)
endfunction()

write_input_set(small "${SMALL_BYTES}")
write_input_set(large "${LARGE_BYTES}")

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
set(comparison "${WORKDIR}/comparison.csv")
if(NOT BASELINE_SMASHPP STREQUAL "")
  file(WRITE "${comparison}" "input,scenario,current_median,baseline_median,speedup\n")
endif()

function(run_benchmark_case input_label scenario)
  set(ref_file "${WORKDIR}/input-${input_label}/ref")
  set(tar_file "${WORKDIR}/input-${input_label}/tar")

  list(LENGTH benchmark_labels executable_count)
  math(EXPR executable_last "${executable_count} - 1")

  foreach(executable_index RANGE 0 ${executable_last})
    list(GET benchmark_labels "${executable_index}" executable_label)
    list(GET benchmark_executables "${executable_index}" executable_path)
    set(timings_${executable_label})
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
      seconds_to_micros("${elapsed}" elapsed_micros)
      list(APPEND timings_${executable_label} "${elapsed_micros}")

      file(SIZE "${json_file}" json_size)
      file(APPEND "${summary}"
           "${executable_label},${input_label},${scenario},${run_index},${elapsed},${json_size}\n")
    endforeach()
  endforeach()

  if(NOT BASELINE_SMASHPP STREQUAL "")
    median_micros(current_median_micros ${timings_current})
    median_micros(baseline_median_micros ${timings_baseline})
    micros_to_seconds("${current_median_micros}" current_median)
    micros_to_seconds("${baseline_median_micros}" baseline_median)
    speedup_ratio("${baseline_median_micros}" "${current_median_micros}" speedup)
    file(APPEND "${comparison}"
         "${input_label},${scenario},${current_median},${baseline_median},${speedup}\n")
  endif()
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
if(NOT BASELINE_SMASHPP STREQUAL "")
  message(STATUS "Benchmark comparison written to ${comparison}")
endif()
