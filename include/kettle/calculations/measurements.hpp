#pragma once

#include <cstddef>
#include <map>
#include <optional>
#include <random>
#include <string>
#include <vector>

#include "kettle/calculations/probabilities.hpp"
#include "kettle/circuit/circuit.hpp"
#include "kettle/state/state.hpp"

/*
    This file contains code components to perform measurements of the state.
*/

namespace ket
{

auto memory_to_counts(const std::vector<std::size_t>& measurements) -> std::map<std::size_t, std::size_t>;

auto memory_to_fractions(const std::vector<std::size_t>& measurements) -> std::map<std::size_t, double>;

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
    std::optional<int> seed = std::nullopt
) -> std::vector<std::size_t>;

auto perform_measurements_as_memory(
    const QuantumState& state,
    std::size_t n_shots,
    const QuantumNoise* noise = nullptr,
    std::optional<int> seed = std::nullopt
) -> std::vector<std::size_t>;

auto perform_measurements_as_counts_raw(
    const std::vector<double>& probabilities_raw,
    std::size_t n_shots,
    std::optional<int> seed = std::nullopt
) -> std::map<std::size_t, std::size_t>;

auto perform_measurements_as_counts_raw(
    const QuantumState& state,
    std::size_t n_shots,
    const QuantumNoise* noise = nullptr,
    std::optional<int> seed = std::nullopt
) -> std::map<std::size_t, std::size_t>;

auto perform_measurements_as_counts_marginal(
    const std::vector<double>& probabilities_raw,
    std::size_t n_shots,
    const std::vector<std::size_t>& marginal_qubits = {},
    std::optional<int> seed = std::nullopt
) -> std::map<std::string, std::size_t>;

auto perform_measurements_as_counts_marginal(
    const QuantumState& state,
    std::size_t n_shots,
    const std::vector<std::size_t>& marginal_qubits = {},
    const QuantumNoise* noise = nullptr,
    std::optional<int> seed = std::nullopt
) -> std::map<std::string, std::size_t>;

auto perform_measurements_as_counts_marginal(
    const QuantumCircuit& circuit,
    const QuantumState& original_state,
    std::size_t n_shots,
    const std::vector<std::size_t>& marginal_qubits = {},
    const QuantumNoise* noise = nullptr,
    std::optional<int> seed = std::nullopt
) -> std::map<std::string, std::size_t>;

auto perform_measurements_as_counts(
    const QuantumState& state,
    std::size_t n_shots,
    const QuantumNoise* noise = nullptr,
    std::optional<int> seed = std::nullopt
) -> std::map<std::string, std::size_t>;

}  // namespace ket


namespace ket::internal
{

constexpr inline auto CUMULATIVE_END_OFFSET_FRACTION = double {1.0e-4};

/*
    We want to avoid sampling entries beyond the end of the probability distribution,
    because this correponds to an index for a computational state that does not exist.

    To prevent this, we need to offset the largest value produced by the random number
    generator by a small amount, to make sure the largest value is never sampled.
*/
auto cumulative_end_offset_(const std::vector<double>& cumulative_probabilities) -> double;

auto calculate_cumulative_sum_(const std::vector<double>& probabilities) -> std::vector<double>;

auto build_marginal_bitmask_(const std::vector<std::size_t>& marginal_qubits, std::size_t n_qubits) -> std::vector<std::uint8_t>;

class ProbabilitySampler_
{
public:
    explicit ProbabilitySampler_(const std::vector<double>& probabilities, std::optional<int> seed = std::nullopt);

    auto operator()() -> std::size_t;

private:
    std::vector<double> cumulative_;
    std::mt19937 prng_;
    std::uniform_real_distribution<double> uniform_dist_;
};

}  // namespace ket::internal
