#include <iostream>

#include <kettle/kettle.hpp>


auto main() -> int
{
    auto circuit = ket::QuantumCircuit {4};
    ket::apply_forward_fourier_transform(circuit, {0, 1, 3});
    ket::apply_inverse_fourier_transform(circuit, {0, 1, 3});

    auto state = ket::QuantumState {"0000"};

    ket::simulate(circuit, state);

    for (std::size_t i {0}; i < 16; ++i) {
        std::cout << state[i].real() << ", " << state[i].imag() << '\n';
    }

    return 0;
}
