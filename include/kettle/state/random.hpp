#pragma once

#include <cstddef>
#include <random>

#include "kettle/state/state.hpp"


namespace ket
{

/*
    Generate a random `QuantumState` instance, taking the PRNG directly.
*/
auto generate_random_state(std::size_t n_qubits, std::mt19937& prng) -> QuantumState;

/*
    Generate a random `QuantumState` instance, generating a fresh PRNG using the provided seed.
*/
auto generate_random_state(std::size_t n_qubits, int seed) -> QuantumState;

/*
    Generate a random `QuantumState` instance, generating the PRNG from the random device.
*/
auto generate_random_state(std::size_t n_qubits) -> QuantumState;

}  // namespace ket
