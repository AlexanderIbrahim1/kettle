#include <algorithm>
#include <cstddef>
#include <optional>
#include <random>
#include <stdexcept>
#include <string>
#include <map>
#include <vector>

#include "kettle/calculations/probabilities.hpp"
#include "kettle/circuit/circuit.hpp"
#include "kettle/common/mathtools.hpp"
#include "kettle/common/prng.hpp"
#include "kettle/simulation/simulate.hpp"
#include "kettle/state/state.hpp"
#include "kettle/state/marginal.hpp"

#include "kettle/calculations/measurements.hpp"

/*
    This file contains code components to perform measurements of the state.
*/

namespace ket
{

auto memory_to_counts(const std::vector<std::size_t>& measurements)
    -> std::map<std::size_t, std::size_t>
{
    auto map = std::map<std::size_t, std::size_t> {};

    // REMINDER: if the entry does not exist, `std::map` will first initialize it to 0
    for (auto i_state : measurements) {
        ++map[i_state];
    }

    return map;
}

auto memory_to_fractions(const std::vector<std::size_t>& measurements) -> std::map<std::size_t, double>
{
    auto map = std::map<std::size_t, double> {};

    // REMINDER: if the entry does not exist, `std::map` will first initialize it to 0
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
auto perform_measurements_as_memory(
    const std::vector<double>& probabilities_raw,
    std::size_t n_shots,
    std::optional<int> seed
) -> std::vector<std::size_t>
{
    auto sampler = ket::internal::ProbabilitySampler_ {probabilities_raw, seed};

    auto measurements = std::vector<std::size_t> {};
    measurements.reserve(n_shots);

    for (std::size_t i_shot {0}; i_shot < n_shots; ++i_shot) {
        const auto i_state = sampler();
        measurements.push_back(i_state);
    }

    return measurements;
}

auto perform_measurements_as_memory(
    const QuantumState& state,
    std::size_t n_shots,
    const QuantumNoise* noise,
    std::optional<int> seed
) -> std::vector<std::size_t>
{
    const auto probabilities_raw = calculate_probabilities_raw(state, noise);
    return perform_measurements_as_memory(probabilities_raw, n_shots, seed);
}

auto perform_measurements_as_counts_raw(
    const std::vector<double>& probabilities_raw,
    std::size_t n_shots,
    std::optional<int> seed
) -> std::map<std::size_t, std::size_t>
{
    auto sampler = ket::internal::ProbabilitySampler_ {probabilities_raw, seed};
    auto measurements = std::map<std::size_t, std::size_t> {};

    // REMINDER: if the entry does not exist, `std::map` will first initialize it to 0
    for (std::size_t i_shot {0}; i_shot < n_shots; ++i_shot) {
        const auto i_state = sampler();
        ++measurements[i_state];
    }

    return measurements;
}

auto perform_measurements_as_counts_raw(
    const QuantumState& state,
    std::size_t n_shots,
    const QuantumNoise* noise,
    std::optional<int> seed
) -> std::map<std::size_t, std::size_t>
{
    const auto probabilities_raw = calculate_probabilities_raw(state, noise);
    return perform_measurements_as_counts_raw(probabilities_raw, n_shots, seed);
}

auto perform_measurements_as_counts_marginal(
    const std::vector<double>& probabilities_raw,
    std::size_t n_shots,
    const std::vector<std::size_t>& marginal_qubits,
    std::optional<int> seed
) -> std::map<std::string, std::size_t>
{
    if (!ket::internal::is_power_of_2(probabilities_raw.size())) {
        throw std::runtime_error {"The number of probabilities must be a power of 2.\n"};
    }

    const auto n_qubits = ket::internal::log_2_int(probabilities_raw.size());
    const auto marginal_bitmask = ket::internal::build_marginal_bitmask_(marginal_qubits, n_qubits);

    auto sampler = ket::internal::ProbabilitySampler_ {probabilities_raw, seed};
    auto measurements = std::map<std::string, std::size_t> {};

    // the internal layout of the quantum state is little endian, so the probabilities are as well
    const auto endian = ket::QuantumStateEndian::LITTLE;

    // REMINDER: if the entry does not exist, `std::map` will first initialize it to 0
    for (std::size_t i_shot {0}; i_shot < n_shots; ++i_shot) {
        const auto i_state = sampler();
        const auto bitstring = impl_ket::state_index_to_bitstring_marginal_(i_state, marginal_bitmask, endian);
        ++measurements[bitstring];
    }

    return measurements;
}

auto perform_measurements_as_counts_marginal(
    const QuantumState& state,
    std::size_t n_shots,
    const std::vector<std::size_t>& marginal_qubits,
    const QuantumNoise* noise,
    std::optional<int> seed
) -> std::map<std::string, std::size_t>
{
    const auto probabilities_raw = calculate_probabilities_raw(state, noise);
    return perform_measurements_as_counts_marginal(probabilities_raw, n_shots, marginal_qubits, seed);
}

auto perform_measurements_as_counts_marginal(
    const QuantumCircuit& circuit,
    const QuantumState& original_state,
    std::size_t n_shots,
    const std::vector<std::size_t>& marginal_qubits,
    const QuantumNoise* noise,
    std::optional<int> seed
) -> std::map<std::string, std::size_t>
{
    const auto n_qubits = circuit.n_qubits();
    const auto marginal_bitmask = ket::internal::build_marginal_bitmask_(marginal_qubits, n_qubits);

    // the internal layout of the quantum state is little endian, so the probabilities are as well
    const auto endian = ket::QuantumStateEndian::LITTLE;

    auto measurements = std::map<std::string, std::size_t> {};

    for (std::size_t i {0}; i < n_shots; ++i) {
        auto state = original_state;
        ket::simulate(circuit, state);

        const auto probabilities_raw = calculate_probabilities_raw(state, noise);
        auto sampler = ket::internal::ProbabilitySampler_ {probabilities_raw, seed};

        const auto i_state = sampler();
        const auto bitstring = impl_ket::state_index_to_bitstring_marginal_(i_state, marginal_bitmask, endian);
        ++measurements[bitstring];
    }

    return measurements;
}

auto perform_measurements_as_counts(
    const QuantumState& state,
    std::size_t n_shots,
    const QuantumNoise* noise,
    std::optional<int> seed
) -> std::map<std::string, std::size_t>
{
    const auto probabilities_raw = calculate_probabilities_raw(state, noise);
    const auto marginal_qubits = std::vector<std::size_t> {};
    return perform_measurements_as_counts_marginal(probabilities_raw, n_shots, marginal_qubits, seed);
}

}  // namespace ket


namespace ket::internal
{

/*
    We want to avoid sampling entries beyond the end of the probability distribution,
    because this correponds to an index for a computational state that does not exist.

    To prevent this, we need to offset the largest value produced by the random number
    generator by a small amount, to make sure the largest value is never sampled.
*/
auto cumulative_end_offset_(const std::vector<double>& cumulative_probabilities) -> double
{
    // a circuit requires at least 1 qubit, with at least two computational states; thus
    // there should be at least two entries in the vector of cumulative probabilities
    const auto size = cumulative_probabilities.size();

    const auto last = cumulative_probabilities[size - 1];

    // find the first probability from the end that is strictly less than the last probability
    const auto second_last = [&]()
    {
        for (std::size_t i {size - 1}; i > 0; --i) {
            if (cumulative_probabilities[i - 1] < last) {
                return cumulative_probabilities[i - 1];
            }
        }

        return 0.0;
    }();

    return (last - second_last) * CUMULATIVE_END_OFFSET_FRACTION;
}


auto calculate_cumulative_sum_(const std::vector<double>& probabilities) -> std::vector<double>
{
    auto cumulative = std::vector<double> {};
    cumulative.reserve(probabilities.size());

    std::partial_sum(probabilities.begin(), probabilities.end(), std::back_inserter(cumulative));

    return cumulative;
}

auto build_marginal_bitmask_(
    const std::vector<std::size_t>& marginal_qubits,
    std::size_t n_qubits
) -> std::vector<std::uint8_t>
{
    const auto is_in_range = [&](auto i) { return i >= n_qubits; };
    if (std::ranges::any_of(marginal_qubits, is_in_range))
    {
        throw std::runtime_error {"ERROR: marginal qubit index out of range."};
    }

    auto marginal_bitmask = std::vector<std::uint8_t>(n_qubits, 0);
    for (auto index : marginal_qubits) {
        marginal_bitmask[index] = 1;
    }

    return marginal_bitmask;
}

ProbabilitySampler_::ProbabilitySampler_(const std::vector<double>& probabilities, std::optional<int> seed)
    : cumulative_ {calculate_cumulative_sum_(probabilities)}
    , prng_ {impl_ket::get_prng_(seed)}
{
    const auto max_prob = cumulative_.back();
    const auto offset = cumulative_end_offset_(cumulative_);
    uniform_dist_ = std::uniform_real_distribution<double> {0.0, max_prob - offset};
}

auto ProbabilitySampler_::operator()() -> std::size_t
{
    const auto prob = uniform_dist_(prng_);

    const auto it_state = std::ranges::lower_bound(cumulative_, prob);

    if (it_state == cumulative_.end()) {
        throw std::runtime_error {
            "LOGIC BUG: Ended up with measurement of state past end of cumulative\n"
            "probability distribution, which shouldn't happen?"};
    }

    const auto i_state = static_cast<std::size_t>(std::distance(cumulative_.begin(), it_state));

    return i_state;
}

}  // namespace ket::internal
