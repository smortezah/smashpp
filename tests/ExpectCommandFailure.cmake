if(NOT DEFINED EXECUTABLE OR NOT DEFINED EXPECTED_TEXT)
  message(FATAL_ERROR "EXECUTABLE and EXPECTED_TEXT must be defined.")
endif()

set(command "${EXECUTABLE}")
set(arg_index 0)
while(TRUE)
  set(arg_name "ARG_${arg_index}")
  if(NOT DEFINED ${arg_name})
    break()
  endif()

  list(APPEND command "${${arg_name}}")
  math(EXPR arg_index "${arg_index} + 1")
endwhile()

execute_process(
    COMMAND ${command}
    RESULT_VARIABLE command_result
    OUTPUT_VARIABLE command_stdout
    ERROR_VARIABLE command_stderr)

if(command_result EQUAL 0)
  message(FATAL_ERROR
          "Expected command to fail, but it exited successfully.\n"
          "stdout:\n${command_stdout}\n"
          "stderr:\n${command_stderr}")
endif()

set(command_output "${command_stdout}\n${command_stderr}")
if(NOT command_output MATCHES "${EXPECTED_TEXT}")
  message(FATAL_ERROR
          "Expected command output to match /${EXPECTED_TEXT}/.\n"
          "stdout:\n${command_stdout}\n"
          "stderr:\n${command_stderr}")
endif()
