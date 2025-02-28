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
#include "mini-qiskit/circuit.hpp"
#include "mini-qiskit/common/mathtools.hpp"
#include "mini-qiskit/common/utils.hpp"
#include "mini-qiskit/gate.hpp"
#include "mini-qiskit/state.hpp"

/*
    This file contains code components to perform measurements of the state.
*/

namespace impl_mqis
{

inline auto get_prng_(std::optional<int> seed) -> std::mt19937
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

inline auto perform_measurements_as_counts(
    const std::vector<double>& probabilities_raw,
    std::size_t n_shots,
    std::optional<int> seed = std::nullopt
) -> std::unordered_map<std::string, std::size_t>
{
    if (!impl_mqis::is_power_of_2(probabilities_raw.size())) {
        throw std::runtime_error {
            "The number of probabilities must be a power of 2 to correspond to valid qubit counts."};
    }
    const auto n_qubits = impl_mqis::log_2_int(probabilities_raw.size());

    auto sampler = impl_mqis::ProbabilitySampler_ {probabilities_raw, seed};
    auto measurements = std::unordered_map<std::string, std::size_t> {};

    // REMINDER: if the entry does not exist, `std::unordered_map` will first initialize it to 0
    for (std::size_t i_shot {0}; i_shot < n_shots; ++i_shot) {
        const auto state = sampler();
        const auto bitstring = state_as_bitstring(state, n_qubits);
        ++measurements[bitstring];
    }

    return measurements;
}

inline auto perform_measurements_as_counts(
    const QuantumState& state,
    std::size_t n_shots,
    const QuantumNoise* noise = nullptr,
    std::optional<int> seed = std::nullopt
) -> std::unordered_map<std::string, std::size_t>
{
    const auto probabilities_raw = calculate_probabilities_raw(state, noise);
    return perform_measurements_as_counts(probabilities_raw, n_shots, seed);
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
    const std::vector<std::uint8_t>& measure_bitmask,
    std::optional<int> seed = std::nullopt
) -> std::unordered_map<std::string, std::size_t>
{
    if (!impl_mqis::is_power_of_2(probabilities_raw.size())) {
        throw std::runtime_error {
            "The number of probabilities must be a power of 2 to correspond to valid qubit counts."};
    }
    const auto n_qubits = impl_mqis::log_2_int(probabilities_raw.size());

    if (measure_bitmask.size() != n_qubits) {
        throw std::runtime_error {"The length of the marginal bitmask must match the number of qubits."};
    }

    const auto bit_is_set = [](std::uint8_t bit) { return bit == 1; };
    if (!std::any_of(measure_bitmask.begin(), measure_bitmask.end(), bit_is_set)) {
        throw std::runtime_error {"No measurement gates have been added."};
    }

    auto sampler = impl_mqis::ProbabilitySampler_ {probabilities_raw, seed};
    auto measurements = std::unordered_map<std::string, std::size_t> {};

    // REMINDER: if the entry does not exist, `std::unordered_map` will first initialize it to 0
    for (std::size_t i_shot {0}; i_shot < n_shots; ++i_shot) {
        const auto state = sampler();
        const auto bitstring = impl_mqis::state_as_bitstring_little_endian_marginal_(state, measure_bitmask);
        ++measurements[bitstring];
    }

    return measurements;
}

inline auto perform_measurements_as_counts_marginal(
    const QuantumState& state,
    std::size_t n_shots,
    const std::vector<std::uint8_t>& measure_bitmask,
    const QuantumNoise* noise = nullptr,
    std::optional<int> seed = std::nullopt
) -> std::unordered_map<std::string, std::size_t>
{
    const auto probabilities_raw = calculate_probabilities_raw(state, noise);
    return perform_measurements_as_counts_marginal(probabilities_raw, n_shots, measure_bitmask, seed);
}

inline auto perform_measurements_as_counts_marginal(
    const QuantumCircuit& circuit,
    const QuantumState& state,
    std::size_t n_shots,
    const QuantumNoise* noise = nullptr,
    std::optional<int> seed = std::nullopt
) -> std::unordered_map<std::string, std::size_t>
{
    const auto probabilities_raw = calculate_probabilities_raw(state, noise);
    return perform_measurements_as_counts_marginal(probabilities_raw, n_shots, circuit.measure_bitmask(), seed);
}

}  // namespace mqis
