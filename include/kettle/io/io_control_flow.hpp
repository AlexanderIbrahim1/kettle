#pragma once

#include <concepts>
#include <cstddef>
#include <sstream>
#include <tuple>
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

inline auto format_control_flow_predicate_(const ket::ControlFlowPredicate& predicate) -> std::string
{
    auto output = std::stringstream {};
    output << "BITS";
    output << format_csv_integers_(predicate.bit_indices_to_check());

    if (predicate.control_kind() == ket::ControlFlowBooleanKind::IF) {
        output << " == ";
    } else {
        output << " != ";
    }

    output << format_csv_integers_(predicate.expected_bits());

    return output.str();
}

inline auto format_classical_if_statement_header_(
    const impl_ket::ClassicalIfStatement& stmt
) -> std::string
{
    return std::string {"IF "} + format_control_flow_predicate_(stmt.predicate());
}

inline auto format_classical_if_else_statement_header_(
    const impl_ket::ClassicalIfElseStatement& stmt
) -> std::tuple<std::string, std::string>
{
    const auto if_part = std::string {"IF "} + format_control_flow_predicate_(stmt.predicate());
    const auto else_part = std::string {"ELSE"};

    return {if_part, else_part};
}

}  // namespace impl_ket
