#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

#include "kettle/common/tolerance.hpp"
#include "kettle/state/statevector.hpp"


namespace ket
{

/*
    Takes a quantum state described by `statevector`, and projects it onto the subspace
    for which the qubits given by `qubit_indices` have bit values given by `expected_measurements`.
*/
auto project_statevector(
    const Statevector& statevector,
    const std::vector<std::size_t>& qubit_indices,
    const std::vector<std::uint8_t>& expected_measurements,
    double minimum_norm_tol = ket::PROJECTION_NORMALIZATION_TOLERANCE
) -> Statevector;

}  // namespace ket
