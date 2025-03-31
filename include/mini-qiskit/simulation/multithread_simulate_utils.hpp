#pragma once

#include <stdexcept>
#include <vector>

/*
    This header file contains code to help perform the multithread quantum circuit simulations.
*/

namespace impl_mqis
{

inline auto load_balanced_division(std::size_t numerator, std::size_t denominator) -> std::vector<std::size_t>
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

inline auto partial_sums_from_zero(const std::vector<std::size_t>& values) -> std::vector<std::size_t>
{
    auto output = std::vector<std::size_t> {};
    output.reserve(values.size() + 1);

    output.push_back(0);
    for (std::size_t i {0}; i < values.size(); ++i) {
        output.push_back(output[i] + values[i]);
    }

    return output;
}

}  // namespace impl_mqis
