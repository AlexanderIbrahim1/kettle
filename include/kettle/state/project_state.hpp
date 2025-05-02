#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

#include "kettle/common/mathtools.hpp"
#include "kettle/state/state.hpp"


namespace impl_ket
{

/*
    Returns whether or not the qubit at `index` of a computational basis state given by `value` is 1.
    The endianness does not matter.
*/
inline auto is_index_set_(std::size_t index, std::size_t value) -> bool
{
    return (value & (1UL << index)) != 0;
}

class QubitStateChecker_
{
public:
    QubitStateChecker_(
        std::vector<std::size_t> qubit_indices,
        std::vector<std::uint8_t> expected_measurements
    )
        : qubit_indices_ {std::move(qubit_indices)}
        , expected_measurements_ {std::move(expected_measurements)}
    {
        if (!std::ranges::all_of(expected_measurements, [](std::uint8_t x) {return x == 0 || x == 1; })) {
            throw std::runtime_error {"ERROR: all expected measurements must be 0 or 1.\n"};
        }

        if (qubit_indices.size() != expected_measurements.size()) {
            throw std::runtime_error {"ERROR: mismatch in number of qubit indices and number of expected measurements.\n"};
        }
    }

    [[nodiscard]]
    constexpr auto all_indices_match(std::size_t value) const -> bool
    {
        for (std::size_t i {0}; i < qubit_indices_.size(); ++i)
        {
            const auto binary_at_index = static_cast<std::uint8_t>(is_index_set_(qubit_indices_[i], value));

            if (binary_at_index != expected_measurements_[i]) {
                return false;
            }
        }

        return true;
    }

private:
    std::vector<std::size_t> qubit_indices_;
    std::vector<std::uint8_t> expected_measurements_;
};

/*
    Copy the amplitudes from the old statevector to the new statevector in the project-onto subspace.
*/
inline auto copy_projected_amplitudes_(
    const ket::QuantumState& statevector,
    const std::vector<std::size_t>& qubit_indices,
    const std::vector<std::uint8_t>& expected_measurements
) -> std::vector<std::complex<double>>
{
    const auto new_n_qubits = statevector.n_qubits() - qubit_indices.size();
    const auto new_n_states = 1UL << new_n_qubits;

    auto new_amplitudes = std::vector<std::complex<double>> {};
    new_amplitudes.reserve(new_n_states);

    const auto checker = impl_ket::QubitStateChecker_ {qubit_indices, expected_measurements};

    for (std::size_t i_state {0}; i_state < statevector.n_states(); ++i_state) {
        if (checker.all_indices_match(i_state)) {
            new_amplitudes.emplace_back(statevector[i_state]);
        }
    }

    return new_amplitudes;
}

/*
    Normalize the new amplitudes; if normalization is impossible, throw a `std::runtime_error`.
*/
inline void normalize_amplitudes_(std::vector<std::complex<double>>& amplitudes, double minimum_norm_tol)
{
    auto sum_of_square_amplitudes = double {0.0};
    for (std::size_t i_state {0}; i_state < amplitudes.size(); ++i_state) {
        sum_of_square_amplitudes += std::norm(amplitudes[i_state]);
    }

    if (sum_of_square_amplitudes < minimum_norm_tol) {
        throw std::runtime_error {"The state cannot be normalized; norm of remaining amplitudes is 0.0\n"};
    }

    const auto norm = std::sqrt(1.0 / sum_of_square_amplitudes);

    for (auto& amp : amplitudes) {
        amp *= norm;
    }
}

}  // namespace impl_ket


namespace ket
{

/*
    Takes a quantum state described by `statevector`, and projects it onto the subspace
    for which the qubits given by `qubit_indices` have bit values given by `expected_measurements`.
*/
inline auto project_statevector(
    const QuantumState& statevector,
    const std::vector<std::size_t>& qubit_indices,
    const std::vector<std::uint8_t>& expected_measurements,
    double minimum_norm_tol = impl_ket::NORMALIZATION_TOLERANCE
) -> QuantumState
{
    if (qubit_indices.size() == statevector.n_qubits()) {
        throw std::runtime_error {"ERROR: cannot collapse all qubits, remaining state won't exist.\n"};
    }

    if (std::ranges::max(qubit_indices) >= statevector.n_qubits()) {
        throw std::runtime_error {"ERROR: qubit indices exceed the number of qubits in the statevector.\n"};
    }

    auto new_amplitudes = impl_ket::copy_projected_amplitudes_(statevector, qubit_indices, expected_measurements);
    impl_ket::normalize_amplitudes_(new_amplitudes, minimum_norm_tol);

    return QuantumState {new_amplitudes};
}

}  // namespace ket
