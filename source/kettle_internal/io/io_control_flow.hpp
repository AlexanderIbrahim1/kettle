#pragma once

#include <concepts>
#include <cstddef>
#include <sstream>
#include <tuple>
#include <vector>

#include "kettle/circuit/control_flow_predicate.hpp"

/*
    This header file contains code for writing out and parsing the control flow
    predicate statements
*/

namespace ket::internal
{

inline constexpr auto CONTROL_FLOW_WHITESPACE_DEFAULT = std::size_t {4};

}  // namespace ket::internal


namespace ket::internal::format
{

template <std::integral Integer>
auto format_csv_integers_(const std::vector<Integer>& integers) -> std::string;

auto format_control_flow_predicate_(const ket::ControlFlowPredicate& predicate) -> std::string;

auto format_classical_if_statement_header_(const ket::ControlFlowPredicate& predicate) -> std::string;

auto format_classical_if_else_statement_header_(const ket::ControlFlowPredicate& predicate) -> std::tuple<std::string, std::string>;

}  // namespace ket::internal::format


namespace ket::internal::parse
{

/*
    Discard characters from the stream until a certain delimiter is found; that
    delimited remains within the stream.
*/
void discard_until_char_(std::stringstream& stream, char delimiter);

/*
    Parse the csv of integers within a pair of square brackets into a vector.

    For example:
      - "[0, 3]" parses into std::vector {0, 3}
      - "[]" parses into std::vector {}
*/
template <std::integral Integer>
auto parse_csv_in_brackets_(std::stringstream& stream) -> std::vector<Integer>;

/*
    Parse the comparison sign to determine what kind of if statement is being used.
*/
auto parse_comparison_sign_(std::stringstream& stream) -> ket::ControlFlowBooleanKind;

/*
    Parse the portion of a control flow statement that contains the 'BITS' keyword,
    the classical registers to check, and the expected bit values.

    For example, "BITS[0, 3] == [1, 0]" should parse into a ControlFlowPredicate where:
      - the bit indices to check are {0, 3}
      - the corresponding expected bits are {1, 0}
      - the kind of predicate is IF (for ==) and IF_NOT (for !=)
*/
auto parse_control_flow_predicate_(std::stringstream& stream) -> ket::ControlFlowPredicate;

}  // namespace ket::internal::parse
