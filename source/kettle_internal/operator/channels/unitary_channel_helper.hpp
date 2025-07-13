#pragma once

#include <algorithm>
#include <format>
#include <numeric>
#include <string_view>
#include <vector>


namespace ket::internal
{

template <typename T>
void check_nonempty_(
    const std::vector<T>& weighted_unitaries,
    std::string_view channel_name
)
{
    if (weighted_unitaries.size() == 0) {
        const auto err_msg = std::format(
            "ERROR: cannot construct `{}` with a non-empty container.\n", channel_name
        );
        throw std::runtime_error {err_msg};
    }
}

template <typename T, typename NQubitsGetter>
void check_unitaries_have_same_n_qubits_(
    const std::vector<T>& strings,
    NQubitsGetter getter,
    std::string_view channel_name
)
{
    const auto has_nonequal_n_qubits = [&](const auto& left, const auto& right) -> bool {
        return getter(left) != getter(right);
    };

    if (std::ranges::adjacent_find(strings, has_nonequal_n_qubits) != strings.end()) {
        const auto err_msg = std::format(
            "ERROR: all unitaries in `{}` must have the same number of qubits.\n", channel_name
        );
        throw std::runtime_error {err_msg};
    }
}

template <typename T, typename CoefficientGetter>
void check_probabilities_add_up_to_1_(
    const std::vector<T>& strings,
    CoefficientGetter getter,
    double tolerance,
    std::string_view channel_name
)
{
    const auto add_coeff = [&](double summ, const auto& elem) { return summ + getter(elem); };
    const auto coeff_sum = std::accumulate(strings.begin(), strings.end(), 0.0, add_coeff);

    if (std::abs(coeff_sum - 1.0) > tolerance) {
        const auto err_msg = std::format(
            "ERROR: the sum of all coefficients for unitaries in `{}` must add up to 1.\n", channel_name
        );
        throw std::runtime_error {err_msg};
    }
}

inline void check_number_of_qubits_is_nonzero_(std::size_t n_qubits, std::string_view channel_name)
{
    if (n_qubits == 0) {
        const auto err_msg = std::format(
            "ERROR: the number of qubits in the `{}` cannot be zero.\n", channel_name
        );
        throw std::runtime_error {err_msg};
    }
}

}  // namespace ket::internal
