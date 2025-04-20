#pragma once

#include <cmath>
#include <complex>
#include <random>

#include "mini-qiskit/common/matrix2x2.hpp"
#include "mini-qiskit/common/prng.hpp"


namespace mqis
{

/*
    Generate a random 2x2 unitary matrix, taking the PRNG directly.

    SOURCE: http://home.lu.lv/~sd20008/papers/essays/Random%20unitary%20[paper].pdf
      - page 5, sampling from U(2)
*/
inline auto generate_random_state(
    std::size_t n_qubits,
    std::mt19937& prng
) -> mqis::Matrix2X2
{
    // NOTE: the variable names for the angles don't have any general meaning
    auto uniform = std::uniform_real_distribution<double> {0.0, 1.0};

    const auto alpha = 2.0 * M_PI * uniform(prng);
    const auto phi = 2.0 * M_PI * uniform(prng);
    const auto chi = 2.0 * M_PI * uniform(prng);
    const auto psi = std::asin(std::sqrt(uniform(prng)));

    const auto cospsi = std::cos(psi);
    const auto sinpsi = std::sin(psi);
    
    const auto gphase = std::complex {std::cos(alpha), std::sin(alpha)};
    const auto expphi = std::complex {std::cos(phi), std::sin(phi)};
    const auto expchi = std::complex {std::cos(chi), std::sin(chi)};

    const auto elem00 =   gphase * cospsi * expphi;
    const auto elem01 =   gphase * sinpsi * expchi;
    const auto elem10 = - gphase * sinpsi / expchi;
    const auto elem11 =   gphase * cospsi / expphi;

    return {elem00, elem01, elem10, elem11};
}

}  // namespace mqis
