if(NOT DEFINED WORKDIR OR NOT DEFINED SMASHPP)
  message(FATAL_ERROR "WORKDIR and SMASHPP must be defined.")
endif()

file(REMOVE_RECURSE "${WORKDIR}")
file(MAKE_DIRECTORY "${WORKDIR}")

set(ref_seq "${WORKDIR}/ref.seq")
set(tar_seq "${WORKDIR}/tar.seq")

file(WRITE "${ref_seq}" "ACGTZACGT")
file(WRITE "${tar_seq}" "ACGTACGT")

execute_process(
    COMMAND "${SMASHPP}" -r "${ref_seq}" -t "${tar_seq}" -nr -m 1 -th 20
    WORKING_DIRECTORY "${WORKDIR}"
    RESULT_VARIABLE smashpp_result
    OUTPUT_VARIABLE smashpp_stdout
    ERROR_VARIABLE smashpp_stderr)

if(smashpp_result EQUAL 0)
  message(FATAL_ERROR
          "Expected invalid base input to fail, but it exited successfully.\n"
          "stdout:\n${smashpp_stdout}\n"
          "stderr:\n${smashpp_stderr}")
endif()

set(command_output "${smashpp_stdout}\n${smashpp_stderr}")
if(NOT command_output MATCHES "invalid base")
  message(FATAL_ERROR
          "Expected invalid base error.\n"
          "stdout:\n${smashpp_stdout}\n"
          "stderr:\n${smashpp_stderr}")
endif()

if(NOT command_output MATCHES "invalid base \"Z\"")
  message(FATAL_ERROR
          "Expected invalid base error to include the invalid byte.\n"
          "stdout:\n${smashpp_stdout}\n"
          "stderr:\n${smashpp_stderr}")
endif()

if(NOT command_output MATCHES "ref\\.seq")
  message(FATAL_ERROR
          "Expected invalid base error to include the source file.\n"
          "stdout:\n${smashpp_stdout}\n"
          "stderr:\n${smashpp_stderr}")
endif()
