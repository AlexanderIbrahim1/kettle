#pragma once

#include "kettle/common/mathtools.hpp"

#include <cstddef>

namespace ket::internal
{

template <typename T>
auto almost_eq_pauli_helper_(
    const T& left_op,
    const T& right_op,
    double coeff_tolerance
) -> bool
{
    if (left_op.size() != right_op.size()) {
        return false;
    }

    const auto size = left_op.size();
    for (std::size_t i {0}; i < size; ++i) {
        const auto& left = left_op.at(i);
        const auto& right = right_op.at(i);

        if (!ket::almost_eq(left.coefficient, right.coefficient, coeff_tolerance)) {
            return false;
        }

        if (left.pauli_string != right.pauli_string) {
            return false;
        }
    }

    return true;
}

}  // namespace ket::internal
