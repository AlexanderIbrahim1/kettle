#pragma once

#include <cstddef>
#include <optional>
#include <random>
#include <vector>


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
