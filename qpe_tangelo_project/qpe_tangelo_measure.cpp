#include "kettle/io/statevector.hpp"
#include "kettle/state/endian.hpp"
#include <filesystem>
#include <stdexcept>

#include <kettle/kettle.hpp>
#include <kettle/state/project_state.hpp>

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
            std::exit(EXIT_FAILURE);  // NOLINT(concurrency-mt-unsafe)
        }
    }();

    const auto n_total_qubits = arguments.n_ancilla_qubits + arguments.n_unitary_qubits;

    auto marginal_qubits = ket::arange(0UL, arguments.n_unitary_qubits);

    auto statevector = ket::load_statevector(arguments.abs_statevector_filepath);
    auto circuit = ket::QuantumCircuit {n_total_qubits};

    ket::simulate(circuit, statevector);

    // const auto counts = ket::perform_measurements_as_counts_marginal(statevector, 1UL << 20, marginal_qubits);

    // create the original eigenstate
    const auto initial_circuit = ket::read_tangelo_circuit(
        6,
        std::filesystem::path {"/home/a68ibrah/research/qpe_dipolar_planar_rotors/app/make_gates/rotors_2_ancilla_7_g_0.55_classical/initial_circuit.dat"},
        0
    );

    auto eigenstatevector = ket::QuantumState {6};
    ket::simulate(initial_circuit, eigenstatevector);

    const auto qubit_indices = ket::arange(arguments.n_unitary_qubits, n_total_qubits);

//    for (std::size_t i_state {0}; i_state < (1UL << arguments.n_ancilla_qubits); ++i_state) {
//        const auto bitstring = ket::state_index_to_bitstring_big_endian(i_state, arguments.n_ancilla_qubits);
//        const auto dyn_bitset = ket::bitstring_to_dynamic_bitset(bitstring);
//
//        const auto prefix = std::string(arguments.n_unitary_qubits, 'x');
//        const auto entry = prefix + bitstring;
//
//        const auto count = [&]() {
//            if (counts.contains(entry)) {
//                return counts.at(entry);
//            } else {
//                return 0UL;
//            }
//        }();
//
//        try {
//            const auto projected = ket::project_statevector(statevector, qubit_indices, dyn_bitset);
//            const auto inner_product_sq = ket::inner_product_norm_squared(eigenstatevector, projected);
//            std::cout << bitstring << ", " << count << ", " << inner_product_sq << '\n';
//        }
//        catch (const std::exception& e) {
//            std::cout << bitstring << ", " << count << ", " << 0.0 << '\n';
//        }
//
//    }

    auto projected = ket::project_statevector(statevector, qubit_indices, {1, 1, 1, 1, 1, 1, 0});
    // auto projected = ket::project_statevector(statevector, qubit_indices, {1, 1, 1, 1, 0, 1, 0, 1, 1});
    // 111101011
    // ket::save_statevector(std::cout, projected, ket::QuantumStateEndian::BIG);
    // ket::save_statevector(std::cout, eigenstatevector, ket::QuantumStateEndian::BIG);

    const auto reversing_circuit = ket::read_tangelo_circuit(
        6,
        std::filesystem::path {"/home/a68ibrah/research/qpe_dipolar_planar_rotors/app/plot_qpe/inverse_rotors_2_ancilla_7_g_0.55_classical.reverse"},
        0
    );

    // ket::save_statevector(std::cout, eigenstatevector);
    ket::simulate(reversing_circuit, projected);
    // ket::simulate(reversing_circuit, eigenstatevector);
    ket::save_statevector(std::cout, projected, ket::QuantumStateEndian::BIG);
    // ket::save_statevector(std::cout, eigenstatevector, ket::QuantumStateEndian::BIG);

//    for (const auto& [bitstring, count]: counts) {
//        const auto lstripped_bitstring = ket::lstrip_marginal_bits(bitstring);
//        const auto dyn_bitset = ket::bitstring_to_dynamic_bitset(lstripped_bitstring);
//
//        try {
//            const auto projected = ket::project_statevector(statevector, qubit_indices, dyn_bitset);
//
//            const auto inner_product_sq = ket::inner_product_norm_squared(eigenstatevector, projected);
//
//            std::cout << "(state, count, inner_sq) = (" << bitstring << ", " << count << ", " << inner_product_sq << ")\n";
//        }
//        catch (const std::exception& e) {
//            continue;
//        }
//    }

//     const auto projected = ket::project_statevector(statevector, qubit_indices, {1, 1, 1, 1, 1, 0, 0});
// 
//     const auto inner_product_sq = ket::inner_product_norm_squared(eigenstatevector, projected);
// 
//     std::cout << "inner_product_sq = " << inner_product_sq << '\n';

    // (state, count) = (xxxxxx1110101, 122)
    // (state, count) = (xxxxxx1110110, 169)
    // (state, count) = (xxxxxx1110111, 206)
    // (state, count) = (xxxxxx1111000, 311)
    // (state, count) = (xxxxxx1111001, 496)
    // (state, count) = (xxxxxx1111010, 976)
    // (state, count) = (xxxxxx1111011, 2729)
    // (state, count) = (xxxxxx1111100, 21403)
    // (state, count) = (xxxxxx1111101, 31967)
    // (state, count) = (xxxxxx1111110, 3045)
    // (state, count) = (xxxxxx1111111, 1140)

    return 0;
}

