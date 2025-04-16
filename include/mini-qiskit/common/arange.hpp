#pragma once

#include <cmath>
#include <concepts>
#include <cstddef>
#include <stdexcept>
#include <vector>

/*
    Creating a sequence of integers between two values is a very common operation when working
    with quantum circuits, but C++ doesn't have a convenient way of doing this.
*/

namespace impl_mqis
{

enum class RightCompare
{
    LESS_THAN,
    GREATER_THAN
};

template <std::integral Integer, RightCompare Compare>
auto arange_helper_(Integer left, Integer right, Integer step, Integer capacity) -> std::vector<Integer>
{
    auto output = std::vector<Integer> {};
    output.reserve(static_cast<std::size_t>(capacity));

    if constexpr (Compare == RightCompare::LESS_THAN) {
        for (Integer i {left}; i < right; i += step) {
            output.push_back(i);
        }
    } else {
        for (Integer i {left}; i > right; i += step) {
            output.push_back(i);
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
    using RC = impl_mqis::RightCompare;

    if (value <= 0) {
        return {};
    }

    return impl_mqis::arange_helper_<Integer, RC::LESS_THAN>(Integer {0}, value, Integer {1}, value);
}

/*
    Create a `std::vector` instance that holds integers {left, left + 1, ..., right - 2, right - 1}.

    If `left >= right`, an empty vector is returned, like Python's `range` and numpy's `arange`.
*/
template <std::integral Integer = std::size_t>
auto arange(Integer left, Integer right) -> std::vector<Integer>
{
    using RC = impl_mqis::RightCompare;

    if (left >= right) {
        return {};
    }

    return impl_mqis::arange_helper_<Integer, RC::LESS_THAN>(left, right, Integer {1}, right - left);
}
//     auto output = std::vector<Integer> {};
//     output.reserve(right - left);
// 
//     for (Integer i {left}; i < right; ++i) {
//         output.push_back(i);
//     }
// 
//     return output;

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
auto arange(Integer left, Integer right, Integer step) -> std::vector<Integer>
{
    using RC = impl_mqis::RightCompare;

    if constexpr (std::is_signed_v<Integer>) {
        if (step <= Integer{-1}) {
            if (right >= left) {
                return {};
            } else {
                const auto n_terms = Integer{1} + (left - right) / std::abs(step);
                return impl_mqis::arange_helper_<Integer, RC::GREATER_THAN>(left, right, step, n_terms);
            }
        }
    }

    if (step >= Integer{1}) {
        if (left >= right) {
            return {};
        } else {
            const auto n_terms = Integer{1} + (left - right) / step;
            return impl_mqis::arange_helper_<Integer, RC::LESS_THAN>(left, right, step, n_terms);
        }
    } else {
        throw std::runtime_error {"The `step` value cannot be 0 in `arange()`"};
    }
}

}  // namespace mqis

// 
//                 auto output = std::vector<Integer> {};
//                 output.reserve(n_terms);
//                 for (Integer i {left}; i > right; i += step) {
//                     output.push_back(i);
//                 }
//                 
//                 return output;
