#pragma once

#include <algorithm>
#include <cstddef>
#include <optional>
#include <random>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "mini-qiskit/calculations/probabilities.hpp"
#include "mini-qiskit/circuit/circuit.hpp"
#include "mini-qiskit/common/mathtools.hpp"
#include "mini-qiskit/common/prng.hpp"
#include "mini-qiskit/common/utils.hpp"
#include "mini-qiskit/primitive_gate.hpp"
#include "mini-qiskit/simulation/simulate.hpp"
#include "mini-qiskit/state/state.hpp"

// TODO: remove
#include "mini-qiskit/common/print.hpp"

/*
    This file contains code components to perform measurements of the state.
*/

namespace impl_mqis
{

class ProbabilitySampler_
{
public:
    ProbabilitySampler_(const std::vector<double>& probabilities, std::optional<int> seed = std::nullopt)
        : cumulative_ {calculate_cumulative_sum(probabilities)}
        , prng_ {get_prng_(seed)}
    {
        const auto max_prob = cumulative_.back();
        const auto offset = impl_mqis::cumulative_end_offset(cumulative_);
        uniform_dist_ = std::uniform_real_distribution<double> {0.0, max_prob - offset};
    }

    auto operator()() -> std::size_t
    {
        const auto prob = uniform_dist_(prng_);

        const auto it_state = std::lower_bound(cumulative_.begin(), cumulative_.end(), prob);

        if (it_state == cumulative_.end()) {
            throw std::runtime_error {
                "LOGIC BUG: Ended up with measurement of state past end of cumulative\n"
                "probability distribution, which shouldn't happen?"};
        }

        const auto i_state = static_cast<std::size_t>(std::distance(cumulative_.begin(), it_state));

        return i_state;
    }

private:
    std::vector<double> cumulative_;
    std::mt19937 prng_;
    std::uniform_real_distribution<double> uniform_dist_;
};

inline auto build_marginal_bitmask_(
    const std::vector<std::size_t>& marginal_qubits,
    std::size_t n_qubits
) -> std::vector<std::uint8_t>
{
    const auto is_in_range = [&](auto i) { return i >= n_qubits; };
    if (std::any_of(marginal_qubits.begin(), marginal_qubits.end(), is_in_range))
    {
        throw std::runtime_error {"ERROR: marginal qubit index out of range."};
    }

    auto marginal_bitmask = std::vector<std::uint8_t>(n_qubits, 0);
    for (auto index : marginal_qubits) {
        marginal_bitmask[index] = 1;
    }

    return marginal_bitmask;
}

}  // namespace impl_mqis

namespace mqis
{

inline auto memory_to_counts(const std::vector<std::size_t>& measurements)
    -> std::unordered_map<std::size_t, std::size_t>
{
    auto map = std::unordered_map<std::size_t, std::size_t> {};

    // REMINDER: if the entry does not exist, `std::unordered_map` will first initialize it to 0
    for (auto i_state : measurements) {
        ++map[i_state];
    }

    return map;
}

inline auto memory_to_fractions(const std::vector<std::size_t>& measurements) -> std::unordered_map<std::size_t, double>
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
inline auto perform_measurements_as_memory(
    const std::vector<double>& probabilities_raw,
    std::size_t n_shots,
    std::optional<int> seed = std::nullopt
) -> std::vector<std::size_t>
{
    auto sampler = impl_mqis::ProbabilitySampler_ {probabilities_raw, seed};

    auto measurements = std::vector<std::size_t> {};
    measurements.reserve(n_shots);

    for (std::size_t i_shot {0}; i_shot < n_shots; ++i_shot) {
        const auto i_state = sampler();
        measurements.push_back(i_state);
    }

    return measurements;
}

inline auto perform_measurements_as_memory(
    const QuantumState& state,
    std::size_t n_shots,
    const QuantumNoise* noise = nullptr,
    std::optional<int> seed = std::nullopt
) -> std::vector<std::size_t>
{
    const auto probabilities_raw = calculate_probabilities_raw(state, noise);
    return perform_measurements_as_memory(probabilities_raw, n_shots, seed);
}

inline auto perform_measurements_as_counts_raw(
    const std::vector<double>& probabilities_raw,
    std::size_t n_shots,
    std::optional<int> seed = std::nullopt
) -> std::unordered_map<std::size_t, std::size_t>
{
    auto sampler = impl_mqis::ProbabilitySampler_ {probabilities_raw, seed};
    auto measurements = std::unordered_map<std::size_t, std::size_t> {};

    // REMINDER: if the entry does not exist, `std::unordered_map` will first initialize it to 0
    for (std::size_t i_shot {0}; i_shot < n_shots; ++i_shot) {
        const auto i_state = sampler();
        ++measurements[i_state];
    }

    return measurements;
}

inline auto perform_measurements_as_counts_raw(
    const QuantumState& state,
    std::size_t n_shots,
    const QuantumNoise* noise = nullptr,
    std::optional<int> seed = std::nullopt
) -> std::unordered_map<std::size_t, std::size_t>
{
    const auto probabilities_raw = calculate_probabilities_raw(state, noise);
    return perform_measurements_as_counts_raw(probabilities_raw, n_shots, seed);
}

inline auto perform_measurements_as_counts_marginal(
    const std::vector<double>& probabilities_raw,
    std::size_t n_shots,
    const std::vector<std::size_t>& marginal_qubits = {},
    std::optional<int> seed = std::nullopt
) -> std::unordered_map<std::string, std::size_t>
{
    if (!impl_mqis::is_power_of_2(probabilities_raw.size())) {
        throw std::runtime_error {"The number of probabilities must be a power of 2.\n"};
    }

    const auto n_qubits = impl_mqis::log_2_int(probabilities_raw.size());
    const auto marginal_bitmask = impl_mqis::build_marginal_bitmask_(marginal_qubits, n_qubits);

    auto sampler = impl_mqis::ProbabilitySampler_ {probabilities_raw, seed};
    auto measurements = std::unordered_map<std::string, std::size_t> {};

    // REMINDER: if the entry does not exist, `std::unordered_map` will first initialize it to 0
    for (std::size_t i_shot {0}; i_shot < n_shots; ++i_shot) {
        const auto state = sampler();
        const auto bitstring = impl_mqis::state_as_bitstring_little_endian_marginal_(state, marginal_bitmask);
        ++measurements[bitstring];
    }

    return measurements;
}

inline auto perform_measurements_as_counts_marginal(
    const QuantumState& state,
    std::size_t n_shots,
    const std::vector<std::size_t>& marginal_qubits = {},
    const QuantumNoise* noise = nullptr,
    std::optional<int> seed = std::nullopt
) -> std::unordered_map<std::string, std::size_t>
{
    const auto probabilities_raw = calculate_probabilities_raw(state, noise);
    return perform_measurements_as_counts_marginal(probabilities_raw, n_shots, marginal_qubits, seed);
}

inline auto perform_measurements_as_counts_marginal(
    const QuantumCircuit& circuit,
    const QuantumState& original_state,
    std::size_t n_shots,
    const std::vector<std::size_t>& marginal_qubits = {},
    const QuantumNoise* noise = nullptr,
    std::optional<int> seed = std::nullopt
) -> std::unordered_map<std::string, std::size_t>
{
    const auto n_qubits = circuit.n_qubits();
    const auto marginal_bitmask = impl_mqis::build_marginal_bitmask_(marginal_qubits, n_qubits);

    auto measurements = std::unordered_map<std::string, std::size_t> {};

    for (std::size_t i {0}; i < n_shots; ++i) {
        auto state = original_state;
        mqis::simulate(circuit, state);

        const auto probabilities_raw = calculate_probabilities_raw(state, noise);
        auto sampler = impl_mqis::ProbabilitySampler_ {probabilities_raw, seed};

        const auto i_state = sampler();
        const auto bitstring = impl_mqis::state_as_bitstring_little_endian_marginal_(i_state, marginal_bitmask);
        ++measurements[bitstring];
    }

    return measurements;
}

}  // namespace mqis
