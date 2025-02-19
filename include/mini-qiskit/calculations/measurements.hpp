#pragma once

#include <algorithm>
#include <cstddef>
#include <optional>
#include <random>
#include <unordered_map>
#include <vector>

#include "mini-qiskit/calculations/probabilities.hpp"
#include "mini-qiskit/circuit.hpp"
#include "mini-qiskit/common/mathtools.hpp"
#include "mini-qiskit/gate.hpp"

/*
    This file contains code components to perform measurements of the state.
*/

namespace impl_mqis
{

inline auto get_prng(std::optional<int> seed) -> std::mt19937
{
    if (seed) {
        const auto seed_val = static_cast<std::mt19937::result_type>(seed.value());
        return std::mt19937 {seed_val};
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
    Check that each qubit is measured once and only once during the circuit.
*/
inline auto is_circuit_measurable(const QuantumCircuit& circuit) -> bool
{
    auto measurement_flags = std::vector<std::uint64_t>(circuit.n_qubits(), 0);

    for (const auto& gate : circuit) {
        if (gate.gate == Gate::M) {
            [[maybe_unused]] const auto [qubit_index, ignore] = impl_mqis::unpack_m_gate(gate);
            measurement_flags[qubit_index] += 1;
        }
    }

    const auto equals_one = [](std::uint64_t x) { return x == 1; };

    return std::all_of(measurement_flags.begin(), measurement_flags.end(), equals_one);
}

/*
    Performs measurements of the QuantumState using its probabilities. The measurements
    are in the form of a vector of indices, each of which indicates the computational state
    that the overall QuantumState collapsed to.

    Measurements are performed by first calculating the cumulative probability distribution
    over the states, and sampling with a uniform distribution. This is faster than the method
    used by the reference implementation of MicroQiskit, but takes more memory.

    This method (n = number of qubits, k = number of shots)
      - memory complexity: O(max(2^n, k))
      - time complexity: O(max(2^n, k*n))
        - reason: `std::lower_bound()` has a time complexity of log(2^n) = n, and we do it `k` times

    Reference MicroQiskit
      - memory complexity: O(max(2^n, k))
      - time complexity: O(k * 2^n)
*/
inline auto perform_measurements(
    const std::vector<double>& probabilities,
    std::size_t n_shots,
    std::optional<int> seed = std::nullopt
) -> std::vector<std::size_t>
{
    const auto cumulative = impl_mqis::calculate_cumulative_sum(probabilities);

    const auto max_prob = cumulative.back();
    const auto offset = impl_mqis::cumulative_end_offset(cumulative);
    auto uniform_dist = std::uniform_real_distribution<double> {0.0, max_prob - offset};

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

        const auto i_state = static_cast<std::size_t>(std::distance(cumulative.begin(), it_state));

        measurements.push_back(i_state);
    }

    return measurements;
}

inline auto measurements_to_counts(const std::vector<std::size_t>& measurements)
    -> std::unordered_map<std::size_t, std::size_t>
{
    auto map = std::unordered_map<std::size_t, std::size_t> {};

    // REMINDER: if the entry does not exist, `std::unordered_map` will first initialize it to 0
    for (auto i_state : measurements) {
        ++map[i_state];
    }

    return map;
}

inline auto measurements_to_fractions(const std::vector<std::size_t>& measurements)
    -> std::unordered_map<std::size_t, double>
{
    auto map = std::unordered_map<std::size_t, double> {};

    // REMINDER: if the entry does not exist, `std::unordered_map` will first initialize it to 0
    for (auto i_state : measurements) {
        map[i_state] += 1.0;
    }

    const auto n_measurements = static_cast<double>(measurements.size());
    for (auto& pair : map) {
        map[pair.first] /= n_measurements;
    }

    return map;
}

}  // namespace mqis
