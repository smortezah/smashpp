if(NOT DEFINED WORKDIR OR NOT DEFINED SMASHPP OR NOT DEFINED REF OR NOT DEFINED TAR)
  message(FATAL_ERROR "WORKDIR, SMASHPP, REF, and TAR must be defined.")
endif()

file(REMOVE_RECURSE "${WORKDIR}")
file(MAKE_DIRECTORY "${WORKDIR}")

execute_process(
    COMMAND "${SMASHPP}" -fmt json -nr -n 1 -r "${REF}" -t "${TAR}"
    WORKING_DIRECTORY "${WORKDIR}"
    RESULT_VARIABLE smashpp_result
    OUTPUT_VARIABLE smashpp_stdout
    ERROR_VARIABLE smashpp_stderr)

if(NOT smashpp_result EQUAL 0)
  message(FATAL_ERROR
          "Smash++ regression baseline failed with exit code ${smashpp_result}\n"
          "stdout:\n${smashpp_stdout}\n"
          "stderr:\n${smashpp_stderr}")
endif()

set(json_file "${WORKDIR}/ref.tar.json")
if(NOT EXISTS "${json_file}")
  message(FATAL_ERROR "Expected JSON output was not created: ${json_file}")
endif()

file(READ "${json_file}" json_content)

function(expect_json pattern description)
  if(NOT json_content MATCHES "${pattern}")
    message(FATAL_ERROR "Regression JSON is missing ${description}.")
  endif()
endfunction()

set(compact_json "${json_content}")
string(REGEX REPLACE "[ \t\r\n]" "" compact_json "${compact_json}")
function(expect_compact_json pattern description)
  if(NOT compact_json MATCHES "${pattern}")
    message(FATAL_ERROR "Regression JSON is missing ${description}.")
  endif()
endfunction()

string(REGEX MATCHALL "\"reference_begin\"" position_markers "${json_content}")
list(LENGTH position_markers position_count)
if(NOT position_count EQUAL 2)
  message(FATAL_ERROR "Expected 2 baseline positions, got ${position_count}.")
endif()

expect_json("\"watermark\"[ \t\r\n]*:[ \t\r\n]*\"##SMASH\\+\\+\"" "watermark")
expect_json("\"reference_size\"[ \t\r\n]*:[ \t\r\n]*\"1000\"" "reference size")
expect_json("\"target_size\"[ \t\r\n]*:[ \t\r\n]*\"1000\"" "target size")

expect_compact_json(
    "\\{\"reference_begin\":\"0\",\"reference_end\":\"510\",[^}]*\"target_begin\":\"500\",\"target_end\":\"999\",[^}]*\"inverted\":\"F\"\\}"
    "regular baseline position")
expect_compact_json(
    "\\{\"reference_begin\":\"498\",\"reference_end\":\"999\",[^}]*\"target_begin\":\"508\",\"target_end\":\"0\",[^}]*\"inverted\":\"T\"\\}"
    "inverted baseline position")
