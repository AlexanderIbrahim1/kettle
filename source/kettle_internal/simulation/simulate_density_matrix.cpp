#include <optional>
#include <stdexcept>
#include <vector>

#include <Eigen/Dense>

#include "kettle/circuit/classical_register.hpp"
#include "kettle/circuit/circuit.hpp"
#include "kettle/circuit_loggers/circuit_logger.hpp"
#include "kettle/gates/primitive_gate.hpp"
#include "kettle/simulation/simulate_density_matrix.hpp"
#include "kettle/state/density_matrix.hpp"

#include "kettle_internal/parameter/parameter_expression_internal.hpp"
#include "kettle_internal/simulation/operations_density_matrix.hpp"
#include "kettle_internal/simulation/simulate_utils.hpp"


namespace ki = ket::internal;
namespace kpi = ket::param::internal;

namespace
{

auto simulate_loop_body_iterative_(  // NOLINT(readability-function-cognitive-complexity)
    const ket::QuantumCircuit& circuit,
    ket::DensityMatrix& state,
    const ki::FlatIndexPair<Eigen::Index>& single_pair,
    const ki::FlatIndexPair<Eigen::Index>& double_pair,
    int thread_id,
    std::optional<int> prng_seed,
    ket::ClassicalRegister& cregister,
    Eigen::MatrixXcd& buffer
) -> std::vector<ket::CircuitLogger>
{
    using Elements = std::reference_wrapper<const std::vector<ket::CircuitElement>>;

    auto elements_stack = std::vector<Elements> {};
    elements_stack.push_back(std::ref(circuit.circuit_elements()));

    auto instruction_pointers = std::vector<std::size_t> {};
    instruction_pointers.push_back(0);

    auto circuit_loggers = std::vector<ket::CircuitLogger> {};

    const auto& parameter_values_map = kpi::create_parameter_values_map(circuit.parameter_data_map());

    while (elements_stack.size() != 0) {
        const auto& elements = elements_stack.back();
        const auto i_ptr = instruction_pointers.back();

        ++instruction_pointers.back();

        if (i_ptr >= elements.get().size()) {
            elements_stack.pop_back();
            instruction_pointers.pop_back();
            continue;
        }

        const auto& element = elements.get()[i_ptr];

        if (element.is_circuit_logger()) {
            throw std::runtime_error {"DEV ERROR: loggers haven't been implemented yet for the density matrix simulator.\n"};
            const auto& logger = element.get_circuit_logger();

            if (logger.is_classical_register_circuit_logger()) {
                auto cregister_logger = logger.get_classical_register_circuit_logger();
                cregister_logger.add_classical_register(cregister);
                circuit_loggers.emplace_back(std::move(cregister_logger));
            }
            else if (logger.is_density_matrix_circuit_logger()) {
                auto density_matrix_logger = logger.get_density_matrix_circuit_logger();
                density_matrix_logger.add_density_matrix(state);
                circuit_loggers.emplace_back(std::move(density_matrix_logger));
            }
            else {
                throw std::runtime_error {"DEV ERROR: unimplemented circuit logger in `simulate_loop_body_iterative_()`\n"};
            }
        }
        else if (element.is_control_flow()) {
            const auto& control_flow = element.get_control_flow();

            if (control_flow.is_if_statement()) {
                const auto& if_stmt = control_flow.get_if_statement();

                if (if_stmt(cregister)) {
                    const auto& subcircuit = *if_stmt.circuit();
                    elements_stack.push_back(std::ref(subcircuit.circuit_elements()));
                    instruction_pointers.push_back(0);
                }
            }
            else if (control_flow.is_if_else_statement()) {
                const auto& if_else_stmt = control_flow.get_if_else_statement();

                // NOTE: omitting the return type here causes a dangling reference
                const auto& subcircuit = [&]() -> const ket::QuantumCircuit& {
                    if (if_else_stmt(cregister)) {
                        return *if_else_stmt.if_circuit();
                    } else {
                        return *if_else_stmt.else_circuit();
                    }
                }();

                elements_stack.push_back(std::ref(subcircuit.circuit_elements()));
                instruction_pointers.push_back(0);
            }
            else {
                throw std::runtime_error {"DEV ERROR: unimplemented control flow in `simulate_loop_body_iterative_()`\n"};
            }
        }
        else if (element.is_gate()) {
            const auto gate_info = element.get_gate();

            simulate_gate_info_(
                parameter_values_map,
                state,
                single_pair,
                double_pair,
                gate_info,
                thread_id,
                prng_seed,
                cregister,
                buffer
            );
        }
        else {
            throw std::runtime_error {"DEV ERROR: unimplemented circuit element in `simulate_loop_body_iterative_()`\n"};
        }
    }

    return circuit_loggers;
}

void check_valid_number_of_qubits_(const ket::QuantumCircuit& circuit, const ket::DensityMatrix& state)
{
    if (circuit.n_qubits() != state.n_qubits()) {
        throw std::runtime_error {"Invalid simulation; circuit and state have different number of qubits."};
    }

    if (circuit.n_qubits() == 0) {
        throw std::runtime_error {"Cannot simulate a circuit or state with zero qubits."};
    }
}

}  // namespace

namespace ket
{

DensityMatrixSimulator::DensityMatrixSimulator(std::size_t n_qubits)
{
    if (n_qubits == 0) {
        throw std::runtime_error {"ERROR: cannot perform a DensityMatrix simulation with 0 qubits.\n"};
    }

    const auto n_states = static_cast<Eigen::Index>(1UL << n_qubits);

    buffer_ = Eigen::MatrixXcd(n_states, n_states);
}

void DensityMatrixSimulator::run(const QuantumCircuit& circuit, DensityMatrix& state, std::optional<int> prng_seed)
{
    namespace ki = ket::internal;

    check_valid_number_of_qubits_(circuit, state);

    const auto n_single_gate_pairs = static_cast<Eigen::Index>(ki::number_of_single_qubit_gate_pairs_(circuit.n_qubits()));
    const auto single_pair = ki::FlatIndexPair<Eigen::Index> {.i_lower=0, .i_upper=n_single_gate_pairs};

    const auto n_double_gate_pairs = static_cast<Eigen::Index>(ki::number_of_double_qubit_gate_pairs_(circuit.n_qubits()));
    const auto double_pair = ki::FlatIndexPair<Eigen::Index> {.i_lower=0, .i_upper=n_double_gate_pairs};

    cregister_ = ket::ClonePtr<ClassicalRegister> {ClassicalRegister {circuit.n_bits()}};

    // the `simulate_loop_body_()` function is used by both the single-threaded and multi-threaded
    // code, and certain operations are only done on the thread with thread id 0
    const auto thread_id = ki::MEASURING_THREAD_ID;

    circuit_loggers_ = simulate_loop_body_iterative_(circuit, state, single_pair, double_pair, thread_id, prng_seed, *cregister_, buffer_);

    has_been_run_ = true;
}

[[nodiscard]]
auto DensityMatrixSimulator::has_been_run() const -> bool
{
    return has_been_run_;
}

[[nodiscard]]
auto DensityMatrixSimulator::classical_register() const -> const ClassicalRegister&
{
    if (!cregister_) {
        throw std::runtime_error {"ERROR: Cannot access classical register; no simulation has been run\n"};
    }

    return *cregister_;
}

auto DensityMatrixSimulator::classical_register() -> ClassicalRegister&
{
    if (!cregister_) {
        throw std::runtime_error {"ERROR: Cannot access classical register; no simulation has been run\n"};
    }

    return *cregister_;
}

[[nodiscard]]
auto DensityMatrixSimulator::circuit_loggers() const -> const std::vector<CircuitLogger>&
{
    return circuit_loggers_;
}

void simulate(const QuantumCircuit& circuit, DensityMatrix& state, std::optional<int> prng_seed)
{
    auto simulator = DensityMatrixSimulator {state.n_qubits()};
    simulator.run(circuit, state, prng_seed);
}


}  // namespace ket
