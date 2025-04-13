#include <filesystem>
#include <stdexcept>

#include <mini-qiskit/decomposed/read_tangelo_file.hpp>
#include <mini-qiskit/calculations/measurements.hpp>
#include <mini-qiskit/circuit_operations/append_circuits.hpp>
#include <mini-qiskit/simulation/simulate.hpp>
#include <mini-qiskit/circuit.hpp>
#include <mini-qiskit/state.hpp>
#include <mini-qiskit/io/statevector.hpp>

/*
    Measure the statevectors for the N = 2 and N = 3 gates for the rotor paper.
*/

constexpr auto get_measurement_qubits(std::size_t n_ancilla, std::size_t n_unitary) -> std::vector<std::size_t>
{
    auto output = std::vector<std::size_t> {};
    output.reserve(n_ancilla);

    for (std::size_t i {0}; i < n_ancilla; ++i) {
        output.push_back(n_unitary + i);
    }

    return output;
}

static constexpr auto N_UNITARY_QUBITS_TWO_ROTOR = std::size_t {6};
static constexpr auto N_UNITARY_QUBITS_THREE_ROTOR = std::size_t {9};

struct CommandLineArguments
{
    CommandLineArguments(int argc, char** argv)
    {
        if (argc != 4) {
            throw std::runtime_error {
                "./a.out n_ancilla_qubits n_rotors abs_statevector_filepath\n"
            };
        }

        const auto arguments = std::vector<std::string> (argv + 1, argv + argc);

        n_ancilla_qubits = std::stoul(arguments[0]);
        const auto n_rotors = std::stoul(arguments[1]);
        abs_statevector_filepath = std::filesystem::path {arguments[2]};

        if (n_rotors == 2) {
            n_unitary_qubits = N_UNITARY_QUBITS_TWO_ROTOR;
        }
        else if (n_rotors == 3) {
            n_unitary_qubits = N_UNITARY_QUBITS_THREE_ROTOR;
        }
        else {
            throw std::runtime_error {
                "Invalid number of rotors passed; allowed values are '2' and '3'\n"
            };
        }
    }

    std::size_t n_ancilla_qubits;
    std::size_t n_unitary_qubits;
    std::filesystem::path abs_statevector_filepath;
};

auto main(int argc, char** argv) -> int
{
    const auto arguments = [&]() {
        try {
            return CommandLineArguments {argc, argv};
        }
        catch (const std::exception& e)
        {
            std::cout << e.what() << '\n';
            std::exit(EXIT_FAILURE);
        }
    }();

    const auto n_total_qubits = arguments.n_ancilla_qubits + arguments.n_unitary_qubits;
    // const auto measurement_qubits = get_measurement_qubits(arguments.n_ancilla_qubits, arguments.n_unitary_qubits);

    auto marginal_qubits = std::vector<std::size_t> {};
    for (std::size_t i {0}; i < arguments.n_ancilla_qubits; ++i) {
        marginal_qubits.push_back(i);
    }

    auto statevector = mqis::load_statevector(arguments.abs_statevector_filepath);
    auto circuit = mqis::QuantumCircuit {n_total_qubits};
    // TODO: remove circuit.add_m_gate(measurement_qubits);

    mqis::simulate(circuit, statevector);

    const auto counts = mqis::perform_measurements_as_counts_marginal(statevector, 1ul << 12, marginal_qubits);

    for (const auto& [bitstring, count]: counts) {
        std::cout << "(state, count) = (" << bitstring << ", " << count << ")\n";
    }

    return 0;
}

