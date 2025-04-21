#include <complex>
#include <iostream>

#include <mini-qiskit/mini-qiskit.hpp>


auto main() -> int
{
    auto state = mqis::generate_random_state(2);
    mqis::print_state(state);

    auto norm_squared = double {0.0};
    for (std::size_t i {0}; i < state.n_states(); ++i) {
        std::cout << std::norm(state[i]) << '\n';
        norm_squared += std::norm(state[i]);
    }

    std::cout << "norm_squared = " << norm_squared << '\n';

    return 0;
}
