#include <complex>
#include <cstddef>

#include "kettle/common/mathtools.hpp"
#include "kettle_internal/common/mathtools_internal.hpp"

namespace ket::internal
{

auto pow_2_int(std::size_t exponent) noexcept -> std::size_t
{
    return std::size_t {1} << exponent;
}

auto is_power_of_2(std::size_t value) noexcept -> bool
{
    const auto is_positive = value > 0;
    const auto has_one_bit_set = (value & (value - 1)) == 0;

    return is_positive && has_one_bit_set;
}

auto log_2_int(std::size_t power) noexcept -> std::size_t
{
    // assumes that power > 0
    auto log2 = std::size_t {0};
    while (power > 0) {
        ++log2;
        power = (power >> 1UL);
    }

    return log2 - 1;
}

/*
    given a grid of side lengths (size0, size1), find (i0, i1), where
    
        i_flat = i1 + i0 * size1
*/
auto flat_index_to_grid_indices_2d(
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
auto flat_index_to_grid_indices_3d(
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

}  // namespace ket::internal

namespace ket
{

auto almost_eq(
    const std::complex<double>& left,
    const std::complex<double>& right,
    double tolerance_sq
) noexcept -> bool
{
    const auto diff_real = left.real() - right.real();
    const auto diff_imag = left.imag() - right.imag();
    const auto diff_sq = (diff_real * diff_real) + (diff_imag * diff_imag);

    return diff_sq < tolerance_sq;
}

auto endian_flip(std::size_t value, std::size_t n_relevant_bits) -> std::size_t
{
    auto backward = std::size_t {0};

    for (std::size_t i {0}; i < n_relevant_bits; ++i) {
        backward <<= 1UL;
        backward |= (value & 1UL);
        value >>= 1UL;
    }

    return backward;
}


}  // namespace ket
