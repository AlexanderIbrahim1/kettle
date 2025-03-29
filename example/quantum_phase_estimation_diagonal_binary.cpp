#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>

#include <mini-qiskit/mini-qiskit.hpp>
#include <mini-qiskit/decomposed/decomposed_gate.hpp>
#include <mini-qiskit/decomposed/read_decomposition_file.hpp>
#include <mini-qiskit/circuit_operations/append_circuits.hpp>
#include <mini-qiskit/circuit_operations/build_decomposed_circuit.hpp>
#include <mini-qiskit/circuit_operations/make_binary_controlled_circuit.hpp>
#include <mini-qiskit/gates/fourier.hpp>
#include <mini-qiskit/circuit.hpp>
#include <mini-qiskit/simulation/simulate.hpp>
#include <mini-qiskit/state.hpp>

/*
    This example creates the same circuit as displayed in `quantum_phase_estimation_diagonal.cpp`,
    except the binary controlled circuit is built from multiple vectors of decomposed gates,
    each of which is a different power of 2 of the gate of interest.

    The expected outputs are the same.
*/

auto get_gate_pow_1_stream() -> std::stringstream
{
    return std::stringstream {
        "NUMBER_OF_COMMANDS : 5                          \n"
        "SINGLEGATE : 1                                  \n"
        "  0.0000000000000000e+00  0.0000000000000000e+00\n"
        "  1.0000000000000000e+00  0.0000000000000000e+00\n"
        "  1.0000000000000000e+00  0.0000000000000000e+00\n"
        "  0.0000000000000000e+00  0.0000000000000000e+00\n"
        "ALLCONTROL : 0                                  \n"
        " -5.5557023301960196e-01  8.3146961230254535e-01\n"
        "  0.0000000000000000e+00 -0.0000000000000000e+00\n"
        "  0.0000000000000000e+00 -0.0000000000000000e+00\n"
        " -5.5557023301960196e-01 -8.3146961230254546e-01\n"
        "SINGLEGATE : 1                                  \n"
        "  0.0000000000000000e+00  0.0000000000000000e+00\n"
        "  1.0000000000000000e+00  0.0000000000000000e+00\n"
        "  1.0000000000000000e+00  0.0000000000000000e+00\n"
        "  0.0000000000000000e+00  0.0000000000000000e+00\n"
        "ALLCONTROL : 1                                  \n"
        "  8.8192126434835483e-01 -4.7139673682599792e-01\n"
        "  0.0000000000000000e+00 -0.0000000000000000e+00\n"
        "  0.0000000000000000e+00 -0.0000000000000000e+00\n"
        "  8.8192126434835494e-01  4.7139673682599792e-01\n"
        "ALLCONTROL : 0                                  \n"
        " -5.5557023301960218e-01 -8.3146961230254524e-01\n"
        "  0.0000000000000000e+00  0.0000000000000000e+00\n"
        "  0.0000000000000000e+00  0.0000000000000000e+00\n"
        " -7.0710678118654724e-01  7.0710678118654779e-01\n"
    };
}

auto get_gate_pow_2_stream() -> std::stringstream
{
    return std::stringstream {
        "NUMBER_OF_COMMANDS : 5                          \n"
        "SINGLEGATE : 1                                  \n"
        "  0.0000000000000000e+00  0.0000000000000000e+00\n"
        "  1.0000000000000000e+00  0.0000000000000000e+00\n"
        "  1.0000000000000000e+00  0.0000000000000000e+00\n"
        "  0.0000000000000000e+00  0.0000000000000000e+00\n"
        "ALLCONTROL : 0                                  \n"
        " -3.8268343236509045e-01 -9.2387953251128652e-01\n"
        "  0.0000000000000000e+00 -0.0000000000000000e+00\n"
        "  0.0000000000000000e+00 -0.0000000000000000e+00\n"
        " -3.8268343236509045e-01  9.2387953251128652e-01\n"
        "SINGLEGATE : 1                                  \n"
        "  0.0000000000000000e+00  0.0000000000000000e+00\n"
        "  1.0000000000000000e+00  0.0000000000000000e+00\n"
        "  1.0000000000000000e+00  0.0000000000000000e+00\n"
        "  0.0000000000000000e+00  0.0000000000000000e+00\n"
        "ALLCONTROL : 1                                  \n"
        "  5.5557023301960151e-01 -8.3146961230254568e-01\n"
        "  0.0000000000000000e+00 -0.0000000000000000e+00\n"
        "  0.0000000000000000e+00 -0.0000000000000000e+00\n"
        "  5.5557023301960151e-01  8.3146961230254568e-01\n"
        "ALLCONTROL : 0                                  \n"
        " -3.8268343236508984e-01  9.2387953251128663e-01\n"
        "  0.0000000000000000e+00  0.0000000000000000e+00\n"
        "  0.0000000000000000e+00  0.0000000000000000e+00\n"
        " -9.9920072216264089e-16 -9.9999999999999989e-01\n"
    };
}

auto get_gate_pow_4_stream() -> std::stringstream
{
    return std::stringstream {
        "NUMBER_OF_COMMANDS : 5                          \n"
        "SINGLEGATE : 1                                  \n"
        "  0.0000000000000000e+00  0.0000000000000000e+00\n"
        "  1.0000000000000000e+00  0.0000000000000000e+00\n"
        "  1.0000000000000000e+00  0.0000000000000000e+00\n"
        "  0.0000000000000000e+00  0.0000000000000000e+00\n"
        "ALLCONTROL : 0                                  \n"
        " -7.0710678118654646e-01  7.0710678118654846e-01\n"
        "  0.0000000000000000e+00 -0.0000000000000000e+00\n"
        "  0.0000000000000000e+00 -0.0000000000000000e+00\n"
        " -7.0710678118654657e-01 -7.0710678118654857e-01\n"
        "SINGLEGATE : 1                                  \n"
        "  0.0000000000000000e+00  0.0000000000000000e+00\n"
        "  1.0000000000000000e+00  0.0000000000000000e+00\n"
        "  1.0000000000000000e+00  0.0000000000000000e+00\n"
        "  0.0000000000000000e+00  0.0000000000000000e+00\n"
        "ALLCONTROL : 1                                  \n"
        " -3.8268343236509111e-01 -9.2387953251128618e-01\n"
        "  0.0000000000000000e+00 -0.0000000000000000e+00\n"
        "  0.0000000000000000e+00 -0.0000000000000000e+00\n"
        " -3.8268343236509117e-01  9.2387953251128629e-01\n"
        "ALLCONTROL : 0                                  \n"
        " -7.0710678118654724e-01 -7.0710678118654757e-01\n"
        "  0.0000000000000000e+00  0.0000000000000000e+00\n"
        "  0.0000000000000000e+00  0.0000000000000000e+00\n"
        " -1.0000000000000000e+00  1.7208456881689926e-15\n"
    };
}

auto get_gate_pow_8_stream() -> std::stringstream
{
    return std::stringstream {
        "NUMBER_OF_COMMANDS : 5                          \n"
        "SINGLEGATE : 1                                  \n"
        "  0.0000000000000000e+00  0.0000000000000000e+00\n"
        "  1.0000000000000000e+00  0.0000000000000000e+00\n"
        "  1.0000000000000000e+00  0.0000000000000000e+00\n"
        "  0.0000000000000000e+00  0.0000000000000000e+00\n"
        "ALLCONTROL : 0                                  \n"
        " -2.7755575615628898e-15 -9.9999999999999978e-01\n"
        "  0.0000000000000000e+00 -0.0000000000000000e+00\n"
        "  0.0000000000000000e+00 -0.0000000000000000e+00\n"
        " -2.7755575615628914e-15  1.0000000000000002e+00\n"
        "SINGLEGATE : 1                                  \n"
        "  0.0000000000000000e+00  0.0000000000000000e+00\n"
        "  1.0000000000000000e+00  0.0000000000000000e+00\n"
        "  1.0000000000000000e+00  0.0000000000000000e+00\n"
        "  0.0000000000000000e+00  0.0000000000000000e+00\n"
        "ALLCONTROL : 1                                  \n"
        " -7.0710678118654524e-01  7.0710678118654946e-01\n"
        "  0.0000000000000000e+00 -0.0000000000000000e+00\n"
        "  0.0000000000000000e+00 -0.0000000000000000e+00\n"
        " -7.0710678118654557e-01 -7.0710678118654979e-01\n"
        "ALLCONTROL : 0                                  \n"
        " -4.9960036108132044e-16  9.9999999999999967e-01\n"
        "  0.0000000000000000e+00  0.0000000000000000e+00\n"
        "  0.0000000000000000e+00  0.0000000000000000e+00\n"
        "  1.0000000000000000e+00 -3.3861802251067274e-15\n"
    };
}

auto get_gate_pow_16_stream() -> std::stringstream
{
    return std::stringstream {
        "NUMBER_OF_COMMANDS : 5                          \n"
        "SINGLEGATE : 1                                  \n"
        "  0.0000000000000000e+00  0.0000000000000000e+00\n"
        "  1.0000000000000000e+00  0.0000000000000000e+00\n"
        "  1.0000000000000000e+00  0.0000000000000000e+00\n"
        "  0.0000000000000000e+00  0.0000000000000000e+00\n"
        "ALLCONTROL : 0                                  \n"
        " -9.9999999999999956e-01  5.5511151231257795e-15\n"
        "  0.0000000000000000e+00 -0.0000000000000000e+00\n"
        "  0.0000000000000000e+00 -0.0000000000000000e+00\n"
        " -1.0000000000000004e+00 -5.5511151231257843e-15\n"
        "SINGLEGATE : 1                                  \n"
        "  0.0000000000000000e+00  0.0000000000000000e+00\n"
        "  1.0000000000000000e+00  0.0000000000000000e+00\n"
        "  1.0000000000000000e+00  0.0000000000000000e+00\n"
        "  0.0000000000000000e+00  0.0000000000000000e+00\n"
        "ALLCONTROL : 1                                  \n"
        " -5.8841820305133249e-15 -9.9999999999999933e-01\n"
        "  0.0000000000000000e+00 -0.0000000000000000e+00\n"
        "  0.0000000000000000e+00 -0.0000000000000000e+00\n"
        " -5.8841820305133328e-15  1.0000000000000007e+00\n"
        "ALLCONTROL : 0                                  \n"
        " -9.9999999999999933e-01 -9.9920072216264049e-16\n"
        "  0.0000000000000000e+00  0.0000000000000000e+00\n"
        "  0.0000000000000000e+00  0.0000000000000000e+00\n"
        "  1.0000000000000002e+00 -6.8278716014447143e-15\n"
    };
}

auto get_gate_pow_32_stream() -> std::stringstream
{
    return std::stringstream {
        "NUMBER_OF_COMMANDS : 1                          \n"
        "ALLCONTROL : 1                                  \n"
        " -9.9999999999999867e-01  1.1768364061026640e-14\n"
        "  0.0000000000000000e+00 -0.0000000000000000e+00\n"
        "  0.0000000000000000e+00 -0.0000000000000000e+00\n"
        " -1.0000000000000013e+00 -1.1768364061026672e-14\n"
    };
}

auto main() -> int
{
    auto streams = std::vector<std::stringstream> {};
    streams.emplace_back(get_gate_pow_1_stream());
    streams.emplace_back(get_gate_pow_2_stream());
    streams.emplace_back(get_gate_pow_4_stream());
    streams.emplace_back(get_gate_pow_8_stream());
    streams.emplace_back(get_gate_pow_16_stream());
    streams.emplace_back(get_gate_pow_32_stream());

    // create the circuit representing the powers of the 4x4 unitary matrix
    auto unitary_op_circuits = [&]() {
        auto output = std::vector<mqis::QuantumCircuit> {};

        for (auto& stream : streams) {
            auto gates = mqis::read_decomposed_gate_info(stream);
            auto unitary_op_circuit = mqis::make_circuit_from_decomposed_gates(std::move(gates));

            output.emplace_back(std::move(unitary_op_circuit));
        }

        return output;
    }();

    // make the unitary operator circuit a controlled circuit
    // - it will be controlled by 6 other qubits, in a binary controlled manner
    auto subcircuit = mqis::make_binary_controlled_circuit_from_binary_powers(unitary_op_circuits, 8, {0, 1, 2, 3, 4, 5}, {6, 7});

    // create the circuit needed to perform quantum phase estimation
    auto circuit = mqis::QuantumCircuit {8};
    circuit.add_h_gate({0, 1, 2, 3, 4, 5});
    mqis::extend_circuit(circuit, subcircuit);
    mqis::apply_inverse_fourier_transform(circuit, {5, 4, 3, 2, 1, 0});
    circuit.add_m_gate({0, 1, 2, 3, 4, 5});

    // create the input statevector
    // - we set the eigenstates for the unitary operator directly, rather than through x-gates
    auto counting_statevector = mqis::QuantumState {"000000"};
    auto unitary_eigenstatevector = mqis::QuantumState {"00"};
    auto statevector = mqis::tensor_product(counting_statevector, unitary_eigenstatevector);

    // perform the simulation
    mqis::simulate(circuit, statevector);

    // perform the measurements
    const auto counts = mqis::perform_measurements_as_counts_marginal(circuit, statevector, 1024);

    // output the results
    for (const auto& [bitstring, count]: counts) {
        std::cout << "(state, count) = (" << bitstring << ", " << count << ")\n";

        const auto rstripped_bitstring = mqis::rstrip_marginal_bits(bitstring);
        const auto state_index = mqis::bitstring_to_state_index(rstripped_bitstring);
        const auto n_states = 1ul << 6;
        const auto binary_fraction = static_cast<double>(state_index) / static_cast<double>(n_states);

        const auto estimated_phase = 2.0 * M_PI * binary_fraction;

        // the internal layout of the mini-qiskit simulator is little-endian, so we need
        // to reverse the bitstring to get the binary expansion used to calculate the phase
        auto binary_expansion = rstripped_bitstring;
        std::reverse(binary_expansion.begin(), binary_expansion.end());

        std::cout << "binary expansion: " << binary_expansion << '\n';
        std::cout << "estimated phase:  " << estimated_phase << '\n';
    }
    // Expected output (for "00"):
    // ```
    // (state, count) = (011010xx, 1024)
    // binary expansion: 010110
    // estimated phase:  2.15984
    // ```
    //
    // Expected output (for "10"):
    // ```
    // (state, count) = (101001xx, 1024)
    // binary expansion: 100101
    // estimated phase:  3.63247
    // ```
    //
    // Expected output (for "01"):
    // ```
    // (state, count) = (010101xx, 1024)
    // binary expansion: 101010
    // estimated phase:  4.12334
    // ```
    //
    // Expected output (for "11"):
    // ```
    // (state, count) = (101110xx, 1024)
    // binary expansion: 011101
    // estimated phase:  2.84707
    // ```

    return 0;
}

