#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

#include "kettle/state/state.hpp"

namespace ket
{

/*
    Takes a quantum state described by `statevector`, collapses the state at the qubits
    given by `qubit_indices` into the bits given by `expected_measurements`, and returns
    the renormalized state assuming that the measured bits have been marginalized away.

    Throws a `std::runtime_error` if the number of qubit indices does not match the number
    of expected measurements, if the expected measurements are invalid, or if the marginalized
    state cannot be renormalized.
*/
inline auto measure_and_collapse(
    const QuantumState& statevector,
    const std::vector<std::size_t>& qubit_indices,
    const std::vector<std::uint8_t>& expected_measurements
) -> QuantumState
{
    if (qubit_indices.size() != expected_measurements.size()) {
        throw std::runtime_error {"ERROR: mismatch in number of qubit indices and number of expected measurements.\n"};
    }

    const auto is_binary = [](std::uint8_t x) { return x == 0 || x == 1; };
    if (!std::ranges::all_of(expected_measurements, is_binary)) {
        throw std::runtime_error {"ERROR: all expected measurements must be 0 or 1.\n"};
    }

    if (std::ranges::max(qubit_indices) >= statevector.n_qubits()) {
        throw std::runtime_error {"ERROR: qubit indices exceed the number of qubits in the statevector.\n"};
    }

    /*
        PLAN:
        - use SingleQubitGatePairGenerator to loop over each qubit index
          - find the expected measurement, and set the other state amplitude to {0.0, 0.0}
        - loop over all the remaining qubits NOT in the provided vector of qubit indices
          - calculate the new norm
        - create a new QuantumState with the new number of qubits
        - loop over all the remaining qubits NOT in the provided vector of qubit indices
          - copy the new normalized amplitudes into the new QuantumState object
        - return
    */
}

}  // namespace ket
