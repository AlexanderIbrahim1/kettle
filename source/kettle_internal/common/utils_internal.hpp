#pragma once

#include <algorithm>
#include <cstddef>
#include <vector>

#include "kettle/common/utils.hpp"

namespace ket::internal
{

template <ket::QubitIndices Container>
auto get_container_index(const Container& container, std::size_t index) -> std::size_t;

template <ket::QubitIndices Container>
auto get_container_size(const Container& container) -> std::size_t;

template <ket::QubitIndices Container0 = ket::QubitIndicesIList, ket::QubitIndices Container1 = ket::QubitIndicesIList>
auto extend_container_to_vector(const Container0& container0, const Container1& container1) -> std::vector<std::size_t>;

template <typename Element, typename Function>
auto all_same(const std::vector<Element>& elements, Function function) -> bool
{
    if (elements.empty()) {
        return true;
    }

    const auto value0 = function(elements[0]);
    const auto it1 = std::next(std::begin(elements));

    return std::all_of(it1, elements.end(), [&](const auto& elem) { return function(elem) == value0; });
}

constexpr inline auto MARGINALIZED_QUBIT = char {'x'};

}  // namespace ket::internal
