#pragma once

#include <cmath>
#include <random>

#include "kettle/common/matrix2x2.hpp"


namespace ket
{

/*
    Generate a random 2x2 unitary matrix, taking the PRNG directly.

    SOURCE: http://home.lu.lv/~sd20008/papers/essays/Random%20unitary%20[paper].pdf
      - page 5, sampling from U(2)
*/
auto generate_random_unitary2x2(std::mt19937& prng) -> ket::Matrix2X2;

/*
    Generate a random 2x2 unitary matrix, generating a fresh PRNG using the provided seed.

    SOURCE: http://home.lu.lv/~sd20008/papers/essays/Random%20unitary%20[paper].pdf
      - page 5, sampling from U(2)
*/
auto generate_random_unitary2x2(int seed) -> ket::Matrix2X2;

/*
    Generate a random 2x2 unitary matrix, generating the PRNG from the random device.

    SOURCE: http://home.lu.lv/~sd20008/papers/essays/Random%20unitary%20[paper].pdf
      - page 5, sampling from U(2)
*/
auto generate_random_unitary2x2() -> ket::Matrix2X2;

}  // namespace ket
