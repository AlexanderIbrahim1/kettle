#pragma once

#include <cstddef>
#include <iterator>
#include <utility>
#include <vector>

namespace impl_ket
{

template <typename T>
struct always_false : std::false_type
{};

template <typename Container>
concept Iterable = requires(Container container)
{
    typename Container::value_type;
    container.begin();
    container.end();
};

template <typename Container>
concept QubitIndices = requires(Container container)
{
    typename Container::value_type;
    requires std::is_same_v<typename Container::value_type, std::size_t>;
    container.begin();
    container.end();
};

template <typename Container>
concept QubitIndicesAndAngles = requires(Container container)
{
    typename Container::value_type;
    requires std::is_same_v<typename Container::value_type, std::pair<std::size_t, double>>;
    container.begin();
    container.end();
};

template <typename Container>
concept ControlAndTargetIndices = requires(Container container)
{
    typename Container::value_type;
    requires std::is_same_v<typename Container::value_type, std::pair<std::size_t, std::size_t>>;
    container.begin();
    container.end();
};

// identical to the `ControlAndTargetIndices`, but I figured a distinct name would be helpful for users
template <typename Container>
concept QubitAndBitIndices = requires(Container container)
{
    typename Container::value_type;
    requires std::is_same_v<typename Container::value_type, std::pair<std::size_t, std::size_t>>;
    container.begin();
    container.end();
};

template <typename Container>
concept ControlAndTargetIndicesAndAngles = requires(Container container)
{
    typename Container::value_type;
    requires std::is_same_v<typename Container::value_type, std::tuple<std::size_t, std::size_t, double>>;
    container.begin();
    container.end();
};

using QubitIndicesIList = std::initializer_list<std::size_t>;
using QubitIndicesAndAnglesIList = std::initializer_list<std::pair<std::size_t, double>>;
using ControlAndTargetIndicesIList = std::initializer_list<std::pair<std::size_t, std::size_t>>;
using QubitAndBitIndicesIList = std::initializer_list<std::pair<std::size_t, std::size_t>>;
using ControlAndTargetIndicesAndAnglesIList = std::initializer_list<std::tuple<std::size_t, std::size_t, double>>;
using QubitIndicesVector = std::vector<std::size_t>;
using QubitIndicesAndAnglesVector = std::vector<std::pair<std::size_t, double>>;
using ControlAndTargetIndicesVector = std::vector<std::pair<std::size_t, std::size_t>>;
using QubitAndBitIndicesVector = std::vector<std::pair<std::size_t, std::size_t>>;
using ControlAndTargetIndicesAndAnglesVector = std::vector<std::tuple<std::size_t, std::size_t, double>>;

template <Iterable Container>
auto get_container_index(const Container& container, std::size_t index) -> std::size_t
{
    // std::initializer_list<T> doesn't support indexing, for some reason???
    if constexpr (std::is_same_v<Container, std::initializer_list<std::size_t>>) {
        return *(container.begin() + index);
    }
    else {
        return container[index];
    }
}

template <Iterable Container>
auto get_container_size(const Container& container) -> std::size_t
{
    return static_cast<std::size_t>(std::distance(container.begin(), container.end()));
}

template <QubitIndices Container0 = QubitIndicesIList, QubitIndices Container1 = QubitIndicesIList>
auto extend_container_to_vector(const Container0& container0, const Container1& container1) -> std::vector<std::size_t>
{
    const auto new_size = get_container_size(container0) + get_container_size(container1);

    auto new_container = std::vector<std::size_t> {};
    new_container.reserve(new_size);

    for (auto elem : container0) {
        new_container.push_back(elem);
    }

    for (auto elem : container1) {
        new_container.push_back(elem);
    }
    
    return new_container;
}

static constexpr auto MARGINALIZED_QUBIT = char {'x'};

}  // namespace impl_ket
