#pragma once

#include <bitset>
#include <complex>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <string>

namespace impl_mqis
{

constexpr static auto NORMALIZATION_TOLERANCE = double {1.0e-6};
constexpr static auto COMPLEX_ALMOST_EQ_TOLERANCE_SQ = double {1.0e-6};
constexpr static auto ANGLE_ALMOST_EQ_TOLERANCE = double {1.0e-8};
constexpr static auto MATRIX_2X2_SQRT_TOLERANCE = double {1.0e-6};

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

constexpr auto log_2_int(std::size_t power) noexcept -> std::size_t
{
    // assumes that power > 0
    auto log2 = std::size_t {0};
    while (power > 0) {
        ++log2;
        power = (power >> 1);
    }

    return log2 - 1;
}

/*
    given a grid of side lengths (size0, size1), find (i0, i1), where
    
        i_flat = i1 + i0 * size1
*/
constexpr auto flat_index_to_grid_indices_2d(
    std::size_t i_flat,
    std::size_t size1
) -> std::tuple<std::size_t, std::size_t>
{
    const auto i0 = i_flat / size1;
    i_flat -= (i0 * size1);

    const auto i1 = i_flat;

    return {i0, i1};
}

/*
    given a grid of side lengths (size0, size1, size2), find (i0, i1, i2), where
    
        i_flat = i2 + i1 * size2 + i0 * size1 * size2
*/
constexpr auto flat_index_to_grid_indices_3d(
    std::size_t i_flat,
    std::size_t size1,
    std::size_t size2
) -> std::tuple<std::size_t, std::size_t, std::size_t>
{
    const auto i0 = i_flat / (size1 * size2);
    i_flat -= (i0 * size1 * size2);

    const auto i1 = i_flat / size2;
    i_flat -= (i1 * size2);

    const auto i2 = i_flat;

    return {i0, i1, i2};
}

}  // namespace impl_mqis

namespace mqis
{

constexpr auto almost_eq(
    const std::complex<double>& left,
    const std::complex<double>& right,
    double tolerance_sq = impl_mqis::COMPLEX_ALMOST_EQ_TOLERANCE_SQ
) noexcept -> bool
{
    const auto diff_real = left.real() - right.real();
    const auto diff_imag = left.imag() - right.imag();
    const auto diff_sq = diff_real * diff_real + diff_imag * diff_imag;

    return diff_sq < tolerance_sq;
}

}  // namespace mqis
