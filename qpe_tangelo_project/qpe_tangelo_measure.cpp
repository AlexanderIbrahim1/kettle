#include <cstddef>
#include <filesystem>
#include <format>
#include <fstream>
#include <stdexcept>

#include <kettle/kettle.hpp>

/*
    This code:
      - loads in the final simulated statevector from the QPE simulations
      - performs projective measurements on the final simulated statevector
      - calculates the inner product between the initial statevector, and the statevector
        recovered by projecting against each binary register string
*/

static constexpr auto N_UNITARY_QUBITS_TWO_ROTOR = std::size_t {6};
static constexpr auto N_UNITARY_QUBITS_THREE_ROTOR = std::size_t {9};

struct CommandLineArguments
{
    CommandLineArguments(int argc, char** argv)
    {
        if (argc != 7) {
            throw std::runtime_error {
                "./a.out n_ancilla_qubits n_rotors abs_init_circuit_dirpath abs_input_dirpath abs_output_dirpath statevector_filename\n"
            };
        }

        const auto arguments = std::vector<std::string> (argv + 1, argv + argc);

        n_ancilla_qubits = std::stoul(arguments[0]);
        const auto n_rotors = std::stoul(arguments[1]);
        abs_init_circuit_dirpath = std::filesystem::path {arguments[2]};
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

        n_total_qubits = n_ancilla_qubits + n_unitary_qubits;
    }

    std::size_t n_ancilla_qubits;
    std::size_t n_unitary_qubits;
    std::size_t n_total_qubits;
    std::filesystem::path abs_init_circuit_dirpath;
    std::filesystem::path abs_input_dirpath;
    std::filesystem::path abs_output_dirpath;
    std::string statevector_filename;
};

auto create_original_state(const std::filesystem::path& abs_initial_circuit_filepath, std::size_t n_unitary_qubits) -> ket::QuantumState
{
    const auto initial_circuit = ket::read_tangelo_circuit(n_unitary_qubits, abs_initial_circuit_filepath, 0);
    auto statevector = ket::QuantumState {n_unitary_qubits};
    ket::simulate(initial_circuit, statevector);

    return statevector;
}

class MapWithDefault
{
public:
    MapWithDefault(std::map<std::string, std::size_t> counts, std::size_t default_value)
        : counts_ {std::move(counts)}
        , default_value_ {default_value}
    {}

    [[nodiscard]]
    auto at(const std::string& entry) const -> std::size_t
    {
        if (counts_.contains(entry)) {
            return counts_.at(entry);
        } else {
            return default_value_;
        }
    }

private:
    std::map<std::string, std::size_t> counts_;
    std::size_t default_value_;
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

    // perform measurements
    auto counts = ket::perform_measurements_as_counts_marginal(statevector, 1UL << 20, ket::arange(0UL, args.n_unitary_qubits));
    const auto counts_wrapper = MapWithDefault {std::move(counts), 0UL};

    const auto ancilla_qubit_indices = ket::arange(args.n_unitary_qubits, args.n_total_qubits);

    // calculate the original statevector to perform inner product calculations against
    const auto abs_init_circuit_filepath = args.abs_init_circuit_dirpath / "initial_circuit.dat";
    const auto original_statevector = create_original_state(abs_init_circuit_filepath, args.n_unitary_qubits);

    const auto output_filepath = args.abs_output_dirpath / std::format("measurements_{}", args.statevector_filename);
    auto outstream = std::ofstream {output_filepath};
    if (!outstream.is_open()) {
        throw std::ios::failure {std::format("ERROR: cannot open output file '{}'", output_filepath.c_str())};
    }

    outstream << "# [projected register bitstring]   [count]   [|<true_ground_state|projected_state>|^2]\n";

    // calculate the count and inner product for each register
    for (std::size_t i_state {0}; i_state < (1UL << args.n_ancilla_qubits); ++i_state) {
        const auto bitstring = ket::state_index_to_bitstring_big_endian(i_state, args.n_ancilla_qubits);

        const auto prefix = std::string(args.n_unitary_qubits, 'x');
        const auto entry = prefix + bitstring;
        const auto count = counts_wrapper.at(entry);

        const auto dyn_bitset = ket::bitstring_to_dynamic_bitset(bitstring);
        const auto projected = ket::project_statevector(statevector, ancilla_qubit_indices, dyn_bitset);
        const auto inner_product_sq = ket::inner_product_norm_squared(original_statevector, projected);
        outstream << bitstring << "   " << count << "   " << inner_product_sq << '\n';
    }

    return 0;
}

