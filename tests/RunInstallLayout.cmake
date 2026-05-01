if(NOT DEFINED BUILD_DIR OR NOT DEFINED WORKDIR)
  message(FATAL_ERROR "BUILD_DIR and WORKDIR must be defined.")
endif()
if(NOT DEFINED INSTALL_BINDIR OR NOT DEFINED INSTALL_DOCDIR)
  message(FATAL_ERROR "INSTALL_BINDIR and INSTALL_DOCDIR must be defined.")
endif()
if(NOT DEFINED EXECUTABLE_SUFFIX)
  set(EXECUTABLE_SUFFIX "")
endif()

file(REMOVE_RECURSE "${WORKDIR}")
set(prefix "${WORKDIR}/prefix")

set(install_command "${CMAKE_COMMAND}" --install "${BUILD_DIR}" --prefix "${prefix}")
if(DEFINED CONFIG AND NOT CONFIG STREQUAL "")
  list(APPEND install_command --config "${CONFIG}")
endif()

execute_process(
    COMMAND ${install_command}
    RESULT_VARIABLE install_result
    OUTPUT_VARIABLE install_stdout
    ERROR_VARIABLE install_stderr)

if(NOT install_result EQUAL 0)
  message(FATAL_ERROR
          "Install layout smoke test failed with exit code ${install_result}\n"
          "stdout:\n${install_stdout}\n"
          "stderr:\n${install_stderr}")
endif()

function(require_installed_file path description)
  if(NOT EXISTS "${path}")
    message(FATAL_ERROR "Missing installed ${description}: ${path}")
  endif()
endfunction()

set(bin_dir "${prefix}/${INSTALL_BINDIR}")
set(doc_dir "${prefix}/${INSTALL_DOCDIR}")
set(smashpp "${bin_dir}/smashpp${EXECUTABLE_SUFFIX}")
set(inv_rep "${bin_dir}/smashpp-inv-rep${EXECUTABLE_SUFFIX}")
set(exclude_n "${bin_dir}/exclude_N${EXECUTABLE_SUFFIX}")

require_installed_file("${smashpp}" "smashpp executable")
require_installed_file("${inv_rep}" "smashpp-inv-rep executable")
require_installed_file("${exclude_n}" "exclude_N executable")
require_installed_file("${doc_dir}/README.md" "README")
require_installed_file("${doc_dir}/LICENSE" "license")

execute_process(
    COMMAND "${smashpp}" -V
    RESULT_VARIABLE version_result
    OUTPUT_VARIABLE version_stdout
    ERROR_VARIABLE version_stderr)

if(NOT version_result EQUAL 0)
  message(FATAL_ERROR
          "Installed smashpp -V failed with exit code ${version_result}\n"
          "stdout:\n${version_stdout}\n"
          "stderr:\n${version_stderr}")
endif()

set(version_output "${version_stdout}\n${version_stderr}")
if(NOT version_output MATCHES "Smash\\+\\+ .+")
  message(FATAL_ERROR
          "Installed smashpp -V did not print a recognizable version.\n"
          "stdout:\n${version_stdout}\n"
          "stderr:\n${version_stderr}")
endif()
