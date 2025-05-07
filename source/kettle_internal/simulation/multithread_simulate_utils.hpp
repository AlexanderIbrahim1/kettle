#pragma once

#include <vector>

#include "kettle_internal/simulation/simulate_utils.hpp"

/*
    This header file contains code to help perform the multithread quantum circuit simulations.
*/

namespace ket::internal
{

auto load_balanced_division_(std::size_t numerator, std::size_t denominator) -> std::vector<std::size_t>;

auto partial_sums_from_zero_(const std::vector<std::size_t>& values) -> std::vector<std::size_t>;

auto partial_sum_pairs_(std::size_t n_gate_pairs, std::size_t n_threads) -> std::vector<FlatIndexPair>;

}  // namespace ket::internal
