#pragma once

#include <complex>
#include <cstddef>

namespace ket::internal
{

constexpr inline auto NORMALIZATION_TOLERANCE = double {1.0e-6};
constexpr inline auto COMPLEX_ALMOST_EQ_TOLERANCE_SQ = double {1.0e-6};
constexpr inline auto MATRIX_2X2_SQRT_TOLERANCE = double {1.0e-6};

auto pow_2_int(std::size_t exponent) noexcept -> std::size_t;

auto is_power_of_2(std::size_t value) noexcept -> bool;

auto log_2_int(std::size_t power) noexcept -> std::size_t;

auto flat_index_to_grid_indices_2d(
    std::size_t i_flat,
    std::size_t size1
) -> std::tuple<std::size_t, std::size_t>;

auto flat_index_to_grid_indices_3d(
    std::size_t i_flat,
    std::size_t size1,
    std::size_t size2
) -> std::tuple<std::size_t, std::size_t, std::size_t>;

}  // namespace ket::internal

namespace ket
{

auto almost_eq(
    const std::complex<double>& left,
    const std::complex<double>& right,
    double tolerance_sq = ket::internal::COMPLEX_ALMOST_EQ_TOLERANCE_SQ
) noexcept -> bool;

}  // namespace ket
