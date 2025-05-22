#include "kettle/state/qubit_state_conversion.hpp"
#include <filesystem>
#include <stdexcept>

#include <kettle/kettle.hpp>

/*
    This code:
      - loads in the final simulated statevector from the QPE simulations
      - projects the final statevector against a provided binary register string
      - evolves the projected statevector over the reverse circuit
*/

static constexpr auto N_UNITARY_QUBITS_TWO_ROTOR = std::size_t {6};
static constexpr auto N_UNITARY_QUBITS_THREE_ROTOR = std::size_t {9};

struct CommandLineArguments
{
    CommandLineArguments(int argc, char** argv)
    {
        if (argc != 6) {
            throw std::runtime_error {
                "./a.out n_ancilla_qubits n_rotors abs_statevector_filepath abs_reverse_circuit_filepath abs_output_filepath binary_register_string\n"
            };
        }

        const auto arguments = std::vector<std::string> (argv + 1, argv + argc);

        n_ancilla_qubits = std::stoul(arguments[0]);
        const auto n_rotors = std::stoul(arguments[1]);
        abs_statevector_filepath = std::filesystem::path {arguments[2]};
        abs_reverse_circuit_filepath = std::filesystem::path {arguments[3]};
        abs_output_filepath = std::filesystem::path {arguments[4]};
        binary_register_bitset = ket::bitstring_to_dynamic_bitset(arguments[5]);

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

        n_total_qubits = n_ancilla_qubits + n_unitary_qubits;
    }

    std::size_t n_ancilla_qubits;
    std::size_t n_unitary_qubits;
    std::size_t n_total_qubits;
    std::filesystem::path abs_statevector_filepath;
    std::filesystem::path abs_reverse_circuit_filepath;
    std::filesystem::path abs_output_filepath;
    std::vector<std::uint8_t> binary_register_bitset;
};

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

    // load in the final statevector
    auto statevector = ket::load_statevector(args.abs_statevector_filepath);

    // project it against the provided binary register
    const auto qubit_indices = ket::arange(args.n_unitary_qubits, args.n_total_qubits);
    auto projected = ket::project_statevector(statevector, qubit_indices, args.binary_register_bitset);

    // evolve the projected state through the reversing circuit
    const auto reversing_circuit = ket::read_tangelo_circuit(args.n_unitary_qubits, args.abs_reverse_circuit_filepath, 0);
    ket::simulate(reversing_circuit, projected);
    ket::save_statevector(std::cout, projected, ket::QuantumStateEndian::BIG);

    return 0;
}

