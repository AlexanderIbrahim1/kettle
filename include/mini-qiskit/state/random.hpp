#pragma once

#include <algorithm>
#include <cmath>
#include <complex>
#include <cstddef>
#include <numeric>
#include <optional>
#include <random>
#include <vector>

#include "mini-qiskit/common/prng.hpp"
#include "mini-qiskit/state/state.hpp"


namespace mqis
{

inline auto generate_random_state(
    std::size_t n_qubits,
    std::mt19937& prng
) -> mqis::QuantumState
{
    if (n_qubits == 0) {
        throw std::runtime_error {"Cannot generate a quantum state with 0 qubits.\n"};
    }

    const auto n_states = 1ul << n_qubits;
    auto magnitudes = std::vector<double> {};
    magnitudes.reserve(n_states);

    auto magnitude_gen = std::uniform_real_distribution<double> {1.0e-3, 1.0e1};
    auto angle_gen = std::uniform_real_distribution<double> {0.0, 2.0 * M_PI};

    // first, generate the magnitudes of the states
    for (std::size_t i {0}; i < n_states; ++i) {
        magnitudes.push_back(magnitude_gen(prng));
    }

    // then normalize the magnitudes
    const auto sum_of_squares = std::accumulate(
        magnitudes.begin(),
        magnitudes.end(),
        0.0,
        [](auto sum_so_far, auto new_elem) { return sum_so_far + new_elem * new_elem; }
    );

    const auto norm = 1.0 / std::sqrt(sum_of_squares);

    auto amplitudes = std::vector<std::complex<double>> {};
    amplitudes.reserve(n_states);

    for (std::size_t i {0}; i < n_states; ++i) {
        const auto angle = angle_gen(prng);
        const auto coeff = norm * magnitudes[i];

        const auto x = coeff * std::cos(angle);
        const auto y = coeff * std::sin(angle);
        amplitudes.emplace_back(x, y);
    }

    return amplitudes;
}

inline auto generate_random_state(std::size_t n_qubits, int seed) -> mqis::QuantumState
{
    auto prng = impl_mqis::get_prng_(seed);
    return generate_random_state(n_qubits, prng);
}

inline auto generate_random_state(std::size_t n_qubits) -> mqis::QuantumState
{
    // NOTE: creating a single `generate_random_state()` function that takes as an
    // argument `std::optional<int> seed = std::nullopt` causes an infinite self-recursion
    // loop; I guess an instance of `std::mt19937` gets picked up as `std::optional<int>`???
    auto prng = impl_mqis::get_prng_(std::nullopt);
    return generate_random_state(n_qubits, prng);
}

}  // namespace mqis
