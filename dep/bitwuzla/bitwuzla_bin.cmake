include(ExternalProject)
ExternalProject_Add(
  bitwuzla
  GIT_REPOSITORY https://github.com/bitwuzla/bitwuzla.git
  GIT_TAG main
  BUILD_IN_SOURCE 1
  CONFIGURE_COMMAND meson setup build -Dbuildtype=release
  BUILD_COMMAND meson compile -C build
  INSTALL_COMMAND cp build/src/main/bitwuzla ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)
