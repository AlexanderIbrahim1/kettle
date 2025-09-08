#pragma once

#include <Eigen/Dense>

#include "kettle/common/matrix2x2.hpp"
#include "kettle/operator/noise/standard_errors.hpp"
#include "kettle/state/density_matrix.hpp"

namespace ctestutils
{

inline auto result_depolarizing_noise_1qubit(
    const ket::Matrix2X2& dens_mat,
    double parameter
) -> ket::Matrix2X2
{
    const auto elem00 = dens_mat.elem00 * (1.0 - (4.0 * parameter / 3.0)) + (2.0 * parameter / 3.0);
    const auto elem01 = dens_mat.elem01 * (1.0 - (4.0 * parameter / 3.0));
    const auto elem10 = dens_mat.elem10 * (1.0 - (4.0 * parameter / 3.0));
    const auto elem11 = dens_mat.elem11 * (1.0 - (4.0 * parameter / 3.0)) + (2.0 * parameter / 3.0);

    return {.elem00=elem00, .elem01=elem01, .elem10=elem10, .elem11=elem11};
}

inline auto result_amplitude_damping_2qubit(
    const ket::DensityMatrix& state,
    double parameter
) -> ket::DensityMatrix
{
    if (state.n_qubits() != 2) {
        throw std::runtime_error {"ERROR: this function requires a 2-qubit state.\n"};
    }

    if (parameter < 0.0 || parameter > 1.0) {
        throw std::runtime_error {"ERROR: amplitude damping parameter must lie in [0.0, 1.0].\n"};
    }

    // NOTES:
    // - parameters don't have meaningful names, and there isn't a "clean" way to write this AFAIK
    // - each of these powers appears multiple times; preparing these variables makes it easier to read
    const auto eta1 = std::sqrt(1.0 - parameter);
    const auto eta2 = eta1 * eta1;
    const auto eta3 = eta1 * eta1 * eta1;
    const auto eta4 = eta2 * eta2;

    const auto lam1 = std::sqrt(parameter);
    const auto lam2 = lam1 * lam1;
    const auto lam4 = lam2 * lam2;

    const auto& densmat = state.matrix();

    auto densmat_after = Eigen::MatrixXcd(4, 4);

    densmat_after << \
        // row 0
        densmat(0, 0) + lam2 * (densmat(1, 1) + densmat(2, 2)) + lam4 * densmat(3, 3), 
        eta1 * densmat(0, 1) + eta1 * lam2 * densmat(1, 3),
        eta1 * densmat(0, 2) + eta1 * lam2 * densmat(2, 3),
        eta2 * densmat(0, 3),
        // row 1
        eta1 * densmat(1, 0) + eta1 * lam2 * densmat(3, 1),
        eta2 * densmat(1, 1) + eta2 * lam2 * densmat(3, 3),
        eta2 * densmat(1, 2),
        eta3 * densmat(1, 3),
        // row 2
        eta1 * densmat(2, 0) + eta1 * lam2 * densmat(3, 2),
        eta2 * densmat(2, 1),
        eta2 * densmat(2, 2) + eta2 * lam2 * densmat(3, 3),
        eta3 * densmat(2, 3),
        // row 3
        eta2 * densmat(3, 0),
        eta3 * densmat(3, 1),
        eta3 * densmat(3, 2),
        eta4 * densmat(3, 3);

    return ket::DensityMatrix {densmat_after};
}

inline auto result_phase_amplitude_damping_1qubit(
    const ket::Matrix2X2& dens_mat,
    const ket::PhaseAmplitudeDampingParameters parameters
) -> ket::Matrix2X2
{
    auto check_in_0_1 = [](double param) {
        if (param < 0.0 || param > 1.0) {
            throw std::runtime_error {"ERROR: found out-of-bounds parameter in unit test\n"};
        }
    };

    check_in_0_1(parameters.amplitude);
    check_in_0_1(parameters.phase);
    check_in_0_1(parameters.excited_population);
    check_in_0_1(parameters.amplitude + parameters.phase);

    const auto [amp, pha, pop] = parameters;
    const auto offdiag = std::sqrt(1.0 - amp - pha);

    const auto output00 = (1.0 - (pop * amp)) * dens_mat.elem00 + (1.0 - pop) * amp * dens_mat.elem11;
    const auto output01 = offdiag * dens_mat.elem01;
    const auto output10 = offdiag * dens_mat.elem10;
    const auto output11 = (1.0 + (pop * amp) - amp) * dens_mat.elem11 + pop * amp * dens_mat.elem00;

    return {.elem00=output00, .elem01=output01, .elem10=output10, .elem11=output11};
}

}  // namespace ctestutils