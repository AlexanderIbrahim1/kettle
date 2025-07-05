#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

#include "kettle/state/statevector.hpp"


namespace ket::internal
{

/*
    Returns whether or not the qubit at `index` of a computational basis state given by `value` is 1.
    The endianness does not matter.
*/
auto is_index_set_(std::size_t index, std::size_t value) -> bool;

class QubitStateChecker_
{
public:
    QubitStateChecker_(
        std::vector<std::size_t> qubit_indices,
        std::vector<std::uint8_t> expected_measurements
    );

    [[nodiscard]]
    auto all_indices_match(std::size_t value) const -> bool;

private:
    std::vector<std::size_t> qubit_indices_;
    std::vector<std::uint8_t> expected_measurements_;
};

/*
    Copy the amplitudes from the old statevector to the new statevector in the project-onto subspace.
*/
auto copy_projected_amplitudes_(
    const ket::Statevector& statevector,
    const std::vector<std::size_t>& qubit_indices,
    const std::vector<std::uint8_t>& expected_measurements
) -> std::vector<std::complex<double>>;

/*
    Normalize the new amplitudes; if normalization is impossible, throw a `std::runtime_error`.
*/
void normalize_amplitudes_(std::vector<std::complex<double>>& amplitudes, double minimum_norm_tol);

}  // namespace ket::internal
