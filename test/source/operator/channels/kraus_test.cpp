#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <Eigen/Dense>

#include "kettle/circuit/circuit.hpp"
#include "kettle/common/matrix2x2.hpp"
#include "kettle/operator/channels/multi_qubit_kraus_channel.hpp"
#include "kettle/operator/channels/one_qubit_kraus_channel.hpp"
#include "kettle/simulation/simulate_density_matrix.hpp"
#include "kettle/state/density_matrix.hpp"

#include "kettle_internal/common/state_test_utils.hpp"
#include "kettle_internal/simulation/density_matrix_through_channel.hpp"

namespace ki = ket::internal;

namespace
{

auto result_depolarizing_noise_1qubit(
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

auto apply_depolarizing_noise_1qubit(
    const ket::Matrix2X2& dens_mat,
    const ket::OneQubitKrausChannel& channel
) -> ket::Matrix2X2
{
    auto output = ket::Matrix2X2 {};
    for (const auto& mat : channel.matrices()) {
        output += (mat * dens_mat * ket::conjugate_transpose(mat));
    }

    return output;
}

auto mat2x2_to_eigen(const ket::Matrix2X2& matrix) -> Eigen::MatrixXcd
{
    auto output = Eigen::MatrixXcd(2, 2);
    output(0, 0) = matrix.elem00;
    output(0, 1) = matrix.elem01;
    output(1, 0) = matrix.elem10;
    output(1, 1) = matrix.elem11;

    return output;
}

auto result_amplitude_damping_2qubit(
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

}  // namespace


TEST_CASE("Kraus channel depolarizing noise")
{
    const auto parameter = GENERATE(0.2, 0.4, 0.6, 0.75, 1.0);

    SECTION("manual application of Matrix2X2 instances above")
    {
        const auto depol_channel = ket::depolarizing_noise(parameter, 0);

        auto circuit = ket::QuantumCircuit {1};
        circuit.add_h_gate(0);
        circuit.add_ry_gate(0, 0.15 * M_PI);
        circuit.add_rx_gate(0, 0.25 * M_PI);

        auto state = ket::DensityMatrix {"0"};
        ket::simulate(circuit, state);

        const auto matrix = ket::Matrix2X2 {
            .elem00=state.matrix()(0, 0),
            .elem01=state.matrix()(0, 1),
            .elem10=state.matrix()(1, 0),
            .elem11=state.matrix()(1, 1),
        };

        const auto actual = apply_depolarizing_noise_1qubit(matrix, depol_channel);
        const auto expected = result_depolarizing_noise_1qubit(matrix, parameter);

        REQUIRE(ket::almost_eq(actual, expected));
    }

    SECTION("using `simulate_one_qubit_kraus_channel()`")
    {
        const auto depol_channel = ket::depolarizing_noise(parameter, 0);

        // state should be simple but not completely arbitrary, so we don't use a random state
        auto state = [&]() {
            auto circuit = ket::QuantumCircuit {1};
            circuit.add_h_gate(0);
            circuit.add_ry_gate(0, 0.15 * M_PI);
            circuit.add_rx_gate(0, 0.25 * M_PI);

            auto state_ = ket::DensityMatrix {"0"};
            ket::simulate(circuit, state_);

            return state_;
        }();

        // naming doesn't matter; buffers play different roles within the function
        auto buffer0 = Eigen::MatrixXcd(2, 2);
        auto buffer1 = Eigen::MatrixXcd(2, 2);
        auto buffer2 = Eigen::MatrixXcd(2, 2);

        // the only pair of indices for a 2-qubit system is (0, 1)
        const auto n_single_gate_pairs = Eigen::Index {1};
        const auto single_pair = ki::FlatIndexPair<Eigen::Index> {.i_lower=0, .i_upper=n_single_gate_pairs};

        const auto matrix = ket::Matrix2X2 {
            .elem00=state.matrix()(0, 0),
            .elem01=state.matrix()(0, 1),
            .elem10=state.matrix()(1, 0),
            .elem11=state.matrix()(1, 1),
        };

        ket::simulate_one_qubit_kraus_channel(state, depol_channel, single_pair, buffer0, buffer1, buffer2);

        const auto expected_mat = result_depolarizing_noise_1qubit(matrix, parameter);
        const auto expected_state = ket::DensityMatrix {mat2x2_to_eigen(expected_mat)};

        REQUIRE(ki::almost_eq_with_print_(state, expected_state));
    }
}

TEST_CASE("MultiQubitKrausChannel amplitude damping")
{
    const auto parameter = 0.4; // GENERATE(0.2, 0.4, 0.6, 0.75, 1.0);

    // NOTE: parameters don't have meaningful names
    const auto eta = std::sqrt(1.0 - parameter);
    const auto lam = std::sqrt(parameter);

    const auto size = Eigen::Index {4};

    // create the four Kraus matrices for the 2-qubit amplitude damping Kraus channel
    auto kraus_matrix00 = [eta]() {
        auto output = Eigen::MatrixXcd::Zero(size, size).eval();
        output(0, 0) = 1.0;
        output(1, 1) = eta;
        output(2, 2) = eta;
        output(3, 3) = eta * eta;
        
        return output;
    }();

    auto kraus_matrix01 = [eta, lam]() {
        auto output = Eigen::MatrixXcd::Zero(size, size).eval();
        output(0, 1) = lam;
        output(1, 3) = eta * lam;
        
        return output;
    }();

    auto kraus_matrix10 = [eta, lam]() {
        auto output = Eigen::MatrixXcd::Zero(size, size).eval();
        output(0, 2) = lam;
        output(2, 3) = eta * lam;
        
        return output;
    }();

    auto kraus_matrix11 = [lam]() {
        auto output = Eigen::MatrixXcd::Zero(size, size).eval();
        output(0, 3) = lam * lam;
        
        return output;
    }();

    const auto channel = ket::MultiQubitKrausChannel {{
        std::move(kraus_matrix00), std::move(kraus_matrix01),
        std::move(kraus_matrix10), std::move(kraus_matrix11)
    }};

    // state should be simple but not completely arbitrary, so we don't use a random state
    auto state = [&]() {
        auto circuit = ket::QuantumCircuit {2};
        circuit.add_h_gate({0, 1});
        circuit.add_x_gate(0);
        circuit.add_y_gate(1);
        circuit.add_s_gate({0, 1});
        circuit.add_ry_gate(0, 0.15 * M_PI);
        circuit.add_rx_gate(1, 0.25 * M_PI);

        auto state_ = ket::DensityMatrix {"00"};
        ket::simulate(circuit, state_);

        return state_;
    }();

    auto buffer = Eigen::MatrixXcd(4, 4);

    const auto expected = result_amplitude_damping_2qubit(state, parameter);
    ket::simulate_multi_qubit_kraus_channel(state, channel, buffer);

    REQUIRE(ki::almost_eq_with_print_(state, expected));
}
