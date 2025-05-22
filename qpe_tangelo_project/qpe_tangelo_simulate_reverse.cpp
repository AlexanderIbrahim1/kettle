#include <filesystem>
#include <iostream>
#include <stdexcept>

#include <kettle/kettle.hpp>

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
                "./a.out n_ancilla_qubits n_rotors abs_gate_dirpath abs_input_dirpath abs_output_dirpath statevector_filename\n"
            };
        }

        const auto arguments = std::vector<std::string> (argv + 1, argv + argc);

        n_ancilla_qubits = std::stoul(arguments[0]);
        const auto n_rotors = std::stoul(arguments[1]);
        abs_circuits_dirpath = std::filesystem::path {arguments[2]};
        abs_input_dirpath = std::filesystem::path {arguments[3]};
        abs_output_dirpath = std::filesystem::path {arguments[4]};
        statevector_filename = arguments[5];

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
    std::filesystem::path abs_circuits_dirpath;
    std::filesystem::path abs_input_dirpath;
    std::filesystem::path abs_output_dirpath;
    std::string statevector_filename;
};


void simulate_subcircuit(
    const std::filesystem::path& circuit_filepath,
    ket::QuantumState& statevector,
    std::size_t n_total_qubits
)
{
    const auto circuit = ket::read_tangelo_circuit(n_total_qubits, circuit_filepath, 0);
    ket::simulate(circuit, statevector);
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
            std::exit(EXIT_FAILURE);  // NOLINT(concurrency-mt-unsafe)
        }
    }();

    const auto n_total_qubits = args.n_ancilla_qubits + args.n_unitary_qubits;
    const auto reversed_statevector_filename = std::string{"reversed_"} + args.statevector_filename;

    auto statevector = ket::load_statevector(args.abs_input_dirpath / args.statevector_filename);

    simulate_subcircuit(args.abs_circuits_dirpath / "reverse_initial_circuit.dat", statevector, n_total_qubits);

    ket::save_statevector(args.abs_output_dirpath / reversed_statevector_filename, statevector);

    return 0;
}

