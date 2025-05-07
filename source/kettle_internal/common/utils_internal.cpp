#include <cstddef>
#include <iterator>
#include <vector>

#include "kettle/common/utils.hpp"
#include "kettle_internal/common/utils_internal.hpp"

namespace ket::internal
{

template <ket::QubitIndices Container>
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
template auto get_container_index<ket::QubitIndicesVector>(const ket::QubitIndicesVector& container, std::size_t index) -> std::size_t;
template auto get_container_index<ket::QubitIndicesIList>(const ket::QubitIndicesIList& container, std::size_t index) -> std::size_t;

template <ket::QubitIndices Container>
auto get_container_size(const Container& container) -> std::size_t
{
    return static_cast<std::size_t>(std::distance(container.begin(), container.end()));
}
template auto get_container_size<ket::QubitIndicesVector>(const ket::QubitIndicesVector& container) -> std::size_t;
template auto get_container_size<ket::QubitIndicesIList>(const ket::QubitIndicesIList& container) -> std::size_t;

template <ket::QubitIndices Container0, ket::QubitIndices Container1>
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
template auto extend_container_to_vector<ket::QubitIndicesVector, ket::QubitIndicesVector>(
    const ket::QubitIndicesVector& container0, const ket::QubitIndicesVector& container1
) -> std::vector<std::size_t>;
template auto extend_container_to_vector<ket::QubitIndicesIList, ket::QubitIndicesVector>(
    const ket::QubitIndicesIList& container0, const ket::QubitIndicesVector& container1
) -> std::vector<std::size_t>;
template auto extend_container_to_vector<ket::QubitIndicesVector, ket::QubitIndicesIList>(
    const ket::QubitIndicesVector& container0, const ket::QubitIndicesIList& container1
) -> std::vector<std::size_t>;
template auto extend_container_to_vector<ket::QubitIndicesIList, ket::QubitIndicesIList>(
    const ket::QubitIndicesIList& container0, const ket::QubitIndicesIList& container1
) -> std::vector<std::size_t>;

}  // namespace ket::internal
