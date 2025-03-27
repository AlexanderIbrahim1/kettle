cmake_minimum_required(VERSION 3.14)

project(mini-qiskitExamples CXX)

include(../cmake/project-is-top-level.cmake)
include(../cmake/folders.cmake)

if(PROJECT_IS_TOP_LEVEL)
  find_package(mini-qiskit REQUIRED)
endif()

add_custom_target(run-examples)

function(add_example NAME)
  add_executable("${NAME}" "${NAME}.cpp")
  target_link_libraries("${NAME}" PRIVATE mini-qiskit::mini-qiskit)
  target_compile_features("${NAME}" PRIVATE cxx_std_20)
  add_custom_target("run_${NAME}" COMMAND "${NAME}" VERBATIM)
  add_dependencies("run_${NAME}" "${NAME}")
  add_dependencies(run-examples "run_${NAME}")
endfunction()

add_example(deutsch)
add_example(deutsch_jozsa)
add_example(deutsch_jozsa_balanced_ibm_example)
add_example(grover_two_qubit)
add_example(inverse_fourier)
add_example(quantum_phase_estimation_t_gate)
add_example(shor)
add_example(simon_ibm_example)
add_example(read_decomposition_file)
add_example(quantum_phase_estimation_diagonal)
add_example(quantum_phase_estimation_diagonal_binary)
add_example(quantum_phase_estimation_trotter20)

add_folders(Example)
