cmake_minimum_required(VERSION 3.14)

# ---- Declare library ----

add_library(mini-qiskit_mini-qiskit INTERFACE)
add_library(mini-qiskit::mini-qiskit ALIAS mini-qiskit_mini-qiskit)

set_property(
    TARGET mini-qiskit_mini-qiskit
    PROPERTY
    EXPORT_NAME mini-qiskit
)

target_include_directories(
    mini-qiskit_mini-qiskit
    ${warning_guard}
    INTERFACE
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
)

target_compile_features(
    mini-qiskit_mini-qiskit
    INTERFACE
    cxx_std_20
)
