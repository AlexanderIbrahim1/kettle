#pragma once

#include <cstddef>
#include <tuple>

namespace ket::internal
{

/*
    Calculate `2^exponent`.
*/
template <typename T>
auto pow_2_int(T exponent) noexcept -> T;

/*
    Returns if `value` is a nonnegative power of 2.
*/
template <typename T>
auto is_power_of_2(T value) noexcept -> bool;

/*
    Calculate the binary logarithm of `power`.
*/
template <typename T>
auto log_2_int(T power) noexcept -> std::size_t;

/*
    Given a grid of side lengths (size0, size1), find (i0, i1), where
        `i_flat = i1 + i0 * size1`
*/
auto flat_index_to_grid_indices_2d(
    std::size_t i_flat,
    std::size_t size1
) -> std::tuple<std::size_t, std::size_t>;

/*
    Given a grid of side lengths (size0, size1, size2), find (i0, i1, i2), where
        `i_flat = i2 + i1 * size2 + i0 * size1 * size2`
*/
auto flat_index_to_grid_indices_3d(
    std::size_t i_flat,
    std::size_t size1,
    std::size_t size2
) -> std::tuple<std::size_t, std::size_t, std::size_t>;

}  // namespace ket::internal
