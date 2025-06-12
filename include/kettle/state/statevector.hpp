#pragma once

#include <complex>
#include <string>
#include <vector>

#include "kettle/common/tolerance.hpp"
#include "kettle/state/endian.hpp"
#include "kettle/state/qubit_state_conversion.hpp"

namespace ket
{

class Statevector
{
public:
    /*
        The default constructor sets the initial state to the |0000...0> state; this means the entire
        weight is on the 0th element. The global phase factor is ignored.

        The 0 state is the same in both the little and big endian representations, so it isn't needed
        in this constructor.
    */
    explicit Statevector(std::size_t n_qubits);

    explicit Statevector(
        std::vector<std::complex<double>> coefficients,
        Endian input_endian = Endian::LITTLE,
        double normalization_tolerance = ket::CONSTRUCTION_NORMALIZATION_TOLERANCE
    );

    explicit Statevector(
        const std::string& computational_state,
        Endian input_endian = Endian::LITTLE
    );

    constexpr auto operator[](std::size_t index) const noexcept -> const std::complex<double>&
    {
        return coefficients_[index];
    }

    constexpr auto operator[](std::size_t index) noexcept -> std::complex<double>&
    {
        return coefficients_[index];
    }

    [[nodiscard]]
    auto at(std::size_t index) const -> const std::complex<double>&
    {
        check_index_(index);
        return coefficients_[index];
    }

    auto at(std::size_t index) -> std::complex<double>&
    {
        check_index_(index);
        return coefficients_[index];
    }

    [[nodiscard]]
    auto at(
        const std::string& bitstring,
        Endian endian = Endian::LITTLE
    ) const -> const std::complex<double>&
    {
        const auto state_index = bitstring_to_state_index(bitstring, endian);
        check_index_(state_index);
        return coefficients_[state_index];
    }

    auto at(
        const std::string& bitstring,
        Endian endian = Endian::LITTLE
    ) -> std::complex<double>&
    {
        const auto state_index = bitstring_to_state_index(bitstring, endian);
        check_index_(state_index);
        return coefficients_[state_index];
    }

    [[nodiscard]]
    constexpr auto n_states() const noexcept -> std::size_t
    {
        return n_states_;
    }

    [[nodiscard]]
    constexpr auto n_qubits() const noexcept -> std::size_t
    {
        return n_qubits_;
    }

private:
    std::size_t n_qubits_;
    std::size_t n_states_;
    std::vector<std::complex<double>> coefficients_;

    void check_power_of_2_with_at_least_one_qubit_() const;

    void check_normalization_of_coefficients_(double normalization_tolerance) const;

    void check_index_(std::size_t index) const;

    void check_at_least_one_qubit_() const;

    void perform_endian_flip_on_coefficients_() noexcept;
};

auto almost_eq(
    const Statevector& left,
    const Statevector& right,
    double tolerance_sq = ket::COMPLEX_ALMOST_EQ_TOLERANCE_SQ
) noexcept -> bool;

auto tensor_product(const Statevector& left, const Statevector& right) -> Statevector;

auto inner_product(const Statevector& bra_state, const Statevector& ket_state) -> std::complex<double>;

auto diagonal_expectation_value(const std::vector<std::complex<double>>& eigenvalues, const Statevector& state) -> std::complex<double>;

auto inner_product_norm_squared(const Statevector& left, const Statevector& right) -> double;

}  // namespace ket
