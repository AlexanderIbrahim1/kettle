cmake_minimum_required(VERSION 3.14)

include(cmake/prelude.cmake)

project(
    kettle
    VERSION 0.1.0
    DESCRIPTION "A reproduction of MicroQiskit, with some changes in implementation."
    HOMEPAGE_URL "https://example.com/"
    LANGUAGES CXX
)

include(cmake/project-is-top-level.cmake)
include(cmake/variables.cmake)

add_subdirectory(include) # kettle

# ---- Install rules ----

if(NOT CMAKE_SKIP_INSTALL_RULES)
  include(cmake/install-rules.cmake)
endif()

# ---- Examples ----

if(PROJECT_IS_TOP_LEVEL)
  option(BUILD_EXAMPLES "Build examples tree." "${kettle_BUILD_EXAMPLES}")
  if(BUILD_EXAMPLES)
    add_subdirectory(example)
  endif()
endif()

# ---- QPE Tangelo Project ----

if(PROJECT_IS_TOP_LEVEL)
  option(BUILD_QPE_TANGELO_PROJECT "Build QPE tangelo project tree." "${kettle_BUILD_QPE_TANGELO_PROJECT}")
  if(BUILD_QPE_TANGELO_PROJECT)
    add_subdirectory(qpe_tangelo_project)
  endif()
endif()

# ---- Developer mode ----

if(NOT kettle_DEVELOPER_MODE)
  return()
elseif(NOT PROJECT_IS_TOP_LEVEL)
  message(
      AUTHOR_WARNING
      "Developer mode is intended for developers of kettle"
  )
endif()

include(cmake/dev-mode.cmake)
