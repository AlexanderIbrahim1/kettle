#pragma once

#include <concepts>
#include <cstddef>
#include <utility>

namespace impl_mqis
{

template <typename T>
struct always_false : std::false_type
{};

template <typename Container>
concept ContainerOfQubitIndices = (requires(Container container) {
                                      typename Container::value_type;
                                      requires std::is_same_v<typename Container::value_type, std::size_t>;
                                  }) || std::is_same_v<Container, std::initializer_list<std::size_t>>;

template <typename Container>
concept ContainerOfAnglesAndQubitIndices =
    (requires(Container container) {
        typename Container::value_type;
        requires std::is_same_v<typename Container::value_type, std::pair<double, std::size_t>>;
    }) || std::is_same_v<Container, std::initializer_list<std::pair<double, std::size_t>>>;

template <typename Container>
concept ContainerOfControlAndTargetQubitIndices =
    (requires(Container container) {
        typename Container::value_type;
        requires std::is_same_v<typename Container::value_type, std::pair<std::size_t, std::size_t>>;
    }) || std::is_same_v<Container, std::initializer_list<std::pair<std::size_t, std::size_t>>>;

template <typename Container>
concept ContainerOfAnglesAndControlAndTargetQubitIndices =
    (requires(Container container) {
        typename Container::value_type;
        requires std::is_same_v<typename Container::value_type, std::tuple<double, std::size_t, std::size_t>>;
    }) || std::is_same_v<Container, std::initializer_list<std::tuple<double, std::size_t, std::size_t>>>;

template <ContainerOfQubitIndices Container>
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

template <typename Container>
auto get_container_size(const Container& container) -> std::size_t
{
    return static_cast<std::size_t>(std::distance(container.begin(), container.end()));
}

template <typename Container0 = std::initializer_list<std::size_t>, typename Container1 = std::initializer_list<std::size_t>>
requires ContainerOfQubitIndices<Container0> && ContainerOfQubitIndices<Container1>
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

}  // namespace impl_mqis
