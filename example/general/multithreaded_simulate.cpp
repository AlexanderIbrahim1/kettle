#include <iostream>

#include <kettle/kettle.hpp>

/*
    This file shows an example of performing a multithreaded simulation.

    WARNING: the current multithreaded implementation is slower than the singlethreaded implementation;
    I'm not sure of the reasons yet (too much waiting at the barrier, multiple states per cache line, etc.)
*/

auto main() -> int
{
    auto circuit = ket::QuantumCircuit {4};
    circuit.add_h_gate({0, 1, 2, 3});
    circuit.add_cx_gate({{0, 1}, {0, 2}});
    circuit.add_x_gate({0, 1, 2, 3});

    auto statevector = ket::QuantumState {"0000"};

    ket::simulate_multithreaded(circuit, statevector, 2);

    const auto counts = ket::perform_measurements_as_counts_marginal(statevector, 1024);

    for (const auto& [bitstring, count] : counts) {
        std::cout << "(state, count) = (" << bitstring << ", " << count << ")\n";
    }

    return 0;
}
