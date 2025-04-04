#include <filesystem>
#include <format>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include <mini-qiskit/decomposed/read_tangelo_file.hpp>
#include <mini-qiskit/circuit_operations/append_circuits.hpp>
#include <mini-qiskit/simulation/simulate.hpp>
#include <mini-qiskit/circuit.hpp>
#include <mini-qiskit/state.hpp>
#include <mini-qiskit/io/statevector.hpp>

/*
    Perform QPE for the N = 2 and N = 3 gates for the rotor paper, using the minimal
    output files for the gates of the circuit.
*/

static constexpr auto N_UNITARY_QUBITS_TWO_ROTOR = std::size_t {6};
static constexpr auto N_UNITARY_QUBITS_THREE_ROTOR = std::size_t {9};

struct CommandLineArguments
{
    CommandLineArguments(int argc, char** argv)
    {
        if (argc != 7) {
            throw std::runtime_error {
                "./a.out n_ancilla_qubits n_rotors n_trotter_steps abs_gate_dirpath subcircuit_filename abs_statevector_output_filepath\n"
            };
        }

        const auto arguments = std::vector<std::string> (argv + 1, argv + argc);

        n_ancilla_qubits = std::stoul(arguments[0]);
        const auto n_rotors = std::stoul(arguments[1]);
        n_trotter_steps = std::stoul(arguments[2]);
        abs_circuits_dirpath = std::filesystem::path {arguments[3]};
        subcircuit_filename = arguments[4];
        abs_statevector_output_filepath = std::filesystem::path {arguments[5]};

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
    std::size_t n_trotter_steps;
    std::filesystem::path abs_circuits_dirpath;
    std::string subcircuit_filename;
    std::filesystem::path abs_statevector_output_filepath;
};

void simulate_subcircuit(
    const std::filesystem::path& circuit_filepath,
    mqis::QuantumState& statevector,
    std::size_t n_total_qubits
)
{
    const auto circuit = mqis::read_tangelo_circuit(n_total_qubits, circuit_filepath, 0);
    mqis::simulate(circuit, statevector);
}

void simulate_unitary(
    const CommandLineArguments& args,
    mqis::QuantumState& statevector,
    std::size_t i_control
)
{
    auto n_powers = 1ul << i_control;
    const auto n_total_qubits = args.n_ancilla_qubits + args.n_unitary_qubits;

    const auto circuit_filepath = [&]() {
        auto output = std::stringstream {};
        output << args.subcircuit_filename << i_control;

        return args.abs_circuits_dirpath / output.str();
    }();

    const auto circuit = mqis::read_tangelo_circuit(n_total_qubits, circuit_filepath, 0);

    for (std::size_t i {0}; i < n_powers; ++i) {
        for (std::size_t i_trotter_ {0}; i_trotter_ < args.n_trotter_steps; ++i_trotter_) {
            mqis::simulate(circuit, statevector);
        }
    }
}

auto main(int argc, char** argv) -> int
{
    const auto args = [&]() {
        try {
            return CommandLineArguments {argc, argv};
        }
        catch (const std::exception& e)
        {
            std::cout << e.what() << '\n';
            std::exit(EXIT_FAILURE);
        }
    }();

    const auto n_total_qubits = args.n_ancilla_qubits + args.n_unitary_qubits;
    auto statevector = mqis::QuantumState {n_total_qubits};

    // simulate the initial circuit
    simulate_subcircuit(args.abs_circuits_dirpath / "initial_circuit.dat", statevector, n_total_qubits);
    simulate_subcircuit(args.abs_circuits_dirpath / "qft_circuit.dat", statevector, n_total_qubits);

    for (std::size_t i_control {0}; i_control < args.n_ancilla_qubits; ++i_control) {
        simulate_unitary(args, statevector, i_control);
    }

    simulate_subcircuit(args.abs_circuits_dirpath / "iqft_circuit.dat", statevector, n_total_qubits);

    mqis::save_statevector(args.abs_statevector_output_filepath, statevector);

    return 0;
}

