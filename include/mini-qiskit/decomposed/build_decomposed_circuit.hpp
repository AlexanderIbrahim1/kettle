#pragma once

#include <cstddef>
#include <vector>

#include "mini-qiskit/circuit.hpp"
#include "mini-qiskit/circuit_operations/append_circuits.hpp"
#include "mini-qiskit/circuit_operations/make_controlled_circuit.hpp"
#include "mini-qiskit/decomposed/decomposed_gate.hpp"

namespace impl_mqis
{

inline auto all_indices_between_except(
    std::size_t lower,
    std::size_t upper,
    std::size_t omitted
) -> std::vector<std::size_t>
{
    const auto size = (upper - lower) - 1;
    auto output = std::vector<std::size_t> {};
    output.reserve(size);

    for (std::size_t i {lower}; i < upper; ++i) {
        if (i != omitted) {
            output.push_back(i);
        }
    }

    return output;
}

}  // namespace impl_mqis

namespace mqis
{

inline auto make_circuit_from_decomposed_gates(const std::vector<DecomposedGateInfo>& gates) -> QuantumCircuit
{
    using DGS = DecomposedGateState;
    const auto circuit_size = decomposed_circuit_size(gates);

    auto circuit = QuantumCircuit {circuit_size};

    for (const auto& gate : gates) {
        switch (gate.state)
        {
            case DGS::SINGLEGATE : {
                circuit.add_u_gate(gate.matrix, gate.qubit_index);
                break;
            }
            case DGS::ALLCONTROL : {
                auto control_qubits = impl_mqis::all_indices_between_except(0, circuit_size, gate.qubit_index);
                auto subcircuit = QuantumCircuit {1};
                subcircuit.add_u_gate(gate.matrix, 0);
                auto controlled_circuit = make_multiplicity_controlled_circuit(subcircuit, circuit_size, control_qubits, {gate.qubit_index});
                extend_circuit(circuit, controlled_circuit);
                break;
            }
            default : {
                throw std::runtime_error {"UNREACHABLE: dev error, invalid DecomposedGateState found."};
            }
        }
    }

    return circuit;
}

}  // namespace mqis
