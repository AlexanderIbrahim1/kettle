#pragma once

#include <stdexcept>
#include <utility>
#include <vector>

#include "mini-qiskit/common/mathtools.hpp"
#include "mini-qiskit/gate_pair_generator.hpp"
#include "mini-qiskit/state.hpp"

/*
    This file contains code components to calculate the probabilities of each of
    the individual computational states in the overall QuantumState object.

    It is also possible to add noise to the measurements.
*/

namespace impl_mqis
{

constexpr void apply_noise(double noise, std::size_t i_qubit, std::size_t n_qubits, std::vector<double>& probabilities)
{
    auto generator = mqis::SingleQubitGatePairGenerator {i_qubit, n_qubits};
    for (std::size_t i_pair {0}; i_pair < generator.size(); ++i_pair) {
        const auto [state0_index, state1_index] = generator.next();

        const auto current_prob0 = probabilities[state0_index];
        const auto current_prob1 = probabilities[state1_index];
        const auto new_prob0 = (1.0 - noise) * current_prob0 + noise * current_prob1;
        const auto new_prob1 = (1.0 - noise) * current_prob1 + noise * current_prob0;

        probabilities[state0_index] = new_prob0;
        probabilities[state1_index] = new_prob1;
    }
}

}  // namespace impl_mqis

namespace mqis
{

/*
    The QuantumNoise class holds the noise applied to the probabilities calculated
    from the QuantumState object.

    This is basically a thin wrapper around a `std::vector<double>` that checks if
    the noise satisfies certain conditions.
*/
class QuantumNoise
{
public:
    QuantumNoise(std::size_t n_qubits)
        : n_qubits_ {n_qubits}
        , noise_(n_qubits, 0.0)
    {}

    constexpr auto set(std::size_t index, double noise) -> double&
    {
        check_index_(index);
        check_noise_value_(noise);
        noise_[index] = noise;
    }

    constexpr auto get(std::size_t index) const -> const double&
    {
        check_index_(index);
        return noise_[index];
    }

private:
    std::size_t n_qubits_;
    std::vector<double> noise_ {};

    constexpr void check_noise_value_(double value) const
    {
        const auto between_0_and_1 = [](double x) { return 0.0 <= x && x <= 1.0; };

        if (!between_0_and_1(value)) {
            throw std::runtime_error {"ERROR: Cannot set probability in QuantumNoise object outside of [0, 1]."};
        }
    }

    constexpr void check_index_(std::size_t index) const
    {
        if (index >= n_qubits_) {
            throw std::runtime_error {"ERROR: Out-of-bounds access for QuantumNoise probability."};
        }
    }
};

constexpr auto calculate_probabilities(const QuantumState& state, const QuantumNoise* noise) noexcept
    -> std::vector<double>
{
    const auto n_states = state.n_states();
    const auto n_qubits = state.n_qubits();

    auto probabilities = std::vector<double> {};
    probabilities.reserve(n_states);

    for (std::size_t i_state {0}; i_state < n_states; ++i_state) {
        const auto& coeff = state[i_state];
        const auto prob = impl_mqis::norm_squared(coeff.real, coeff.imag);
        probabilities.push_back(prob);
    }

    if (noise) {
        for (std::size_t i_qubit {0}; i_qubit < n_qubits; ++i_qubit) {
            const auto prob_noise = noise->get(i_qubit);
            impl_mqis::apply_noise(prob_noise, i_qubit, n_qubits, probabilities);
        }
    }

    return probabilities;
}

}  // namespace mqis
