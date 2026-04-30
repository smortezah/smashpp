if(NOT DEFINED WORKDIR OR NOT DEFINED SMASHPP)
  message(FATAL_ERROR "WORKDIR and SMASHPP must be defined.")
endif()

file(REMOVE_RECURSE "${WORKDIR}")
file(MAKE_DIRECTORY "${WORKDIR}")

set(ref_fasta "${WORKDIR}/ref.fa")
set(tar_fastq "${WORKDIR}/tar.fq")

string(REPEAT "acgtnryk" 130 seq_input)
string(REPEAT "ACGTNNNN" 130 expected_seq)
string(REPEAT "I" 1040 quality)

file(WRITE "${ref_fasta}" ">ref mixed case and ambiguous bases\r\n${seq_input}\r\n")
file(WRITE "${tar_fastq}" "@tar\r\n${seq_input}\r\n+\r\n${quality}\r\n")

execute_process(
    COMMAND "${SMASHPP}" -r "${ref_fasta}" -t "${tar_fastq}" -sb -nr -m 1 -th 20 -fmt json
    WORKING_DIRECTORY "${WORKDIR}"
    RESULT_VARIABLE smashpp_result
    OUTPUT_VARIABLE smashpp_stdout
    ERROR_VARIABLE smashpp_stderr)

if(NOT smashpp_result EQUAL 0)
  message(FATAL_ERROR
          "Smash++ input normalization run failed with exit code ${smashpp_result}\n"
          "stdout:\n${smashpp_stdout}\n"
          "stderr:\n${smashpp_stderr}")
endif()

set(ref_seq "${WORKDIR}/ref.seq")
set(tar_seq "${WORKDIR}/tar.seq")
if(NOT EXISTS "${ref_seq}")
  message(FATAL_ERROR "Expected normalized reference sequence was not created: ${ref_seq}")
endif()
if(NOT EXISTS "${tar_seq}")
  message(FATAL_ERROR "Expected normalized target sequence was not created: ${tar_seq}")
endif()

file(READ "${ref_seq}" ref_content)
file(READ "${tar_seq}" tar_content)
if(NOT ref_content STREQUAL expected_seq)
  message(FATAL_ERROR "Reference normalization produced unexpected content.")
endif()
if(NOT tar_content STREQUAL expected_seq)
  message(FATAL_ERROR "Target normalization produced unexpected content.")
endif()

set(json_file "${WORKDIR}/ref.fa.tar.fq.json")
if(NOT EXISTS "${json_file}")
  message(FATAL_ERROR "Expected JSON output was not created: ${json_file}")
endif()

file(READ "${json_file}" json_content)
if(NOT json_content MATCHES "\"reference\"[ \t\r\n]*:[ \t\r\n]*\"ref.fa\"")
  message(FATAL_ERROR "JSON output did not preserve the original reference name.")
endif()
if(NOT json_content MATCHES "\"target\"[ \t\r\n]*:[ \t\r\n]*\"tar.fq\"")
  message(FATAL_ERROR "JSON output did not preserve the original target name.")
endif()
if(NOT json_content MATCHES "\"reference_size\"[ \t\r\n]*:[ \t\r\n]*\"1040\"")
  message(FATAL_ERROR "JSON output has an unexpected normalized reference size.")
endif()
if(NOT json_content MATCHES "\"target_size\"[ \t\r\n]*:[ \t\r\n]*\"1040\"")
  message(FATAL_ERROR "JSON output has an unexpected normalized target size.")
endif()

set(direct_ref "${WORKDIR}/direct_ref.seq")
set(direct_tar "${WORKDIR}/direct_tar.seq")
file(WRITE "${direct_ref}" "${seq_input}\r\n")
file(WRITE "${direct_tar}" "${seq_input}\r\n")

execute_process(
    COMMAND "${SMASHPP}" -r "${direct_ref}" -t "${direct_tar}" -nr -m 1 -th 20 -fmt json
    WORKING_DIRECTORY "${WORKDIR}"
    RESULT_VARIABLE direct_result
    OUTPUT_VARIABLE direct_stdout
    ERROR_VARIABLE direct_stderr)

if(NOT direct_result EQUAL 0)
  message(FATAL_ERROR
          "Smash++ direct sequence normalization run failed with exit code ${direct_result}\n"
          "stdout:\n${direct_stdout}\n"
          "stderr:\n${direct_stderr}")
endif()
