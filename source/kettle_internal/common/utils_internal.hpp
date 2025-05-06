#pragma once

#include <cstddef>
#include <iterator>
#include <utility>
#include <vector>

#include "kettle/common/utils.hpp"

namespace ket::internal
{

template <typename T>
struct always_false : std::false_type
{};

template <ket::QubitIndices Container>
auto get_container_index(const Container& container, std::size_t index) -> std::size_t;

template <ket::QubitIndices Container>
auto get_container_size(const Container& container) -> std::size_t;

template <ket::QubitIndices Container0 = ket::QubitIndicesIList, ket::QubitIndices Container1 = ket::QubitIndicesIList>
auto extend_container_to_vector(const Container0& container0, const Container1& container1) -> std::vector<std::size_t>;

constexpr inline auto MARGINALIZED_QUBIT = char {'x'};

}  // namespace ket::internal
