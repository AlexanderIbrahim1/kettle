#include <cstddef>
#include <iostream>
#include <sstream>

#include <mini-qiskit/mini-qiskit.hpp>


auto main() -> int
{
    // create the circuit and populate it with gates
    auto circuit = mqis::QuantumCircuit {3};
    circuit.add_h_gate({0, 1, 2});
    circuit.add_x_gate({0});
    circuit.add_y_gate({1, 2});
    circuit.add_rx_gate({{0, M_PI_4}, {2, M_PI_2}});

    // create the statevector; then
    // perform the simulation to make the state non-trivial
    auto state = mqis::QuantumState {3};
    mqis::simulate(circuit, state);

    // save the statevector to either a file or an output stream
    auto stream = std::stringstream {};
    mqis::save_statevector(stream, state);

    std::cout << stream.str() << '\n';

    return 0;
}
