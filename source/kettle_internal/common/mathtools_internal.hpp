#pragma once

#include <cstddef>
#include <tuple>

namespace ket::internal
{

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
