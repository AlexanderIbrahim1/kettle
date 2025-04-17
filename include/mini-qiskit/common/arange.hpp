#pragma once

#include <cmath>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

/*
    Creating a sequence of integers between two values is a very common operation when working
    with quantum circuits, but C++ doesn't have a convenient way of doing this.
*/

namespace impl_mqis
{

enum class RightCompare_
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

}  // namespace impl_mqis


namespace mqis
{

/*
    Create a `std::vector` instance that holds integers {0, 1, 2, ..., value - 1}.

    If `value <= 0`, an empty vector is returned, like Python's `range` and numpy's `arange`.
*/
template <std::integral Integer = std::size_t>
auto arange(Integer value) -> std::vector<Integer>
{
    using RC = impl_mqis::RightCompare_;

    if (value <= 0) {
        return {};
    }

    return impl_mqis::arange_helper_<Integer, RC::LESS_THAN>(Integer {0}, value, 1, value);
}

/*
    If `step >= 1`;
    create a `std::vector` instance that holds integers {left, left + step, left + 2*step, ..., }
    up to but not exceeding `right - 1`.

    If `step <= -1`;
    create a `std::vector` instance that holds integers {left, left - step, left - 2*step, ..., }
    down to but not less than `right + 1`.

    The argument `step` cannot be `0`.

    If `left >= right` and `step >= 1`, the empty vector is returned.
    If `right >= left` and `step <= -1`, the empty vector is returned.
*/
template <std::integral Integer = std::size_t>
auto arange(Integer left, Integer right, std::int64_t step = 1) -> std::vector<Integer>
{
    using RC = impl_mqis::RightCompare_;

    if (step == 0) {
        throw std::runtime_error {"The `step` value cannot be 0 in `arange()`"};
    }

    const auto abs_step = static_cast<Integer>(std::abs(step));

    if (step >= 1) {
        if (left >= right) {
            return {};
        } else {
            const auto n_terms = Integer{1} + (right - left) / abs_step;
            return impl_mqis::arange_helper_<Integer, RC::LESS_THAN>(left, right, step, n_terms);
        }
    }
    else {
        if (right >= left) {
            return {};
        } else {
            const auto n_terms = Integer{1} + (left - right) / abs_step;
            return impl_mqis::arange_helper_<Integer, RC::GREATER_THAN>(left, right, step, n_terms);
        }
    }
}

/*
    Create a `std::vector` instance that holds the reverse of `arange(value)`.
*/
template <std::integral Integer = std::size_t>
auto revarange(Integer value) -> std::vector<Integer>
{
    auto output = arange(value);
    std::reverse(output.begin(), output.end());

    return output;
}

/*
    Create a `std::vector` instance that holds the reverse of `arange(left, right, step)`.
*/
template <std::integral Integer = std::size_t>
auto revarange(Integer left, Integer right, std::int64_t step = 1) -> std::vector<Integer>
{
    auto output = arange(left, right, step);
    std::reverse(output.begin(), output.end());

    return output;
}

}  // namespace mqis
