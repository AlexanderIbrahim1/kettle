#include <stdexcept>
#include <string>
#include <map>
#include <vector>

#include "kettle/state/state.hpp"
#include "kettle/state/qubit_state_conversion.hpp"

#include "kettle/calculations/probabilities.hpp"

#include "kettle_internal/calculations/probabilities_internal.hpp"
#include "kettle_internal/simulation/gate_pair_generator.hpp"

/*
    This file contains code components to calculate the probabilities of each of
    the individual computational states in the overall QuantumState object.

    It is also possible to add noise to the measurements.
*/

namespace ket
{

QuantumNoise::QuantumNoise(std::size_t n_qubits)
    : n_qubits_ {n_qubits}
    , noise_(n_qubits, 0.0)
{}

void QuantumNoise::set(std::size_t index, double noise)
{
    check_index_(index);
    ket::internal::check_noise_value_(noise);
    noise_[index] = noise;
}

auto QuantumNoise::get(std::size_t index) const -> const double&
{
    check_index_(index);
    return noise_[index];
}

void QuantumNoise::check_index_(std::size_t index) const
{
    if (index >= n_qubits_) {
        throw std::runtime_error {"ERROR: Out-of-bounds access for QuantumNoise probability."};
    }
}

auto calculate_probabilities_raw(const QuantumState& state, const QuantumNoise* noise)
    -> std::vector<double>
{
    const auto n_states = state.n_states();
    const auto n_qubits = state.n_qubits();

    auto probabilities = std::vector<double> {};
    probabilities.reserve(n_states);

    for (std::size_t i_state {0}; i_state < n_states; ++i_state) {
        const auto prob = std::norm(state[i_state]);
        probabilities.push_back(prob);
    }

    if (noise != nullptr) {
        for (std::size_t i_qubit {0}; i_qubit < n_qubits; ++i_qubit) {
            const auto prob_noise = noise->get(i_qubit);
            ket::internal::apply_noise_(prob_noise, i_qubit, n_qubits, probabilities);
        }
    }

    return probabilities;
}

auto calculate_probabilities(const QuantumState& state, const QuantumNoise* noise)
    -> std::map<std::string, double>
{
    const auto n_states = state.n_states();
    const auto n_qubits = state.n_qubits();
    auto probabilities = std::map<std::string, double> {};

    // the internal layout of the quantum state is little endian, so the probabilities are as well
    const auto endian = ket::QuantumStateEndian::LITTLE;

    // applying noise involves generating the indices of pairs of states, and this is much more convenient
    // when done with indices rather than strings; so the downsides of using twice the memory don't seem
    // that bad
    if (noise != nullptr) {
        const auto probabilities_raw = calculate_probabilities_raw(state, noise);

        for (std::size_t i_state {0}; i_state < n_states; ++i_state) {
            const auto bitstring = state_index_to_bitstring(i_state, n_qubits, endian);
            probabilities[bitstring] = probabilities_raw[i_state];
        }
    }
    else {
        for (std::size_t i_state {0}; i_state < n_states; ++i_state) {
            const auto prob = std::norm(state[i_state]);
            const auto bitstring = state_index_to_bitstring(i_state, n_qubits, endian);
            probabilities[bitstring] = prob;
        }
    }

    return probabilities;
}

}  // namespace ket

namespace ket::internal
{

void apply_noise_(double noise, std::size_t i_qubit, std::size_t n_qubits, std::vector<double>& probabilities)
{
    auto generator = ket::internal::SingleQubitGatePairGenerator {i_qubit, n_qubits};
    for (std::size_t i_pair {0}; i_pair < generator.size(); ++i_pair) {
        const auto [state0_index, state1_index] = generator.next();

        const auto current_prob0 = probabilities[state0_index];
        const auto current_prob1 = probabilities[state1_index];
        const auto new_prob0 = ((1.0 - noise) * current_prob0) + (noise * current_prob1);
        const auto new_prob1 = ((1.0 - noise) * current_prob1) + (noise * current_prob0);

        probabilities[state0_index] = new_prob0;
        probabilities[state1_index] = new_prob1;
    }
}

/*
    Ensures that the noise parameter lies in [0.0, 1.0]; otherwise, the noise application is invalid.
*/
void check_noise_value_(double value)
{
    const auto between_0_and_1 = [](double x) { return 0.0 <= x && x <= 1.0; };

    if (!between_0_and_1(value)) {
        throw std::runtime_error {"ERROR: Cannot set probability in QuantumNoise object outside of [0, 1]."};
    }
}

}  // namespace ket::internal
