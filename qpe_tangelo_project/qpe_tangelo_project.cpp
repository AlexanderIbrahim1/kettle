#include <filesystem>
#include <format>
#include <fstream>
#include <stdexcept>

#include <kettle/kettle.hpp>

/*
    This code:
      - loads in the final simulated statevector from the QPE simulations
      - for each register bitstring:
        - projects the final statevector against the register bitstring
        - saves the projected statevector in its own file in the output directory
*/

static constexpr auto N_UNITARY_QUBITS_TWO_ROTOR = std::size_t {6};
static constexpr auto N_UNITARY_QUBITS_THREE_ROTOR = std::size_t {9};

struct CommandLineArguments
{
    CommandLineArguments(int argc, char** argv)
    {
        if (argc != 6) {
            throw std::runtime_error {
                "./a.out n_ancilla_qubits n_rotors abs_input_dirpath abs_output_dirpath statevector_filename\n"
            };
        }

        const auto arguments = std::vector<std::string> (argv + 1, argv + argc);

        n_ancilla_qubits = std::stoul(arguments[0]);
        const auto n_rotors = std::stoul(arguments[1]);
        abs_input_dirpath = std::filesystem::path {arguments[2]};
        abs_output_dirpath = std::filesystem::path {arguments[3]};
        statevector_filename = arguments[4];

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
    std::filesystem::path abs_input_dirpath;
    std::filesystem::path abs_output_dirpath;
    std::string statevector_filename;
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

    auto statevector = ket::load_statevector(args.abs_input_dirpath / args.statevector_filename);
    const auto ancilla_qubit_indices = ket::arange(args.n_unitary_qubits, args.n_total_qubits);

    for (std::size_t i_state {0}; i_state < (1UL << args.n_ancilla_qubits); ++i_state) {
        // project it against the provided binary register
        const auto ancilla_bitstring = ket::state_index_to_bitstring_big_endian(i_state, args.n_ancilla_qubits);
        const auto ancilla_bitset = ket::bitstring_to_dynamic_bitset(ancilla_bitstring);
        const auto projected = ket::project_statevector(statevector, ancilla_qubit_indices, ancilla_bitset);

        const auto output_filepath = args.abs_output_dirpath / std::format("projected_{}_{}", ancilla_bitstring, args.statevector_filename);
        auto outstream = std::ofstream {output_filepath};
        if (!outstream.is_open()) {
            throw std::ios::failure {std::format("ERROR: cannot open output file '{}'", output_filepath.c_str())};
        }

        ket::save_statevector(outstream, projected, ket::QuantumStateEndian::BIG);
    }

    return 0;
}
