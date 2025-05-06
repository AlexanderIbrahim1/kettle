cmake_minimum_required(VERSION 3.14)

include(cmake/prelude.cmake)

project(
    kettle
    VERSION 0.1.0
    DESCRIPTION "A quantum circuit simulation written in C++20"
    HOMEPAGE_URL "https://example.com/"
    LANGUAGES CXX
)

include(cmake/project-is-top-level.cmake)
include(cmake/variables.cmake)

# ---- Declare library ----

add_library(
    kettle_kettle
    source/kettle_internal/calculations/probabilities.cpp
    source/kettle_internal/calculations/measurements.cpp
    source/kettle_internal/circuit/circuit.cpp
    source/kettle_internal/circuit/control_flow_predicate.cpp
    source/kettle_internal/common/mathtools.cpp
    source/kettle_internal/common/matrix2x2.cpp
    source/kettle_internal/common/prng.cpp
    source/kettle_internal/common/state_test_utils.cpp
    source/kettle_internal/common/utils_internal.cpp
)
add_library(kettle::kettle ALIAS kettle_kettle)

include(GenerateExportHeader)
generate_export_header(
    kettle_kettle
    BASE_NAME kettle
    EXPORT_FILE_NAME export/kettle/kettle_export.hpp
    CUSTOM_CONTENT_FROM_VARIABLE pragma_suppress_c4251
)

if(NOT BUILD_SHARED_LIBS)
  target_compile_definitions(kettle_kettle PUBLIC KETTLE_STATIC_DEFINE)
endif()

set_target_properties(
    kettle_kettle PROPERTIES
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN YES
    VERSION "${PROJECT_VERSION}"
    SOVERSION "${PROJECT_VERSION_MAJOR}"
    EXPORT_NAME kettle
    OUTPUT_NAME kettle
)

target_include_directories(
    kettle_kettle
    ${warning_guard}
    PUBLIC
    "\$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
    PUBLIC
    "\$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/source>"
)

target_include_directories(
    kettle_kettle SYSTEM
    PUBLIC
    "\$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/export>"
)

target_compile_features(
    kettle_kettle
    PUBLIC
    cxx_std_20
)

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
