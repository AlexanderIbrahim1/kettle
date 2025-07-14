#pragma once

#include <cstddef>

namespace ket::internal
{

template <typename T, typename F>
auto almost_eq_helper_(
    const T& left_op,
    const T& right_op,
    F almost_eq_func
) -> bool
{
    if (left_op.size() != right_op.size()) {
        return false;
    }

    const auto size = left_op.size();
    for (std::size_t i {0}; i < size; ++i) {
        const auto& left = left_op.at(i);
        const auto& right = right_op.at(i);

        if (!almost_eq_func(left, right)) {
            return false;
        }
    }

    return true;
}

}  // namespace ket::internal
