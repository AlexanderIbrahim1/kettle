cmake_minimum_required(VERSION 3.14)

# ---- Declare library ----

add_library(kettle_kettle INTERFACE)
add_library(kettle::kettle ALIAS kettle_kettle)

set_property(
    TARGET kettle_kettle
    PROPERTY
    EXPORT_NAME kettle
)

target_include_directories(
    kettle_kettle
    ${warning_guard}
    INTERFACE
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
)

target_compile_features(
    kettle_kettle
    INTERFACE
    cxx_std_20
)
