#pragma once

namespace ket
{

class QuantumCircuit;
    
void extend_circuit(QuantumCircuit& left, const QuantumCircuit& right);

auto append_circuits(QuantumCircuit left, const QuantumCircuit& right) -> QuantumCircuit;

}  // namespace ket
