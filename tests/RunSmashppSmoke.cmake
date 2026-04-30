if(NOT DEFINED WORKDIR OR NOT DEFINED SMASHPP OR NOT DEFINED REF OR NOT DEFINED TAR)
  message(FATAL_ERROR "WORKDIR, SMASHPP, REF, and TAR must be defined.")
endif()

file(REMOVE_RECURSE "${WORKDIR}")
file(MAKE_DIRECTORY "${WORKDIR}")

execute_process(
    COMMAND "${SMASHPP}" -fmt json -r "${REF}" -t "${TAR}"
    WORKING_DIRECTORY "${WORKDIR}"
    RESULT_VARIABLE smashpp_result
    OUTPUT_VARIABLE smashpp_stdout
    ERROR_VARIABLE smashpp_stderr)

if(NOT smashpp_result EQUAL 0)
  message(FATAL_ERROR
          "Smash++ smoke run failed with exit code ${smashpp_result}\n"
          "stdout:\n${smashpp_stdout}\n"
          "stderr:\n${smashpp_stderr}")
endif()

set(json_file "${WORKDIR}/ref.tar.json")
if(NOT EXISTS "${json_file}")
  message(FATAL_ERROR "Expected JSON output was not created: ${json_file}")
endif()

file(READ "${json_file}" json_content)
if(NOT json_content MATCHES "\"watermark\"[ \t\r\n]*:[ \t\r\n]*\"##SMASH\\+\\+\"")
  message(FATAL_ERROR "JSON output is missing the Smash++ watermark.")
endif()
if(NOT json_content MATCHES "\"target_size\"[ \t\r\n]*:[ \t\r\n]*\"1000\"")
  message(FATAL_ERROR "JSON output has an unexpected target size.")
endif()
if(NOT json_content MATCHES "\"positions\"[ \t\r\n]*:[ \t\r\n]*\\[")
  message(FATAL_ERROR "JSON output is missing the positions array.")
endif()

file(GLOB default_profiles "${WORKDIR}/*.prf")
if(default_profiles)
  message(FATAL_ERROR "Default run left temporary profile files: ${default_profiles}")
endif()

file(GLOB default_filters "${WORKDIR}/*.fil")
if(default_filters)
  message(FATAL_ERROR "Default run left temporary filter files: ${default_filters}")
endif()

execute_process(
    COMMAND "${SMASHPP}" viz -o smoke.svg "${json_file}"
    WORKING_DIRECTORY "${WORKDIR}"
    RESULT_VARIABLE viz_result
    OUTPUT_VARIABLE viz_stdout
    ERROR_VARIABLE viz_stderr)

if(NOT viz_result EQUAL 0)
  message(FATAL_ERROR
          "Smash++ visualizer smoke run failed with exit code ${viz_result}\n"
          "stdout:\n${viz_stdout}\n"
          "stderr:\n${viz_stderr}")
endif()

set(svg_file "${WORKDIR}/smoke.svg")
if(NOT EXISTS "${svg_file}")
  message(FATAL_ERROR "Expected SVG output was not created: ${svg_file}")
endif()

file(READ "${svg_file}" svg_content)
if(NOT svg_content MATCHES "<svg")
  message(FATAL_ERROR "SVG output does not contain an <svg> element.")
endif()

set(save_profile_workdir "${WORKDIR}/save-profile")
file(MAKE_DIRECTORY "${save_profile_workdir}")

execute_process(
    COMMAND "${SMASHPP}" -sp -nr -fmt json -r "${REF}" -t "${TAR}"
    WORKING_DIRECTORY "${save_profile_workdir}"
    RESULT_VARIABLE save_profile_result
    OUTPUT_VARIABLE save_profile_stdout
    ERROR_VARIABLE save_profile_stderr)

if(NOT save_profile_result EQUAL 0)
  message(FATAL_ERROR
          "Smash++ save-profile smoke run failed with exit code ${save_profile_result}\n"
          "stdout:\n${save_profile_stdout}\n"
          "stderr:\n${save_profile_stderr}")
endif()

file(GLOB saved_profiles "${save_profile_workdir}/*.prf")
if(NOT saved_profiles)
  message(FATAL_ERROR "Save-profile run did not create any profile files.")
endif()
