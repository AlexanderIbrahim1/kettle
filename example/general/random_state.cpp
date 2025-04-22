#include <complex>
#include <iostream>

#include <kettle/kettle.hpp>


auto main() -> int
{
    auto state = ket::generate_random_state(2);
    ket::print_state(state);

    auto norm_squared = double {0.0};
    for (std::size_t i {0}; i < state.n_states(); ++i) {
        std::cout << std::norm(state[i]) << '\n';
        norm_squared += std::norm(state[i]);
    }

    std::cout << "norm_squared = " << norm_squared << '\n';

    return 0;
}
