#include <algorithm>
#include <initializer_list>
#include <numeric>
#include <vector>

#include "kettle/operator/channels/pauli_channel.hpp"
#include "kettle/operator/pauli/sparse_pauli_string.hpp"

#include "kettle_internal/operator/pauli/pauli_common.hpp"


namespace
{

auto check_nonempty_(const std::vector<ket::ProbabilisticPauliString>& weighted_pauli_strings)
{
    if (weighted_pauli_strings.size() == 0) {
        throw std::runtime_error {
            "ERROR: construction of `PauliChannel` with only ProbabilisticPauliString instances requires\n"
            "a non-empty vector.\n"
        };
    }
}

auto check_all_strings_same_length_(const std::vector<ket::ProbabilisticPauliString>& strings)
{
    const auto has_nonequal_n_qubits = [](const auto& left, const auto& right) -> bool {
        return left.pauli_string.n_qubits() != right.pauli_string.n_qubits();
    };

    if (std::ranges::adjacent_find(strings, has_nonequal_n_qubits) != strings.end()) {
        throw std::runtime_error {
            "ERROR: construction of `PauliChannel` with ProbabilisticPauliString instances requires\n"
            "all pauli strings to have the same number of qubits.\n"
        };
    }

}

auto check_probabilities_add_up_to_1_(const std::vector<ket::ProbabilisticPauliString>& strings, double tolerance)
{
    const auto add_string_prob = [](double summ, const auto& string) { return summ + string.coefficient; };
    const auto total_probability = std::accumulate(strings.begin(), strings.end(), 0.0, add_string_prob);

    if (std::abs(total_probability - 1.0) > tolerance) {
        throw std::runtime_error {
            "ERROR: the sum of all coefficients for Pauli strings in the PauliChannel must add up to 1.\n"
        };
    }
}

}  // namespace


namespace ket
{

// NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
PauliChannel::PauliChannel(
    std::vector<ProbabilisticPauliString> weighted_pauli_strings,
    double tolerance
)
    : n_qubits_ {0}
    , weighted_pauli_strings_ {std::move(weighted_pauli_strings)}
{
    check_nonempty_(weighted_pauli_strings_);
    check_all_strings_same_length_(weighted_pauli_strings_);
    check_probabilities_add_up_to_1_(weighted_pauli_strings_, tolerance);

    n_qubits_ = weighted_pauli_strings_[0].pauli_string.n_qubits();
}

PauliChannel::PauliChannel(
    const std::initializer_list<ProbabilisticPauliString>& weighted_pauli_strings,
    double tolerance
)
    : PauliChannel {std::vector<ProbabilisticPauliString> {weighted_pauli_strings}, tolerance}
{}


auto almost_eq(
    const PauliChannel& left_op,
    const PauliChannel& right_op,
    double coeff_tolerance
) -> bool
{
    return ket::internal::almost_eq_pauli_helper_(left_op, right_op, coeff_tolerance);
}

}  // namespace ket
