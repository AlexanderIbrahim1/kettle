#include <filesystem>
#include <fstream>
#include <iostream>

#include <mini-qiskit/decomposed/decomposed_gate.hpp>
#include <mini-qiskit/decomposed/read_decomposition_file.hpp>
#include <mini-qiskit/circuit_operations/build_decomposed_circuit.hpp>
#include <mini-qiskit/common/print.hpp>
#include <mini-qiskit/circuit.hpp>
#include <mini-qiskit/simulate.hpp>
#include <mini-qiskit/state.hpp>

auto main() -> int
{
    auto stream = std::stringstream {
        "NUMBER_OF_COMMANDS : 7                          \n"
        "ALLCONTROL : 0                                  \n"
        "  0.0000000000000000e+00 -0.0000000000000000e+00\n"
        "  1.0000000000000000e+00 -0.0000000000000000e+00\n"
        "  1.0000000000000000e+00 -0.0000000000000000e+00\n"
        "  0.0000000000000000e+00 -0.0000000000000000e+00\n"
        "ALLCONTROL : 1                                  \n"
        "  7.0710678118654757e-01  0.0000000000000000e+00\n"
        "  7.0710678118654746e-01 -8.6595605623549316e-17\n"
        " -7.0710678118654746e-01 -8.6595605623549316e-17\n"
        "  7.0710678118654757e-01 -0.0000000000000000e+00\n"
        "SINGLEGATE : 1                                  \n"
        "  0.0000000000000000e+00  0.0000000000000000e+00\n"
        "  1.0000000000000000e+00  0.0000000000000000e+00\n"
        "  1.0000000000000000e+00  0.0000000000000000e+00\n"
        "  0.0000000000000000e+00  0.0000000000000000e+00\n"
        "ALLCONTROL : 0                                  \n"
        "  5.7735026918962584e-01  0.0000000000000000e+00\n"
        "  8.1649658092772603e-01 -9.9991992434789747e-17\n"
        " -8.1649658092772603e-01 -9.9991992434789747e-17\n"
        "  5.7735026918962584e-01 -0.0000000000000000e+00\n"
        "SINGLEGATE : 1                                  \n"
        "  0.0000000000000000e+00  0.0000000000000000e+00\n"
        "  1.0000000000000000e+00  0.0000000000000000e+00\n"
        "  1.0000000000000000e+00  0.0000000000000000e+00\n"
        "  0.0000000000000000e+00  0.0000000000000000e+00\n"
        "ALLCONTROL : 1                                  \n"
        " -7.0710678118654757e-01  8.6595605623549341e-17\n"
        " -3.5731629454852966e-16 -7.0710678118654746e-01\n"
        "  3.5731629454852966e-16 -7.0710678118654746e-01\n"
        " -7.0710678118654757e-01 -8.6595605623549341e-17\n"
        "ALLCONTROL : 0                                  \n"
        "  0.0000000000000000e+00  0.0000000000000000e+00\n"
        " -5.7667474161826995e-16 -1.0000000000000000e+00\n"
        "  0.0000000000000000e+00 -1.0000000000000000e+00\n"
        "  0.0000000000000000e+00  0.0000000000000000e+00\n"
    };
    // const auto filename = std::string {"decomposition_example.txt"};
    // const auto filepath = std::filesystem::path {"example"} / filename;

    const auto gates = mqis::read_decomposed_gate_info(stream);

    for (const auto& gate : gates) {
        std::cout << "STATE: " << static_cast<int>(gate.state) << '\n';
        std::cout << "QUBIT: " << gate.qubit_index << '\n';
        std::cout << "(" << gate.matrix.elem00.real() << ", " << gate.matrix.elem00.imag() << ")\n";
        std::cout << "(" << gate.matrix.elem01.real() << ", " << gate.matrix.elem01.imag() << ")\n";
        std::cout << "(" << gate.matrix.elem10.real() << ", " << gate.matrix.elem10.imag() << ")\n";
        std::cout << "(" << gate.matrix.elem11.real() << ", " << gate.matrix.elem11.imag() << ")\n";
    }

    const auto circuit = mqis::make_circuit_from_decomposed_gates(gates);

    // mqis::print_circuit(circuit);

    auto statevector = mqis::QuantumState {"11"};
    mqis::simulate(circuit, statevector);

    mqis::print_state(statevector);

    return 0;
}
