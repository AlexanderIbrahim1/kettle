#include "kettle/state/state.hpp"
#include <kettle/kettle.hpp>
#include <kettle/simulation/statevector_simulator.hpp>

auto main() -> int
{
    // create a quantum circuit with 2 qubit registers, and apply the H and CX gates
    auto circuit = ket::QuantumCircuit {2};
    circuit.add_h_gate(0);
    circuit.add_cx_gate(0, 1);

    // begin with a 2-qubit statevector in the |00> state
    auto statevector = ket::QuantumState {"00"};

    // propagate the state through the circuit, creating the (|00> + |11>) / sqrt(2) state
    auto simulator = ket::StatevectorSimulator {};
    simulator.run(circuit, statevector);

    // perform measurements on this statevector
    const auto counts = ket::perform_measurements_as_counts(statevector, 1024);
    for (const auto& [bitstring, count] : counts) {
        std::cout << "(state, count) = (" << bitstring << ", " << count << ")\n";
    }

    return 0;
}
