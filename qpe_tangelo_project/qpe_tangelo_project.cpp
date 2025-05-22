#include <filesystem>
#include <format>
#include <stdexcept>

#include <kettle/kettle.hpp>

/*
    This code:
      - loads in the final simulated statevector from the QPE simulations, after it also passed through the reverse circuit
      - projects the final statevector against a provided binary register string
      - saves the projected statevector
*/

static constexpr auto N_UNITARY_QUBITS_TWO_ROTOR = std::size_t {6};
static constexpr auto N_UNITARY_QUBITS_THREE_ROTOR = std::size_t {9};

struct CommandLineArguments
{
    CommandLineArguments(int argc, char** argv)
    {
        if (argc != 7) {
            throw std::runtime_error {
                "./a.out n_ancilla_qubits n_rotors abs_rev_input_dirpath abs_rev_output_dirpath statevector_filename binary_register_string\n"
            };
        }

        const auto arguments = std::vector<std::string> (argv + 1, argv + argc);

        n_ancilla_qubits = std::stoul(arguments[0]);
        const auto n_rotors = std::stoul(arguments[1]);
        abs_rev_input_dirpath = std::filesystem::path {arguments[2]};
        abs_rev_output_dirpath = std::filesystem::path {arguments[3]};
        statevector_filename = arguments[4];
        binary_register_string = arguments[5];

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
    std::filesystem::path abs_rev_input_dirpath;
    std::filesystem::path abs_rev_output_dirpath;
    std::string statevector_filename;
    std::string binary_register_string;
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

    // load in the final reversed statevector
    auto rev_statevector = ket::load_statevector(args.abs_rev_input_dirpath / std::format("reversed_{}", args.statevector_filename));

    // project it against the provided binary register
    const auto qubit_indices = ket::arange(args.n_unitary_qubits, args.n_total_qubits);
    const auto register_bitset = ket::bitstring_to_dynamic_bitset(args.binary_register_string);
    auto projected = ket::project_statevector(rev_statevector, qubit_indices, register_bitset);

    // save the projected statevector
    const auto output_filepath = args.abs_rev_output_dirpath / std::format("projected_{}_{}", args.binary_register_string, args.statevector_filename);
    ket::save_statevector(output_filepath, projected, ket::QuantumStateEndian::BIG);

    return 0;
}

