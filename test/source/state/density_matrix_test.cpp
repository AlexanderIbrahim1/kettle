#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <Eigen/Dense>

#include "kettle/circuit/circuit.hpp"
#include "kettle/simulation/simulate.hpp"
#include "kettle/state/statevector.hpp"
#include "kettle/state/density_matrix.hpp"
#include "kettle_internal/common/state_test_utils.hpp"


TEST_CASE("statevector_to_density_matrix()")
{
    SECTION("single computational basis state, 2 qubits")
    {
        struct TestCase
        {
            std::string basis_state;
            Eigen::Index idx;
        };

        const auto testcase = GENERATE(
            TestCase {"00", 0},
            TestCase {"10", 1},
            TestCase {"01", 2},
            TestCase {"11", 3}
        );

        auto statevector = ket::Statevector {testcase.basis_state};
        const auto density_matrix = ket::statevector_to_density_matrix(statevector);

        const auto expected = [&]() {
            auto matrix = Eigen::MatrixXcd::Zero(4, 4).eval();
            matrix(testcase.idx, testcase.idx) = 1.0;

            return ket::DensityMatrix {std::move(matrix)};
        }();

        REQUIRE(ket::internal::almost_eq_with_print_(density_matrix, expected));
    }

    SECTION("bell_state(00+)")
    {
        auto circuit = ket::QuantumCircuit {2};
        circuit.add_h_gate(0);
        circuit.add_cx_gate(0, 1);

        auto statevector = ket::Statevector {"00"};
        ket::simulate(circuit, statevector);

        const auto density_matrix = ket::statevector_to_density_matrix(statevector);

        const auto expected = []() {
            auto matrix = Eigen::MatrixXcd::Zero(4, 4).eval();
            matrix(0, 0) = 0.5;
            matrix(0, 3) = 0.5;
            matrix(3, 0) = 0.5;
            matrix(3, 3) = 0.5;

            return ket::DensityMatrix {std::move(matrix)};
        }();

        REQUIRE(ket::internal::almost_eq_with_print_(density_matrix, expected));
    }
}
