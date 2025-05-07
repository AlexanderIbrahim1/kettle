#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

#include "kettle/state/state.hpp"
#include "kettle/state/project_state.hpp"


namespace ket::internal
{

auto is_index_set_(std::size_t index, std::size_t value) -> bool
{
    return (value & (1UL << index)) != 0;
}

QubitStateChecker_::QubitStateChecker_(
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
auto QubitStateChecker_::all_indices_match(std::size_t value) const -> bool
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

auto copy_projected_amplitudes_(
    const ket::QuantumState& statevector,
    const std::vector<std::size_t>& qubit_indices,
    const std::vector<std::uint8_t>& expected_measurements
) -> std::vector<std::complex<double>>
{
    const auto new_n_qubits = statevector.n_qubits() - qubit_indices.size();
    const auto new_n_states = 1UL << new_n_qubits;

    auto new_amplitudes = std::vector<std::complex<double>> {};
    new_amplitudes.reserve(new_n_states);

    const auto checker = QubitStateChecker_ {qubit_indices, expected_measurements};

    for (std::size_t i_state {0}; i_state < statevector.n_states(); ++i_state) {
        if (checker.all_indices_match(i_state)) {
            new_amplitudes.emplace_back(statevector[i_state]);
        }
    }

    return new_amplitudes;
}

void normalize_amplitudes_(std::vector<std::complex<double>>& amplitudes, double minimum_norm_tol)
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

}  // namespace ket::internal


namespace ket
{

auto project_statevector(
    const QuantumState& statevector,
    const std::vector<std::size_t>& qubit_indices,
    const std::vector<std::uint8_t>& expected_measurements,
    double minimum_norm_tol
) -> QuantumState
{
    namespace ki = ket::internal;

    if (qubit_indices.size() == statevector.n_qubits()) {
        throw std::runtime_error {"ERROR: cannot collapse all qubits, remaining state won't exist.\n"};
    }

    if (std::ranges::max(qubit_indices) >= statevector.n_qubits()) {
        throw std::runtime_error {"ERROR: qubit indices exceed the number of qubits in the statevector.\n"};
    }

    auto new_amplitudes = ki::copy_projected_amplitudes_(statevector, qubit_indices, expected_measurements);
    ki::normalize_amplitudes_(new_amplitudes, minimum_norm_tol);

    return QuantumState {new_amplitudes};
}

}  // namespace ket
