include(ExternalProject)
ExternalProject_Add(
  fuzzbtor2
  GIT_REPOSITORY https://github.com/CoriolisSP/FuzzBtor2
  GIT_TAG main
  PATCH_COMMAND cp ${CMAKE_CURRENT_LIST_DIR}/CMakeLists.txt .
  INSTALL_COMMAND cp fuzzbtor2 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)
