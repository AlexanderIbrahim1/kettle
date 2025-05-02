#include <filesystem>
#include <stdexcept>

#include <kettle/kettle.hpp>

/*
    Perform QPE for the N = 2 and N = 3 gates for the rotor paper.
*/

static constexpr auto N_UNITARY_QUBITS_TWO_ROTOR = std::size_t {6};
static constexpr auto N_UNITARY_QUBITS_THREE_ROTOR = std::size_t {9};

struct CommandLineArguments
{
    CommandLineArguments(int argc, char** argv)
    {
        if (argc != 5) {
            throw std::runtime_error {
                "./a.out n_ancilla_qubits n_rotors abs_gate_filepath abs_statevector_output_filepath\n"
            };
        }

        const auto arguments = std::vector<std::string> (argv + 1, argv + argc);

        n_ancilla_qubits = std::stoul(arguments[0]);
        const auto n_rotors = std::stoul(arguments[1]);
        abs_gate_filepath = std::filesystem::path {arguments[2]};
        abs_statevector_output_filepath = std::filesystem::path {arguments[3]};

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
    std::filesystem::path abs_gate_filepath;
    std::filesystem::path abs_statevector_output_filepath;
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
            std::exit(EXIT_FAILURE);  // NOLINT(concurrency-mt-unsafe)
        }
    }();

    const auto n_total_qubits = arguments.n_ancilla_qubits + arguments.n_unitary_qubits;
    const auto circuit = ket::read_tangelo_circuit(n_total_qubits, arguments.abs_gate_filepath, 0);

    auto statevector = ket::QuantumState {n_total_qubits};
    ket::simulate(circuit, statevector);

    ket::save_statevector(arguments.abs_statevector_output_filepath, statevector);

    return 0;
}

