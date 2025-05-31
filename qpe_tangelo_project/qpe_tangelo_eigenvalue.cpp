#include <complex>
#include <filesystem>
#include <format>
#include <fstream>
#include <stdexcept>
#include <vector>

#include <kettle/kettle.hpp>

#include "matrix2d.cpp"  // NOLINT

/*
    This code:
      - loads in the final simulated statevector from the QPE simulations
      - loads in the hamiltonian
      - projects the final statevector against all possible binary register strings
      - calculates the estimated eigenvalue of each projected statevector
*/

static constexpr auto N_UNITARY_QUBITS_TWO_ROTOR = std::size_t {6};
static constexpr auto N_UNITARY_QUBITS_THREE_ROTOR = std::size_t {9};

struct CommandLineArguments
{
    CommandLineArguments(int argc, char** argv)
    {
        if (argc != 7) {
            throw std::runtime_error {
                "./a.out n_ancilla_qubits n_rotors abs_input_dirpath abs_output_dirpath abs_sparse_hamiltonian_filepath statevector_filename\n"
            };
        }

        const auto arguments = std::vector<std::string> (argv + 1, argv + argc);

        n_ancilla_qubits = std::stoul(arguments[0]);
        const auto n_rotors = std::stoul(arguments[1]);
        abs_input_dirpath = std::filesystem::path {arguments[2]};
        abs_output_dirpath = std::filesystem::path {arguments[3]};
        abs_sparse_hamiltonian_filepath = std::filesystem::path {arguments[4]};
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

        n_total_qubits = n_ancilla_qubits + n_unitary_qubits;
    }

    std::size_t n_ancilla_qubits;
    std::size_t n_unitary_qubits;
    std::size_t n_total_qubits;
    std::filesystem::path abs_input_dirpath;
    std::filesystem::path abs_output_dirpath;
    std::filesystem::path abs_sparse_hamiltonian_filepath;
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
    const auto n_unitary_states = 1UL << args.n_unitary_qubits;

    // stream to where the expectation values will be written
    const auto output_filepath = args.abs_output_dirpath / std::format("eigenvalues_{}", args.statevector_filename);
    auto outstream = std::ofstream {output_filepath};
    if (!outstream.is_open()) {
        throw std::ios::failure {std::format("ERROR: cannot open output file '{}'", output_filepath.c_str())};
    }

    // read in the hamiltonian
    auto hstream = std::ifstream {args.abs_sparse_hamiltonian_filepath};
    if (!hstream.is_open()) {
        throw std::ios::failure {std::format("ERROR: cannot open hamiltonian file '{}'", args.abs_sparse_hamiltonian_filepath.c_str())};
    }

    const auto hamiltonian = load_square_matrix(hstream, n_unitary_states);

    outstream << "# [projected register bitstring]   [<projected_state|H|projected_state> (classical rescale)]\n";
    outstream << std::fixed << std::setprecision(14);

    for (std::size_t i_state {0}; i_state < (1UL << args.n_ancilla_qubits); ++i_state) {
        // project it against the provided binary register
        const auto ancilla_bitstring = ket::state_index_to_bitstring_big_endian(i_state, args.n_ancilla_qubits);
        const auto ancilla_bitset = ket::bitstring_to_dynamic_bitset(ancilla_bitstring);
        const auto projected = ket::project_statevector(statevector, ancilla_qubit_indices, ancilla_bitset);

        auto expvalue = expectation_value(hamiltonian, projected);
        
        if (std::fabs(expvalue.imag()) > 1.0e-6) {
            const auto msg = std::format("Found eigenvalue with a non-zero imaginary component: {}, {}", expvalue.real(), expvalue.imag());
            throw std::runtime_error {msg};
            std::exit(EXIT_FAILURE);  // NOLINT(concurrency-mt-unsafe)
        }

        outstream << ancilla_bitstring << "   " << expvalue.real() << '\n';
    }

    return 0;
}

