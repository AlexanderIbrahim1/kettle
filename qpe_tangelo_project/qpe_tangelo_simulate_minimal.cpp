#include "kettle/circuit/circuit.hpp"
#include <filesystem>
#include <format>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include <kettle/kettle.hpp>

/*
    Perform QPE for the N = 2 and N = 3 gates for the rotor paper, using the minimal
    output files for the gates of the circuit.
*/

static constexpr auto N_UNITARY_QUBITS_TWO_ROTOR = std::size_t {6};
static constexpr auto N_UNITARY_QUBITS_THREE_ROTOR = std::size_t {9};
static constexpr auto RUN_FROM_START_KEY = int {-1};

struct CommandLineArguments
{
    CommandLineArguments(int argc, char** argv)
    {
        if (argc != 9) {
            throw std::runtime_error {
                "./a.out n_ancilla_qubits n_rotors n_trotter_steps abs_gate_dirpath subcircuit_filename abs_output_dirpath init_state_kind i_continue\n"
            };
        }

        const auto arguments = std::vector<std::string> (argv + 1, argv + argc);

        n_ancilla_qubits = std::stoul(arguments[0]);
        const auto n_rotors = std::stoul(arguments[1]);
        n_trotter_steps = std::stoul(arguments[2]);
        abs_circuits_dirpath = std::filesystem::path {arguments[3]};
        subcircuit_filename = arguments[4];
        abs_output_dirpath = std::filesystem::path {arguments[5]};
        init_state_kind = std::string {arguments[6]};
        i_continue = std::stoi(arguments[7]);

        if (init_state_kind != "true_ground_state" && init_state_kind != "hadamard") {
            auto err_msg = std::stringstream {};
            err_msg << "'init_state_kind' must be 'true_ground_state' or 'hadamard'\n";
            err_msg << "found: " << init_state_kind << '\n';
            throw std::runtime_error {err_msg.str()};
        }

        if (i_continue <= -2) {
            throw std::runtime_error {
                "'i_continue' must be -1 (for running from start) or a non-negative integer"
            };
        }

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
    std::filesystem::path abs_output_dirpath;
    std::string init_state_kind;
    int i_continue;
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

auto statevector_filename(int i) -> std::string
{
    return std::format("statevector.dat{}", i);
}

void simulate_unitary(
    const CommandLineArguments& args,
    ket::QuantumState& statevector,
    std::size_t i_control,
    int& count
)
{
    auto n_powers = 1UL << i_control;
    const auto n_total_qubits = args.n_ancilla_qubits + args.n_unitary_qubits;

    const auto circuit_filepath = [&]() {
        auto output = std::stringstream {};
        output << args.subcircuit_filename << i_control;

        return args.abs_circuits_dirpath / output.str();
    }();

    const auto circuit = ket::read_tangelo_circuit(n_total_qubits, circuit_filepath, 0);

    for (std::size_t i {0}; i < n_powers; ++i) {
        if (args.i_continue != RUN_FROM_START_KEY && count <= args.i_continue) {
            ++count;
            continue;
        }

        for (std::size_t i_trotter_ {0}; i_trotter_ < args.n_trotter_steps; ++i_trotter_) {
            ket::simulate(circuit, statevector);
        }

        ket::save_statevector(args.abs_output_dirpath / statevector_filename(count), statevector);
        ++count;
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
            std::exit(EXIT_FAILURE);  // NOLINT(concurrency-mt-unsafe)
        }
    }();

    const auto n_total_qubits = args.n_ancilla_qubits + args.n_unitary_qubits;

    auto statevector = [&]() {
        if (args.i_continue == RUN_FROM_START_KEY) {
            return ket::QuantumState {n_total_qubits};
        } else {
            return ket::load_statevector(args.abs_output_dirpath / statevector_filename(args.i_continue));
        }
    }();

    // simulate the initial circuit
    if (args.i_continue == RUN_FROM_START_KEY) {
        if (args.init_state_kind == "true_ground_state") {
            simulate_subcircuit(args.abs_circuits_dirpath / "initial_circuit.dat", statevector, n_total_qubits);
        }
        else if (args.init_state_kind == "hadamard") {
            // the |000000> state has much less overlap with the ground state than the uniformly distributed state
            auto hadamard_circuit = ket::QuantumCircuit {n_total_qubits};
            hadamard_circuit.add_h_gate(ket::arange(args.n_unitary_qubits));
            ket::simulate(hadamard_circuit, statevector);
        }
        else {
            throw std::runtime_error {"ERROR: invalid init_state_kind value found.\n"};
        }

        simulate_subcircuit(args.abs_circuits_dirpath / "qft_circuit.dat", statevector, n_total_qubits);
    }

    auto count = int {0};
    for (std::size_t i_control {0}; i_control < args.n_ancilla_qubits; ++i_control) {
        simulate_unitary(args, statevector, i_control, count);
    }

    simulate_subcircuit(args.abs_circuits_dirpath / "iqft_circuit.dat", statevector, n_total_qubits);

    ket::save_statevector(args.abs_output_dirpath / statevector_filename(count), statevector);

    return 0;
}

