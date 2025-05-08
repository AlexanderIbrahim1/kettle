#include <cstddef>
#include <sstream>
#include <tuple>
#include <vector>

#include "kettle/circuit/control_flow_predicate.hpp"

#include "kettle_internal/io/io_control_flow.hpp"

namespace ket::internal::format
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
template auto format_csv_integers_<int>(const std::vector<int>& integers) -> std::string;
template auto format_csv_integers_<std::size_t>(const std::vector<std::size_t>& integers) -> std::string;

auto format_control_flow_predicate_(const ket::ControlFlowPredicate& predicate) -> std::string
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

auto format_classical_if_statement_header_(
    const ket::ControlFlowPredicate& predicate
) -> std::string
{
    return std::string {"IF "} + format_control_flow_predicate_(predicate);
}

auto format_classical_if_else_statement_header_(
    const ket::ControlFlowPredicate& predicate
) -> std::tuple<std::string, std::string>
{
    const auto if_part = std::string {"IF "} + format_control_flow_predicate_(predicate);
    const auto else_part = std::string {"ELSE"};

    return {if_part, else_part};
}

}  // namespace ket::internal::format


namespace ket::internal::parse
{

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
template auto parse_csv_in_brackets_<int>(std::stringstream& stream) -> std::vector<int>;
template auto parse_csv_in_brackets_<std::size_t>(std::stringstream& stream) -> std::vector<std::size_t>;

auto parse_comparison_sign_(std::stringstream& stream) -> ket::ControlFlowBooleanKind
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

void discard_until_char_(std::stringstream& stream, char delimiter)
{
    char ch {};
    while (stream.peek() != EOF && stream.peek() != delimiter) {
        stream.get(ch);
    }
}

auto parse_control_flow_predicate_(std::stringstream& stream) -> ket::ControlFlowPredicate
{
    // remove the BITS
    discard_until_char_(stream, '[');

    auto bit_indices_to_check = parse_csv_in_brackets_<std::size_t>(stream);
    const auto control_kind = parse_comparison_sign_(stream);

    discard_until_char_(stream, '[');

    auto expected_bits = parse_csv_in_brackets_<int>(stream);

    return ket::ControlFlowPredicate {std::move(bit_indices_to_check), std::move(expected_bits), control_kind};
}

}  // namespace ket::internal::parse
