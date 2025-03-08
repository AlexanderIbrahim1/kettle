#include <iostream>

#include <mini-qiskit/mini-qiskit.hpp>
#include <mini-qiskit/gates/fourier.hpp>


auto main() -> int
{
    auto circuit = mqis::QuantumCircuit {4};
    mqis::apply_forward_fourier_transform(circuit, {0, 1, 3});
    mqis::apply_inverse_fourier_transform(circuit, {0, 1, 3});

    auto state = mqis::QuantumState {"0000"};

    mqis::simulate(circuit, state);

    for (std::size_t i {0}; i < 16; ++i) {
        std::cout << state[i].real() << ", " << state[i].imag() << '\n';
    }

    return 0;
}
