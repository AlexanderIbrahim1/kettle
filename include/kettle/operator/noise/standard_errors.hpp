#pragma once

#include "kettle/operator/channels/pauli_channel.hpp"
#include "kettle/common/utils.hpp"

/*
    This header contains the functions for implementing the following error types:
      - symmetric depolarizing error (as a Pauli Channel)
      - amplitude error (as a Kraus channel)
      - phase damping error (as a Kraus channel)
      - phase-amplitude damping error (as a Kraus channel)
      - thermal relaxation error (as a Kraus channel)
      - reset error (as a Mixed Circuit channel)
      - coherent unitary error (as a Mixed Circuit channel)
    
    Some error types aren't implemented as functions; you should call the constructors directly:
      - general pauli errors (as a PauliChannel)
      - mixed unitary error (as a MixedCircuitChannel)
*/

namespace ket
{

/*
    The symmetric depolarizing error channel applied to a single qubit.

    For this definition:
      - p = 0 gives a noiseless channel
      - p = 3/4 gives a full depolarized channel, and the output will be proportional to the identity matrix
      - p = 1 gives the uniform Pauli error channel, where X, Y, and Z are applied equally to the 1-qubit density matrix
*/
template <QubitIndices Container = QubitIndicesIList>
auto symmetric_depolarizing_error_channel(
    double parameter,
    std::size_t n_qubits,
    const Container& indices
) -> ket::PauliChannel;

}  // namespace ket