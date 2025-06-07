#include <iostream>

#include <kettle/kettle.hpp>


auto main() -> int
{
    auto circuit = ket::QuantumCircuit {4};
    circuit.add_qft_gate({0, 1, 3});
    circuit.add_iqft_gate({0, 1, 3});

    auto state = ket::QuantumState {"0000"};

    ket::simulate(circuit, state);

    for (std::size_t i {0}; i < 16; ++i) {
        std::cout << state[i].real() << ", " << state[i].imag() << '\n';
    }

    return 0;
}
