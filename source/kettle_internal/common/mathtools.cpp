#include <complex>
#include <cstddef>

#include <Eigen/Dense>

#include "kettle/common/mathtools.hpp"
#include "kettle_internal/common/mathtools_internal.hpp"

namespace ket::internal
{

template <typename T>
auto pow_2_int(T exponent) noexcept -> T
{
    return static_cast<T>(1UL << static_cast<std::size_t>(exponent));
}
template auto pow_2_int<std::size_t>(std::size_t exponent) noexcept -> std::size_t;
template auto pow_2_int<std::ptrdiff_t>(std::ptrdiff_t exponent) noexcept -> std::ptrdiff_t;

template <typename T>
auto is_power_of_2(T value) noexcept -> bool
{
    const auto is_positive = value > 0;
    const auto size_t_value = static_cast<std::size_t>(value);
    const auto has_one_bit_set = (size_t_value & (size_t_value - 1)) == 0;

    return is_positive && has_one_bit_set;
}
template auto is_power_of_2<std::size_t>(std::size_t value) noexcept -> bool;
template auto is_power_of_2<std::ptrdiff_t>(std::ptrdiff_t value) noexcept -> bool;

template <typename T>
auto log_2_int(T power) noexcept -> std::size_t 
{
    auto size_t_power = static_cast<std::size_t>(power);

    // assumes that power > 0
    auto log2 = std::size_t {0};
    while (size_t_power > 0) {
        ++log2;
        size_t_power = (size_t_power >> 1UL);
    }

    return log2 - 1UL;
}
template auto log_2_int<std::size_t>(std::size_t power) noexcept -> std::size_t;
template auto log_2_int<std::ptrdiff_t>(std::ptrdiff_t power) noexcept -> std::size_t;

template <typename T>
auto flat_index_to_grid_indices_2d(T i_flat, T size1) -> std::tuple<T, T>
{
    const auto i0 = i_flat / size1;
    i_flat -= (i0 * size1);

    const auto i1 = i_flat;

    return {i0, i1};
}
template auto flat_index_to_grid_indices_2d<std::size_t>(std::size_t i_flat, std::size_t size1) -> std::tuple<std::size_t, std::size_t>;
template auto flat_index_to_grid_indices_2d<std::ptrdiff_t>(std::ptrdiff_t i_flat, std::ptrdiff_t size1) -> std::tuple<std::ptrdiff_t, std::ptrdiff_t>;

template <typename T>
auto flat_index_to_grid_indices_3d(T i_flat, T size1, T size2) -> std::tuple<T, T, T>
{
    const auto i0 = i_flat / (size1 * size2);
    i_flat -= (i0 * size1 * size2);

    const auto i1 = i_flat / size2;
    i_flat -= (i1 * size2);

    const auto i2 = i_flat;

    return {i0, i1, i2};
}
template auto flat_index_to_grid_indices_3d<std::size_t>(std::size_t i_flat, std::size_t size1, std::size_t size2) -> std::tuple<std::size_t, std::size_t, std::size_t>;
template auto flat_index_to_grid_indices_3d<std::ptrdiff_t>(std::ptrdiff_t i_flat, std::ptrdiff_t size1, std::ptrdiff_t size2) -> std::tuple<std::ptrdiff_t, std::ptrdiff_t, std::ptrdiff_t>;

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
