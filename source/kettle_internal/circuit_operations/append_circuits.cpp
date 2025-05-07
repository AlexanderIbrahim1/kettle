#include <stdexcept>
#include "kettle/circuit/circuit.hpp"

namespace
{

void check_matching_number_of_qubits_(const ket::QuantumCircuit& left, const ket::QuantumCircuit& right)
{
    if (left.n_qubits() != right.n_qubits()) {
        throw std::runtime_error {"Cannot append circuits with different numbers of qubits."};
    }
}

void check_matching_number_of_bits_(const ket::QuantumCircuit& left, const ket::QuantumCircuit& right)
{
    if (left.n_bits() != right.n_bits()) {
        throw std::runtime_error {"Cannot append circuits with different numbers of bits."};
    }
}

}  // namespace

namespace ket
{
    
void extend_circuit(QuantumCircuit& left, const QuantumCircuit& right)
{
    check_matching_number_of_qubits_(left, right);
    check_matching_number_of_bits_(left, right);

    const auto n_new_elements = left.elements_.size() + right.elements_.size();
    left.elements_.reserve(n_new_elements);
    left.elements_.insert(left.elements_.end(), right.elements_.begin(), right.elements_.end());
}

auto append_circuits(QuantumCircuit left, const QuantumCircuit& right) -> QuantumCircuit
{
    extend_circuit(left, right);
    return left;
}


}  // namespace ket
