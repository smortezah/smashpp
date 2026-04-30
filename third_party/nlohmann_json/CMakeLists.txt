if (NOT TARGET nlohmann_json::nlohmann_json)
  add_library(smashpp_nlohmann_json INTERFACE)
  add_library(nlohmann_json::nlohmann_json ALIAS smashpp_nlohmann_json)

  target_compile_features(smashpp_nlohmann_json INTERFACE cxx_std_11)
  target_include_directories(
      smashpp_nlohmann_json SYSTEM
      INTERFACE "${CMAKE_CURRENT_LIST_DIR}/include")
endif()
