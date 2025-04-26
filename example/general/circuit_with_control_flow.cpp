#include <kettle/kettle.hpp>

/*
    This example shows how control flow statements, which depend on the measured values of
    classical bits, are created in kettle.
*/

auto x_and_x_subcircuit() -> ket::QuantumCircuit
{
    auto circ = ket::QuantumCircuit {3};
    circ.add_x_gate({0, 2});
    return circ;
}

auto x_and_h_subcircuit() -> ket::QuantumCircuit
{
    auto circ = ket::QuantumCircuit {3};
    circ.add_x_gate(1);
    circ.add_h_gate(2);
    return circ;
}

auto cx_and_h_subcircuit() -> ket::QuantumCircuit
{
    auto circ = ket::QuantumCircuit {3};
    circ.add_cx_gate(1, 2);
    circ.add_h_gate(2);
    return circ;
}

auto main() -> int
{
    // create the quantum circuit, and add some gates unrelated to control flow
    auto circuit = ket::QuantumCircuit {3};
    circuit.add_x_gate({0, 1});
    circuit.add_h_gate({0, 1, 2});

    // perform measurements on qubits 0 and 1
    circuit.add_m_gate({0, 1});

    // create a branch that depends on the measured value of qubit 0
    circuit.add_if_statement(0, x_and_x_subcircuit());

    // create some non-control-flow gates
    circuit.add_y_gate(0);
    circuit.add_z_gate(1);

    circuit.add_if_else_statement(1, x_and_h_subcircuit(), cx_and_h_subcircuit());

    // create some non-control-flow gates
    circuit.add_y_gate(0);
    circuit.add_z_gate(1);

    ket::print_tangelo_circuit(circuit);

    return 0;
}
