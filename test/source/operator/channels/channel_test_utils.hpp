#pragma once

#include "kettle/circuit/circuit.hpp"
#include "kettle/simulation/simulate_density_matrix.hpp"
#include "kettle/state/density_matrix.hpp"

namespace ctestutils
{

/*
    A state that isn't completely trivial but also not arbitrary, used in several unit tests
*/
inline auto basic_state0() -> ket::DensityMatrix
{
    auto circuit = ket::QuantumCircuit {1};
    circuit.add_h_gate(0);
    circuit.add_ry_gate(0, 0.15 * M_PI);
    circuit.add_rx_gate(0, 0.25 * M_PI);

    auto state_ = ket::DensityMatrix {"0"};
    ket::simulate(circuit, state_);

    return state_;
}


/*
    Another state that isn't completely trivial but also not arbitrary, used in several unit tests
*/
inline auto basic_state1() -> ket::DensityMatrix
{
    auto circuit = ket::QuantumCircuit {1};
    circuit.add_h_gate(0);
    circuit.add_rz_gate(0, 0.15 * M_PI);
    circuit.add_s_gate(0);

    auto state_ = ket::DensityMatrix {"0"};
    ket::simulate(circuit, state_);

    return state_;
}


inline auto mat2x2_to_eigen(const ket::Matrix2X2& matrix) -> Eigen::MatrixXcd
{
    auto output = Eigen::MatrixXcd(2, 2);
    output(0, 0) = matrix.elem00;
    output(0, 1) = matrix.elem01;
    output(1, 0) = matrix.elem10;
    output(1, 1) = matrix.elem11;

    return output;
}

inline auto eigen_to_mat2x2(const Eigen::MatrixXcd& matrix) -> ket::Matrix2X2
{
    if (matrix.cols() != 2 || matrix.rows() != 2) {
        throw std::runtime_error {"ERROR: can only do this for a 2x2 matrix.\n"};
    }

    return ket::Matrix2X2 {
        .elem00=matrix(0, 0),
        .elem01=matrix(0, 1),
        .elem10=matrix(1, 0),
        .elem11=matrix(1, 1),
    };
}


}  // namespace ctestutils