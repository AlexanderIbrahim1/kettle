#include <stdexcept>
#include <vector>

#include "kettle/circuit/circuit.hpp"
#include "kettle/circuit_operations/make_binary_controlled_circuit.hpp"
#include "kettle/common/utils.hpp"
#include "kettle_internal/common/utils_internal.hpp"
#include "kettle/circuit_operations/append_circuits.hpp"
#include "kettle/circuit_operations/make_controlled_circuit.hpp"


namespace
{

template <ket::QubitIndices Container>
void check_subcircuit_powers_(
    const std::vector<ket::QuantumCircuit>& subcircuit_powers,
    const Container& control_qubits
)
{
    if (subcircuit_powers.size() != ket::internal::get_container_size(control_qubits)) {
        throw std::runtime_error {"The number of circuits passed does not match the number of control qubits."};
    }
}
template
void check_subcircuit_powers_<ket::QubitIndicesVector>(
    const std::vector<ket::QuantumCircuit>& subcircuit_powers,
    const ket::QubitIndicesVector& control_qubits
);
template
void check_subcircuit_powers_<ket::QubitIndicesIList>(
    const std::vector<ket::QuantumCircuit>& subcircuit_powers,
    const ket::QubitIndicesIList& control_qubits
);

}  // namespace

namespace ket
{

template <QubitIndices Container>
auto make_binary_controlled_circuit_naive(
    const QuantumCircuit& subcircuit,
    std::size_t n_new_qubits,
    const Container& control_qubits,
    const Container& mapped_qubits
) -> ket::QuantumCircuit
{
    auto new_circuit = QuantumCircuit {n_new_qubits};

    const auto size = ket::internal::get_container_size(control_qubits);

    for (std::size_t i {0}; i < size; ++i) {
        const auto control = ket::internal::get_container_index(control_qubits, i);
        const auto n_iterations = 1UL << i;

        for (std::size_t i_iter {0}; i_iter < n_iterations; ++i_iter) {
            const auto controlled_subcircuit = make_controlled_circuit(subcircuit, n_new_qubits, control, mapped_qubits);
            extend_circuit(new_circuit, controlled_subcircuit);
        }
    }

    return new_circuit;
}
template
auto make_binary_controlled_circuit_naive<QubitIndicesVector>(
    const QuantumCircuit& subcircuit,
    std::size_t n_new_qubits,
    const QubitIndicesVector& control_qubits,
    const QubitIndicesVector& mapped_qubits
) -> ket::QuantumCircuit;
template
auto make_binary_controlled_circuit_naive<QubitIndicesIList>(
    const QuantumCircuit& subcircuit,
    std::size_t n_new_qubits,
    const QubitIndicesIList& control_qubits,
    const QubitIndicesIList& mapped_qubits
) -> ket::QuantumCircuit;


template <QubitIndices Container>
auto make_binary_controlled_circuit_from_binary_powers(
    const std::vector<QuantumCircuit>& subcircuit_powers,
    std::size_t n_new_qubits,
    const Container& control_qubits,
    const Container& mapped_qubits
) -> ket::QuantumCircuit
{
    check_subcircuit_powers_(subcircuit_powers, control_qubits);

    auto new_circuit = QuantumCircuit {n_new_qubits};
    const auto size = ket::internal::get_container_size(control_qubits);

    for (std::size_t i {0}; i < size; ++i) {
        const auto control = ket::internal::get_container_index(control_qubits, i);

        const auto& subcircuit = subcircuit_powers[i];
        const auto controlled_subcircuit = make_controlled_circuit(subcircuit, n_new_qubits, control, mapped_qubits);
        extend_circuit(new_circuit, controlled_subcircuit);
    }

    return new_circuit;
}
template
auto make_binary_controlled_circuit_from_binary_powers<QubitIndicesVector>(
    const std::vector<QuantumCircuit>& subcircuit_powers,
    std::size_t n_new_qubits,
    const QubitIndicesVector& control_qubits,
    const QubitIndicesVector& mapped_qubits
) -> ket::QuantumCircuit;
template
auto make_binary_controlled_circuit_from_binary_powers<QubitIndicesIList>(
    const std::vector<QuantumCircuit>& subcircuit_powers,
    std::size_t n_new_qubits,
    const QubitIndicesIList& control_qubits,
    const QubitIndicesIList& mapped_qubits
) -> ket::QuantumCircuit;

}  // namespace ket
