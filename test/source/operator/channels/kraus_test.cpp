#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#define REQUIRE_MSG(cond, msg) do { INFO(msg); REQUIRE(cond); } while((void)0, 0)

#include "kettle/circuit/circuit.hpp"
#include "kettle/common/matrix2x2.hpp"
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

}  // namespace


TEST_CASE("Kraus channel depolarizing noise")
{
    const auto parameter = 0.0; // GENERATE(0.2, 0.4, 0.6, 0.75, 1.0);

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
