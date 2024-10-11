#pragma once

#include <cstddef>

namespace impl_mqis
{
constexpr static auto NORMALIZATION_TOLERANCE = double {1.0e-6};

constexpr auto norm_squared(double real, double imag) noexcept -> double
{
    return real * real + imag * imag;
}

constexpr auto pow_2_int(std::size_t exponent) noexcept -> std::size_t
{
    return 1 << exponent;
}

constexpr auto is_power_of_2(std::size_t value) noexcept -> bool
{
    const auto is_positive = value > 0;
    const auto has_one_bit_set = (value & (value - 1)) == 0;

    return is_positive && has_one_bit_set;
}

}  // namespace impl_mqis
