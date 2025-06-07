#pragma once

#include <cstddef>
#include <initializer_list>
#include <utility>
#include <vector>

namespace ket
{

// TODO: simplify the other concepts with the Iterable concept
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

template <typename Container>
concept TwoControlOneTargetIndices = requires(Container container)
{
    typename Container::value_type;
    requires std::is_same_v<typename Container::value_type, std::tuple<std::size_t, std::size_t, std::size_t>>;
    container.begin();
    container.end();
};

template <typename Container>
concept TwoTargetIndices = requires(Container container)
{
    typename Container::value_type;
    requires std::is_same_v<typename Container::value_type, std::pair<std::size_t, std::size_t>>;
    container.begin();
    container.end();
};

template <typename Container>
concept OneControlTwoTargetIndices = requires(Container container)
{
    typename Container::value_type;
    requires std::is_same_v<typename Container::value_type, std::tuple<std::size_t, std::size_t, std::size_t>>;
    container.begin();
    container.end();
};

using QubitIndicesIList = std::initializer_list<std::size_t>;
using QubitIndicesAndAnglesIList = std::initializer_list<std::pair<std::size_t, double>>;
using ControlAndTargetIndicesIList = std::initializer_list<std::pair<std::size_t, std::size_t>>;
using QubitAndBitIndicesIList = std::initializer_list<std::pair<std::size_t, std::size_t>>;
using ControlAndTargetIndicesAndAnglesIList = std::initializer_list<std::tuple<std::size_t, std::size_t, double>>;
using TwoControlOneTargetIndicesIList = std::initializer_list<std::tuple<std::size_t, std::size_t, std::size_t>>;
using TwoTargetIndicesIList = std::initializer_list<std::pair<std::size_t, std::size_t>>;
using OneControlTwoTargetIndicesIList = std::initializer_list<std::tuple<std::size_t, std::size_t, std::size_t>>;

using QubitIndicesVector = std::vector<std::size_t>;
using QubitIndicesAndAnglesVector = std::vector<std::pair<std::size_t, double>>;
using ControlAndTargetIndicesVector = std::vector<std::pair<std::size_t, std::size_t>>;
using QubitAndBitIndicesVector = std::vector<std::pair<std::size_t, std::size_t>>;
using ControlAndTargetIndicesAndAnglesVector = std::vector<std::tuple<std::size_t, std::size_t, double>>;
using TwoControlOneTargetIndicesVector = std::vector<std::tuple<std::size_t, std::size_t, std::size_t>>;
using TwoTargetIndicesVector = std::vector<std::pair<std::size_t, std::size_t>>;
using OneControlTwoTargetIndicesVector = std::vector<std::tuple<std::size_t, std::size_t, std::size_t>>;

}  // namespace ket
