#pragma once

#include <algorithm>
#include <cstddef>
#include <optional>
#include <random>
#include <unordered_map>
#include <vector>

#include "mini-qiskit/circuit.hpp"

/*
    This file contains code components to perform measurements of the state.
*/

namespace impl_mqis
{

constexpr auto calculate_cumulative_sum(const std::vector<double>& probabilities) -> std::vector<double>
{
    auto cumulative = std::vector<double> {};
    cumulative.reserve(probabilities.size());

    std::partial_sum(probabilities.begin(), probabilities.end(), cumulative.begin());

    return cumulative;
}

auto get_prng(std::optional<int> seed) -> std::mt19937
{
    if (seed) {
        return std::mt19937 {seed.value()};
    }
    else {
        auto device = std::random_device {};
        return std::mt19937 {device()};
    }
}

}  // namespace impl_mqis

namespace mqis
{

/*
    Performs measurements of the QuantumState using its probabilities. The measurements
    are in the form of a vector of indices, each of which indicates the computational state
    that the overall QuantumState collapsed to.

    Measurements are performed by first calculating the cumulative probability distribution
    over the states, and sampling with a uniform distribution. This is faster than the method
    used by the reference implementation of MicroQiskit, but takes more memory.

    This method (n = number of qubits, k = number of shots)
      - memory complexity: O(max(2^n, k))
      - time complexity: O(max(2^n, k))

    Reference MicroQiskit
      - memory complexity: O(max(2^n, k))
      - time complexity: O(k * 2^n)
*/
auto perform_measurements(
    const QuantumCircuit& circuit,
    const std::vector<double>& probabilities,
    std::size_t n_shots,
    std::optional<int> seed
) -> std::vector<std::size_t>
{
    const auto cumulative = impl_mqis::calculate_cumulative_sum(probabilities);

    const auto max_prob = cumulative.back();
    auto uniform_dist = std::uniform_real_distribution<double> {0.0, max_prob};

    auto prng = impl_mqis::get_prng(seed);

    auto measurements = std::vector<std::size_t> {};
    measurements.reserve(n_shots);

    for (std::size_t i_shot {0}; i_shot < n_shots; ++i_shot) {
        const auto prob = uniform_dist(prng);

        const auto it_state = std::lower_bound(cumulative.begin(), cumulative.end(), prob);

        if (it_state == cumulative.end()) {
            throw std::runtime_error {
                "LOGIC BUG: Ended up with measurement of state past end of cumulative\n"
                "probability distribution, which shouldn't happen?"};
        }

        measurements.push_back(*it_state);
    }

    return measurements;
}

auto measurements_to_counts(const std::vector<std::size_t>& measurements)
    -> std::unordered_map<std::size_t, std::size_t>
{
    auto map = std::unordered_map<std::size_t, std::size_t> {};

    // REMINDER: if the entry does not exist, `std::unordered_map` will first initialize it
    for (auto i_state : measurements) {
        ++map[i_state];
    }

    return map;
}

}  // namespace mqis
