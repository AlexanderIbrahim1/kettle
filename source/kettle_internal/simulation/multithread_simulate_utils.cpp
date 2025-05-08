#include <stdexcept>
#include <vector>

#include "kettle_internal/simulation/simulate_utils.hpp"
#include "kettle_internal/simulation/multithread_simulate_utils.hpp"

/*
    This header file contains code to help perform the multithread quantum circuit simulations.
*/

namespace ket::internal
{

auto load_balanced_division_(std::size_t numerator, std::size_t denominator) -> std::vector<std::size_t>
{
    if (denominator == 0) {
        throw std::runtime_error {"Cannot perform load balancing division among 0 threads"};
    }

    const auto remainder = numerator % denominator;
    const auto divisor = numerator / denominator;

    auto output = std::vector<std::size_t> (denominator, divisor);
    for (std::size_t i {0}; i < remainder; ++i) {
        ++output[i];
    }

    return output;
}

auto partial_sums_from_zero_(const std::vector<std::size_t>& values) -> std::vector<std::size_t>
{
    auto output = std::vector<std::size_t> {};
    output.reserve(values.size() + 1);

    output.push_back(0);
    for (std::size_t i {0}; i < values.size(); ++i) {
        output.push_back(output[i] + values[i]);
    }

    return output;
}

auto partial_sum_pairs_(std::size_t n_gate_pairs, std::size_t n_threads) -> std::vector<FlatIndexPair>
{
    const auto gate_splits = load_balanced_division_(n_gate_pairs, n_threads);

    auto output = std::vector<FlatIndexPair> {};
    output.reserve(gate_splits.size());
    output.emplace_back(0, gate_splits[0]);

    for (std::size_t i {1}; i < gate_splits.size(); ++i) {
        const auto i_lower = output[i - 1].i_upper;
        const auto i_upper = i_lower + gate_splits[i];
        output.emplace_back(i_lower, i_upper);
    }

    return output;
}

}  // namespace ket::internal
