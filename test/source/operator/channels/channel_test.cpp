#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <Eigen/Dense>
#include <cstddef>

#include "kettle/circuit/circuit.hpp"
#include "kettle/common/matrix2x2.hpp"
#include "kettle/gates/common_u_gates.hpp"
#include "kettle/operator/channels/mixed_circuit_channel.hpp"
#include "kettle/operator/channels/multi_qubit_kraus_channel.hpp"
#include "kettle/operator/channels/one_qubit_kraus_channel.hpp"
#include "kettle/operator/noise/standard_errors.hpp"
#include "kettle/simulation/simulate_density_matrix.hpp"
#include "kettle/state/density_matrix.hpp"

#include "kettle_internal/common/state_test_utils.hpp"
#include "kettle_internal/operator/channels/channel_helper.hpp"
#include "kettle_internal/simulation/density_matrix_through_channel.hpp"

#include "channel_test_utils.hpp"
#include "channel_test_results.hpp"

namespace ki = ket::internal;

namespace
{

/*
    Apply the depolarizing noise to a 1-qubit system manually, in the Kraus manner.
*/
auto depolarizing_noise_manual_1qubit(
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

/*
    The MixedCircuitChannel version for depolarizing noise.

    This is only used for unit testing purposes. By default, the implementation for applying
    depolarizing noise uses a Kraus channel.
*/
auto depolarizing_noise_mixed_unitary_1qubit(double parameter) -> ket::MixedCircuitChannel
{
    if (parameter < 0.0 || parameter > 1.0) {
        throw std::runtime_error {"ERROR: the depolarizing noise parameter must be in [0.0, 1.0].\n"};
    }

    const auto coeff0 = 1.0 - parameter;
    const auto coeff123 = parameter / 3.0;

    auto circuit0 = ket::QuantumCircuit {1};

    auto circuit1 = ket::QuantumCircuit {1};
    circuit1.add_x_gate(0);

    auto circuit2 = ket::QuantumCircuit {1};
    circuit2.add_y_gate(0);

    auto circuit3 = ket::QuantumCircuit {1};
    circuit3.add_z_gate(0);

    return ket::MixedCircuitChannel {
        {.coefficient=coeff0,   .unitary=std::move(circuit0)},
        {.coefficient=coeff123, .unitary=std::move(circuit1)},
        {.coefficient=coeff123, .unitary=std::move(circuit2)},
        {.coefficient=coeff123, .unitary=std::move(circuit3)},
    };
}

auto depolarizing_noise_kraus_1qubit(double parameter, std::size_t target_index) -> ket::OneQubitKrausChannel
{
    if (parameter < 0.0 || parameter > 1.0) {
        throw std::runtime_error {"ERROR: the depolarizing noise parameter must be in [0.0, 1.0].\n"};
    }

    const auto coeff0 = std::sqrt(1.0 - parameter);
    const auto coeff123 = std::sqrt(parameter / 3.0);
    const auto mat0 = coeff0 * ket::i_gate();
    const auto mat1 = coeff123 * ket::x_gate();
    const auto mat2 = coeff123 * ket::y_gate();
    const auto mat3 = coeff123 * ket::z_gate();

    return ket::OneQubitKrausChannel {{mat0, mat1, mat2, mat3}, target_index};
}

}  // namespace


TEST_CASE("Kraus channel depolarizing noise")
{
    const auto parameter = 0.4; // GENERATE(0.2, 0.4, 0.6, 0.75, 1.0);

    // state should be simple but not completely arbitrary, so we don't use a random state
    auto state = ctestutils::basic_state0();

    const auto matrix = ctestutils::eigen_to_mat2x2(state.matrix());
    const auto expected = ctestutils::result_depolarizing_noise_1qubit(matrix, parameter);

    SECTION("manual application of Matrix2X2 instances above")
    {
        const auto depol_channel = depolarizing_noise_kraus_1qubit(parameter, 0);
        const auto actual = depolarizing_noise_manual_1qubit(matrix, depol_channel);

        REQUIRE(ket::almost_eq(actual, expected));
    }

    SECTION("using channels")
    {
        // naming doesn't matter; buffers play different roles within the function
        auto buffer0 = Eigen::MatrixXcd(2, 2);
        auto buffer1 = Eigen::MatrixXcd(2, 2);
        auto buffer2 = Eigen::MatrixXcd(2, 2);

        // the only pair of indices for a 2-qubit system is (0, 1)
        const auto n_single_gate_pairs = Eigen::Index {1};
        const auto single_pair = ki::FlatIndexPair<Eigen::Index> {.i_lower=0, .i_upper=n_single_gate_pairs};

        SECTION("using `simulate_one_qubit_kraus_channel()`")
        {
            const auto depol_channel = depolarizing_noise_kraus_1qubit(parameter, 0);
            ket::simulate_one_qubit_kraus_channel(state, depol_channel, single_pair, buffer0, buffer1, buffer2);

            const auto expected_state = ket::DensityMatrix {ctestutils::mat2x2_to_eigen(expected)};
            REQUIRE(ki::almost_eq_with_print_(state, expected_state));
        }

        SECTION("using `simulate_pauli_channel()`")
        {
            const auto depol_channel = ket::symmetric_depolarizing_error_channel(parameter, 1, {0});
            ket::simulate_pauli_channel(state, depol_channel, single_pair, buffer0, buffer1, buffer2);

            const auto expected_state = ket::DensityMatrix {ctestutils::mat2x2_to_eigen(expected)};
            REQUIRE(ki::almost_eq_with_print_(state, expected_state));
        }

        SECTION("using `simulate_mixed_circuit_channel()`")
        {
            const auto n_double_gate_pairs = Eigen::Index {0};
            const auto double_pair = ki::FlatIndexPair<Eigen::Index> {.i_lower=0, .i_upper=n_double_gate_pairs};

            const auto depol_channel = depolarizing_noise_mixed_unitary_1qubit(parameter);
            ket::simulate_mixed_circuit_channel(state, depol_channel, single_pair, double_pair, buffer0, buffer1, buffer2);

            const auto expected_state = ket::DensityMatrix {ctestutils::mat2x2_to_eigen(expected)};
            REQUIRE(ki::almost_eq_with_print_(state, expected_state));
        }
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

    const auto expected = ctestutils::result_amplitude_damping_2qubit(state, parameter);
    ket::simulate_multi_qubit_kraus_channel(state, channel, buffer);

    REQUIRE(ki::almost_eq_with_print_(state, expected));
}

TEST_CASE("CartesianTicker")
{
    auto ticker = ket::internal::CartesianTicker {3, 3};

    const auto expected = std::vector<std::vector<std::size_t>> {
        {0, 0, 0}, {0, 0, 1}, {0, 0, 2},
        {0, 1, 0}, {0, 1, 1}, {0, 1, 2},
        {0, 2, 0}, {0, 2, 1}, {0, 2, 2},
        {1, 0, 0}, {1, 0, 1}, {1, 0, 2},
        {1, 1, 0}, {1, 1, 1}, {1, 1, 2},
        {1, 2, 0}, {1, 2, 1}, {1, 2, 2},
        {2, 0, 0}, {2, 0, 1}, {2, 0, 2},
        {2, 1, 0}, {2, 1, 1}, {2, 1, 2},
        {2, 2, 0}, {2, 2, 1}, {2, 2, 2},
    };

    REQUIRE(ticker.size() == expected.size());

    for (const auto& elem : expected) {
        REQUIRE(ticker.ticker() == elem);
        ticker.increment();
    }
}

/*
    NOTE: this is more of an integration test, and it's a "negative" test, so maybe I'll get rid
    of it at some point in the future.
    
    In one case:
      - apply the 1-qubit depolarizing noise channel to each of the two 1-qubit state separately
      - take the tensor product to get a 2-qubit state (POST-depolarizing noise)
    
    In another case:
      - take the tensor product to get a 2-qubit state (PRE-depolarizing noise)
      - apply the 2-qubit depolarizing noise channel
    
    The output will NOT be the same in both cases:
      - depolarizing noise is global!
*/
TEST_CASE("depolarizing noise : 2 qubits")
{
    const auto parameter = 0.4;

    // state should be simple but not completely arbitrary, so we don't use a random state
    auto state0 = ctestutils::basic_state0();
    auto state1 = ctestutils::basic_state1();

    const auto tensor_prod_then_depol = [state0, state1, parameter]() mutable {
        // naming doesn't matter; buffers play different roles within the function
        auto buffer0 = Eigen::MatrixXcd(4, 4);
        auto buffer1 = Eigen::MatrixXcd(4, 4);
        auto buffer2 = Eigen::MatrixXcd(4, 4);

        // the only pair of indices for a 2-qubit system is (0, 1)
        const auto n_single_gate_pairs = Eigen::Index {2};
        const auto single_pair = ki::FlatIndexPair<Eigen::Index> {.i_lower=0, .i_upper=n_single_gate_pairs};

        const auto depol_channel = ket::symmetric_depolarizing_error_channel(parameter, 2, {0, 1});

        auto state = ket::tensor_product(state0, state1);

        ket::simulate_pauli_channel(state, depol_channel, single_pair, buffer0, buffer1, buffer2);

        return state;
    }();

    const auto depol_then_tensor_prod = [state0, state1, parameter]() mutable {
        // naming doesn't matter; buffers play different roles within the function
        auto buffer0 = Eigen::MatrixXcd(2, 2);
        auto buffer1 = Eigen::MatrixXcd(2, 2);
        auto buffer2 = Eigen::MatrixXcd(2, 2);

        // the only pair of indices for a 2-qubit system is (0, 1)
        const auto n_single_gate_pairs = Eigen::Index {1};
        const auto single_pair = ki::FlatIndexPair<Eigen::Index> {.i_lower=0, .i_upper=n_single_gate_pairs};

        const auto depol_channel = ket::symmetric_depolarizing_error_channel(parameter, 1, {0});

        ket::simulate_pauli_channel(state0, depol_channel, single_pair, buffer0, buffer1, buffer2);
        ket::simulate_pauli_channel(state1, depol_channel, single_pair, buffer0, buffer1, buffer2);

        return ket::tensor_product(state0, state1);
    }();

    REQUIRE(!ki::almost_eq_with_print_(tensor_prod_then_depol, depol_then_tensor_prod));
}

TEST_CASE("depolarizing channel coefficients")
{
    constexpr auto abs_tol = 1.0e-6;
    const auto& within_abs = Catch::Matchers::WithinAbs;

    const auto parameter = GENERATE(0.2, 0.4, 0.6, 0.75, 1.0);

    SECTION("channel acting on 1 qubit")
    {
        const auto depol_channel = ket::symmetric_depolarizing_error_channel(parameter, 1, {0});

        REQUIRE(depol_channel.size() == 4);

        REQUIRE_THAT(depol_channel.at(0).coefficient, within_abs(1.0 - parameter, abs_tol));

        for (std::size_t i {1}; i < depol_channel.size(); ++i) {
            REQUIRE_THAT(depol_channel.at(i).coefficient, within_abs(parameter / 3.0, abs_tol));
        }
    }

    SECTION("channel acting on 2 qubit")
    {
        const auto depol_channel = ket::symmetric_depolarizing_error_channel(parameter, 2, {0, 1});
        
        REQUIRE(depol_channel.size() == 16);

        REQUIRE_THAT(depol_channel.at(0).coefficient, within_abs(1.0 - parameter, abs_tol));

        for (std::size_t i {1}; i < depol_channel.size(); ++i) {
            REQUIRE_THAT(depol_channel.at(i).coefficient, within_abs(parameter / 15.0, abs_tol));
        }
    }
}


TEST_CASE("one_qubit_phase_amplitude_damping_error_channel()")
{
    const auto parameters = ket::PhaseAmplitudeDampingParameters {
        .amplitude=0.3,
        .phase=0.4,
        .excited_population=0.2,
    };

    auto state = ctestutils::basic_state0();
    const auto matrix = ctestutils::eigen_to_mat2x2(state.matrix());
    const auto expected = ctestutils::result_phase_amplitude_damping_1qubit(matrix, parameters);
    const auto expected_state = ket::DensityMatrix {ctestutils::mat2x2_to_eigen(expected)};

    const auto channel = ket::one_qubit_phase_amplitude_damping_error_channel(parameters, 0);

    auto buffer0 = Eigen::MatrixXcd(2, 2);
    auto buffer1 = Eigen::MatrixXcd(2, 2);
    auto buffer2 = Eigen::MatrixXcd(2, 2);

    // the only pair of indices for a 2-qubit system is (0, 1)
    const auto n_single_gate_pairs = Eigen::Index {1};
    const auto single_pair = ki::FlatIndexPair<Eigen::Index> {.i_lower=0, .i_upper=n_single_gate_pairs};
    ket::simulate_one_qubit_kraus_channel(state, channel, single_pair, buffer0, buffer1, buffer2);

    REQUIRE(ki::almost_eq_with_print_(expected_state, state));
}