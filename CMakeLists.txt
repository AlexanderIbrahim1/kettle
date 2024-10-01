cmake_minimum_required(VERSION 3.14)

include(cmake/prelude.cmake)

project(
    mini-qiskit
    VERSION 0.1.0
    DESCRIPTION "A reproduction of MicroQiskit, with some changes in implementation."
    HOMEPAGE_URL "https://example.com/"
    LANGUAGES CXX
)

include(cmake/project-is-top-level.cmake)
include(cmake/variables.cmake)

add_subdirectory(include) # mini-qiskit

# ---- Install rules ----

if(NOT CMAKE_SKIP_INSTALL_RULES)
  include(cmake/install-rules.cmake)
endif()

# ---- Examples ----

if(PROJECT_IS_TOP_LEVEL)
  option(BUILD_EXAMPLES "Build examples tree." "${mini-qiskit_DEVELOPER_MODE}")
  if(BUILD_EXAMPLES)
    add_subdirectory(example)
  endif()
endif()

# ---- Developer mode ----

if(NOT mini-qiskit_DEVELOPER_MODE)
  return()
elseif(NOT PROJECT_IS_TOP_LEVEL)
  message(
      AUTHOR_WARNING
      "Developer mode is intended for developers of mini-qiskit"
  )
endif()

include(cmake/dev-mode.cmake)
