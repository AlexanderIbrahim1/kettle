#pragma once

#include <cmath>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <vector>

/*
    Creating a sequence of integers between two values is a very common operation when working
    with quantum circuits, but C++ doesn't have a convenient way of doing this.
*/

namespace ket
{

/*
    Create a `std::vector` instance that holds integers {0, 1, 2, ..., value - 1}.

    If `value <= 0`, an empty vector is returned, like Python's `range` and numpy's `arange`.
*/
template <std::integral Integer = std::size_t>
auto arange(Integer value) -> std::vector<Integer>;

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
auto arange(Integer left, Integer right, std::int64_t step = 1) -> std::vector<Integer>;

/*
    Create a `std::vector` instance that holds the reverse of `arange(value)`.
*/
template <std::integral Integer = std::size_t>
auto revarange(Integer value) -> std::vector<Integer>;

/*
    Create a `std::vector` instance that holds the reverse of `arange(left, right, step)`.
*/
template <std::integral Integer = std::size_t>
auto revarange(Integer left, Integer right, std::int64_t step = 1) -> std::vector<Integer>;

}  // namespace ket
