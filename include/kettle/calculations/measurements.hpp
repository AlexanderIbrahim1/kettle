#pragma once

#include <cstddef>
#include <map>
#include <optional>
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
