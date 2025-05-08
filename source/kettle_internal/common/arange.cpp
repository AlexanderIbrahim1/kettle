#include <algorithm>
#include <cmath>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

#include "kettle/common/arange.hpp"

/*
    Creating a sequence of integers between two values is a very common operation when working
    with quantum circuits, but C++ doesn't have a convenient way of doing this.
*/

namespace
{

enum class RightCompare_ : std::uint8_t
{
    LESS_THAN,
    GREATER_THAN
};

template <std::integral Integer, RightCompare_ Compare>
auto arange_helper_(Integer left, Integer right, std::int64_t step, Integer capacity) -> std::vector<Integer>
{
    auto output = std::vector<Integer> {};
    output.reserve(static_cast<std::size_t>(capacity));

    const auto left_ = static_cast<std::int64_t>(left);
    const auto right_ = static_cast<std::int64_t>(right);

    if constexpr (Compare == RightCompare_::LESS_THAN) {
        for (std::int64_t i {left_}; i < right_; i += step) {
            output.push_back(static_cast<Integer>(i));
        }
    } else {
        for (std::int64_t i {left_}; i > right_; i += step) {
            output.push_back(static_cast<Integer>(i));
        }
    }

    return output;
}
template auto arange_helper_<int, RightCompare_::LESS_THAN>(
    int left, int right, std::int64_t step, int capacity
) -> std::vector<int>;
template auto arange_helper_<std::size_t, RightCompare_::LESS_THAN>(
    std::size_t left, std::size_t right, std::int64_t step, std::size_t capacity
) -> std::vector<std::size_t>;
template auto arange_helper_<int, RightCompare_::GREATER_THAN>(
    int left, int right, std::int64_t step, int capacity
) -> std::vector<int>;
template auto arange_helper_<std::size_t, RightCompare_::GREATER_THAN>(
    std::size_t left, std::size_t right, std::int64_t step, std::size_t capacity
) -> std::vector<std::size_t>;

}  // namespace


namespace ket
{

template <std::integral Integer>
auto arange(Integer value) -> std::vector<Integer>
{
    using RC = RightCompare_;

    if (value <= 0) {
        return {};
    }

    return ::arange_helper_<Integer, RC::LESS_THAN>(Integer {0}, value, 1, value);
}
template auto arange<int>(int value) -> std::vector<int>;
template auto arange<std::size_t>(std::size_t value) -> std::vector<std::size_t>;


template <std::integral Integer>
auto arange(Integer left, Integer right, std::int64_t step) -> std::vector<Integer>
{
    using RC = RightCompare_;

    if (step == 0) {
        throw std::runtime_error {"The `step` value cannot be 0 in `arange()`"};
    }

    const auto abs_step = static_cast<Integer>(std::abs(step));

    if (step >= 1) {
        if (left >= right) {
            return {};
        } else {
            const auto n_terms = Integer{1} + ((right - left) / abs_step);
            return arange_helper_<Integer, RC::LESS_THAN>(left, right, step, n_terms);
        }
    }
    else {
        if (right >= left) {
            return {};
        } else {
            const auto n_terms = Integer{1} + ((left - right) / abs_step);
            return arange_helper_<Integer, RC::GREATER_THAN>(left, right, step, n_terms);
        }
    }
}
template auto arange(int left, int right, std::int64_t step) -> std::vector<int>;
template auto arange(std::size_t left, std::size_t right, std::int64_t step) -> std::vector<std::size_t>;


template <std::integral Integer>
auto revarange(Integer value) -> std::vector<Integer>
{
    auto output = arange(value);
    std::ranges::reverse(output);

    return output;
}
template auto revarange<int> (int value) -> std::vector<int>;
template auto revarange<std::size_t> (std::size_t value) -> std::vector<std::size_t>;


template <std::integral Integer>
auto revarange(Integer left, Integer right, std::int64_t step) -> std::vector<Integer>
{
    auto output = arange(left, right, step);
    std::ranges::reverse(output);

    return output;
}
template auto revarange<int>(int left, int right, std::int64_t step) -> std::vector<int>;
template auto revarange<std::size_t>(std::size_t left, std::size_t right, std::int64_t step) -> std::vector<std::size_t>;

}  // namespace ket
