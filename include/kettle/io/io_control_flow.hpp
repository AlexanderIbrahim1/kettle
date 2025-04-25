#pragma once

#include <concepts>
#include <cstddef>
#include <sstream>
#include <vector>

#include "kettle/circuit/control_flow_predicate.hpp"
#include "kettle/circuit/control_flow.hpp"

/*
    This header file contains code for writing out and parsing the control flow
    predicate statements
*/

namespace impl_ket
{

/*
    Some examples:

    BITS[0, 3, 4] = [0, 1, 1]
*/

template <std::integral Integer>
auto format_csv_integers_(const std::vector<Integer>& integers) -> std::string
{
    if (integers.size() == 0) {
        return "[]";
    }

    auto output = std::stringstream {};
    output << "[";
    output << integers[0];

    for (std::size_t i {1}; i < integers.size(); ++i) {
        output << ", " << integers[i];
    }

    output << "]";

    return output.str();
}

inline auto format_control_flow_predicate(const ket::ControlFlowPredicate& predicate) -> std::string
{
    auto output = std::stringstream {};
    output << "BITS";
    output << format_csv_integers_(predicate.bit_indices_to_check());
    output << " = ";
    output << format_csv_integers_(predicate.expected_bits());

    return output.str();
}

}  // namespace impl_ket
