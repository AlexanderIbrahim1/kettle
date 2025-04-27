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

// TODO: implement while loop

namespace impl_ket
{

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
    const ClassicalIfStatement& stmt
) -> std::string
{
    return std::string {"IF "} + format_control_flow_predicate_(stmt.predicate());
}

inline auto format_classical_if_else_statement_header_(
    const ClassicalIfElseStatement& stmt
) -> std::tuple<std::string, std::string>
{
    const auto if_part = std::string {"IF "} + format_control_flow_predicate_(stmt.predicate());
    const auto else_part = std::string {"ELSE"};

    return {if_part, else_part};
}

/*
    Discard characters from the stream until a certain delimiter is found; that
    delimited remains within the stream.
*/
inline void discard_until_char_(std::stringstream& stream, char delimiter)
{
    char ch {};
    while (stream.peek() != EOF && stream.peek() != delimiter) {
        stream.get(ch);
    }
}

/*
    Parse the csv of integers within a pair of square brackets into a vector.

    For example:
      - "[0, 3]" parses into std::vector {0, 3}
      - "[]" parses into std::vector {}
*/
template <std::integral Integer>
auto parse_csv_in_brackets_(std::stringstream& stream) -> std::vector<Integer>
{
    auto output = std::vector<Integer> {};

    char ch {};
    std::string str;
    Integer value {};

    stream >> ch;  // '['

    if (stream.peek() == ']') {
        return {};
    }

    do {
        stream >> value;
        stream >> str;

        output.push_back(value);
    } while (str != "]");

    return output;
}

/*
    Parse the comparison sign to determine what kind of if statement is being used.
*/
inline auto parse_comparison_sign_(std::stringstream& stream) -> ket::ControlFlowBooleanKind
{
    std::string str;
    stream >> str;

    if (str == "==") {
        return ket::ControlFlowBooleanKind::IF;
    }
    else if (str == "!=") {
        return ket::ControlFlowBooleanKind::IF_NOT;
    }
    else {
        throw std::runtime_error {"Invalid comparison sign found; must be '==' or '!='\n"};
    }
}

/*
    Parse the portion of a control flow statement that contains the 'BITS' keyword,
    the classical registers to check, and the expected bit values.

    For example, "BITS[0, 3] == [1, 0]" should parse into a ControlFlowPredicate where:
      - the bit indices to check are {0, 3}
      - the corresponding expected bits are {1, 0}
      - the kind of predicate is IF (for ==) and IF_NOT (for !=)
*/
inline auto parse_control_flow_predicate_(std::stringstream& stream) -> ket::ControlFlowPredicate
{
    // remove the BITS
    discard_until_char_(stream, '[');

    auto bit_indices_to_check = parse_csv_in_brackets_<std::size_t>(stream);
    const auto control_kind = parse_comparison_sign_(stream);

    discard_until_char_(stream, '[');

    auto expected_bits = parse_csv_in_brackets_<int>(stream);

    return ket::ControlFlowPredicate {std::move(bit_indices_to_check), std::move(expected_bits), control_kind};
}

}  // namespace impl_ket
