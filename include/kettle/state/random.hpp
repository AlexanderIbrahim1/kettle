#pragma once

#include <cstddef>
#include <random>

#include "kettle/state/statevector.hpp"


namespace ket
{

/*
    Generate a random `Statevector` instance, taking the PRNG directly.
*/
auto generate_random_state(std::size_t n_qubits, std::mt19937& prng) -> Statevector;

/*
    Generate a random `Statevector` instance, generating a fresh PRNG using the provided seed.
*/
auto generate_random_state(std::size_t n_qubits, int seed) -> Statevector;

/*
    Generate a random `Statevector` instance, generating the PRNG from the random device.
*/
auto generate_random_state(std::size_t n_qubits) -> Statevector;

}  // namespace ket
