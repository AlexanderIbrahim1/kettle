#include <cmath>
#include <complex>
#include <random>

#include "kettle/common/matrix2x2.hpp"
#include "kettle/gates/random_u_gates.hpp"
#include "kettle_internal/common/prng.hpp"


namespace ket
{

auto generate_random_unitary2x2(std::mt19937& prng) -> ket::Matrix2X2
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

    return {.elem00=elem00, .elem01=elem01, .elem10=elem10, .elem11=elem11};
}

auto generate_random_unitary2x2(int seed) -> ket::Matrix2X2
{
    auto prng = ket::internal::get_prng_(seed);
    return generate_random_unitary2x2(prng);
}

auto generate_random_unitary2x2() -> ket::Matrix2X2
{
    // NOTE: creating a single function with the function signature
    //     `generate_random_unitary2x2(std::optional<int> seed = std::nullopt)`
    // causes an infinite self-recursion loop
    //
    // I guess an instance of `std::mt19937` gets picked up as `std::optional<int>`???
    // the point is this is why there are two separate functions
    auto prng = ket::internal::get_prng_(std::nullopt);
    return generate_random_unitary2x2(prng);
}

}  // namespace ket
