#pragma once

#include "kettle/operator/pauli/sparse_pauli_string.hpp"


namespace ket
{

// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init, hicpp-member-init)
struct ProbabilisticPauliString
{
    double coefficient;
    SparsePauliString pauli_string;
};

class PauliChannel
{
public:
    /*
        NOTE: this function shares a lot of similar code with `PauliOperator`, but:
          - there are functions that make sense for `PauliOperator` that don't make sense for `PauliChannel`
          - the coefficients have different types and restrictions
          - there isn't enough code to justify the increased complexity of uisng mixins, or forcing
            the user to use templates
        
        So I'll just deal with the code duplication
    */

    // TODO: get rid of magic number
    // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
    PauliChannel(std::vector<ProbabilisticPauliString> weighted_pauli_strings, double tolerance = 1.0e-6);

    // TODO: get rid of magic number
    PauliChannel(const std::initializer_list<ProbabilisticPauliString>& weighted_pauli_strings, double tolerance = 1.0e-6);

    [[nodiscard]]
    constexpr auto n_qubits() const noexcept -> std::size_t
    {
        return n_qubits_;
    }

    [[nodiscard]]
    constexpr auto size() const noexcept -> std::size_t
    {
        return weighted_pauli_strings_.size();
    }

    [[nodiscard]]
    constexpr auto weighted_pauli_strings() const noexcept -> const std::vector<ProbabilisticPauliString>&
    {
        return weighted_pauli_strings_;
    }

    [[nodiscard]]
    auto at(std::size_t index) const -> const ProbabilisticPauliString&
    {
        return weighted_pauli_strings_[index];
    }

    [[nodiscard]]
    auto at(std::size_t index) -> ProbabilisticPauliString&
    {
        return weighted_pauli_strings_[index];
    }

private:
    std::size_t n_qubits_;
    std::vector<ProbabilisticPauliString> weighted_pauli_strings_;
};


// TODO: replace magic number
auto almost_eq(
    const PauliChannel& left_op,
    const PauliChannel& right_op,
    double coeff_tolerance = 1.0e-6
) -> bool;

}  // namespace ket
