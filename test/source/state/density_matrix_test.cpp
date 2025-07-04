#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#define REQUIRE_MSG(cond, msg) do { INFO(msg); REQUIRE(cond); } while((void)0, 0)

#include <Eigen/Dense>

#include "kettle/circuit/circuit.hpp"
#include "kettle/simulation/simulate_density_matrix.hpp"
#include "kettle/simulation/simulate.hpp"
#include "kettle/state/statevector.hpp"
#include "kettle/state/density_matrix.hpp"
#include "kettle_internal/common/state_test_utils.hpp"


namespace ki = ket::internal;


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

        REQUIRE(ki::almost_eq_with_print_(density_matrix, expected));
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

        REQUIRE(ki::almost_eq_with_print_(density_matrix, expected));
    }
}

/*
    Test if the tensor product function for `DensityMatrix` works by:
      - taking the tensor product of `Statevector`s first, then turn them to `DensityMatrix`s
      - turning `Statevector`s to `DensityMatrix`s first, then taking their tensor product
*/
TEST_CASE("density matrix tensor product")
{
    using CircFunc = std::function<void(ket::QuantumCircuit&)>;

    struct TestCase
    {
        std::string message;
        CircFunc func0;
        CircFunc func1;
    };

    SECTION("1 qubit systems")
    {
        const auto testcase = GENERATE(
            TestCase {
                "H-gate on (state 0, qubit 0)",
                [](ket::QuantumCircuit& circ) { circ.add_h_gate(0); },
                []([[maybe_unused]] ket::QuantumCircuit& circ) { }
            },
            TestCase {
                "H-gate on (state 1, qubit 0)",
                []([[maybe_unused]] ket::QuantumCircuit& circ) { },
                [](ket::QuantumCircuit& circ) { circ.add_h_gate(0); }
            },
            TestCase {
                "H-gate on (state 0, qubit 0), (state 1, qubit 0)",
                [](ket::QuantumCircuit& circ) { circ.add_h_gate(0); },
                [](ket::QuantumCircuit& circ) { circ.add_h_gate(0); }
            },
            TestCase {
                "X-gate on (state 0, qubit 0), Z-gate on (state 1, qubit 0)",
                [](ket::QuantumCircuit& circ) { circ.add_x_gate(0); },
                [](ket::QuantumCircuit& circ) { circ.add_z_gate(0); }
            },
            TestCase {
                "H-gate, Y-gate on (state 0, qubit 0), Z-gate on (state 1, qubit 0)",
                [](ket::QuantumCircuit& circ) { circ.add_h_gate(0); circ.add_y_gate(0); },
                [](ket::QuantumCircuit& circ) { circ.add_z_gate(0); }
            }
        );

        auto circuit0 = ket::QuantumCircuit {1};
        testcase.func0(circuit0);

        auto circuit1 = ket::QuantumCircuit {1};
        testcase.func1(circuit1);

        auto statevector0 = ket::Statevector {"0"};
        auto statevector1 = ket::Statevector {"0"};
        auto density_matrix0 = ket::DensityMatrix {"0"};
        auto density_matrix1 = ket::DensityMatrix {"0"};

        // tensor product on statevectors, *then* create density matrix
        ket::simulate(circuit0, statevector0);
        ket::simulate(circuit1, statevector1);
        const auto statevector_tp = ket::tensor_product(statevector0, statevector1);
        const auto result_tp_then_dm = ket::statevector_to_density_matrix(statevector_tp);

        // creates density matrices, *then* take tensor product
        ket::simulate(circuit0, density_matrix0);
        ket::simulate(circuit1, density_matrix1);
        const auto result_dm_then_tp = ket::tensor_product(density_matrix0, density_matrix1);

        REQUIRE_MSG(ki::almost_eq_with_print_(result_tp_then_dm, result_dm_then_tp), testcase.message);
    }

    SECTION("2 qubit systems")
    {
        const auto testcase = GENERATE(
            TestCase {
                "H-gate on (state 0, qubit 0, qubit 1)",
                [](ket::QuantumCircuit& circ) { circ.add_h_gate({0, 1}); },
                []([[maybe_unused]] ket::QuantumCircuit& circ) { }
            },
            TestCase {
                "H-gate on (state 1, qubit 0, qubit 1)",
                []([[maybe_unused]] ket::QuantumCircuit& circ) { },
                [](ket::QuantumCircuit& circ) { circ.add_h_gate({0, 1}); }
            },
            TestCase {
                "H-gate on (state 0, qubit 0, qubit 1), (state 1, qubit 0)",
                [](ket::QuantumCircuit& circ) { circ.add_h_gate({0, 1}); },
                [](ket::QuantumCircuit& circ) { circ.add_h_gate(0); }
            },
            TestCase {
                "X-gate on (state 0, qubit 0), H-gate on (state 0, qubit 1), Z-gate on (state 1, qubit 0, qubit 1)",
                [](ket::QuantumCircuit& circ) { circ.add_x_gate(0); circ.add_h_gate(1); },
                [](ket::QuantumCircuit& circ) { circ.add_z_gate(0); }
            },
            TestCase {
                "H-gate, Y-gate on (state 0, qubit 0), Y-gate on (state 1, qubit 0) Z-gate on (state 1, qubit 1)",
                [](ket::QuantumCircuit& circ) { circ.add_h_gate(0); circ.add_y_gate(0); },
                [](ket::QuantumCircuit& circ) { circ.add_y_gate(0); circ.add_z_gate(1); }
            }
        );

        auto circuit0 = ket::QuantumCircuit {2};
        testcase.func0(circuit0);

        auto circuit1 = ket::QuantumCircuit {2};
        testcase.func1(circuit1);

        auto statevector0 = ket::Statevector {"00"};
        auto statevector1 = ket::Statevector {"00"};
        auto density_matrix0 = ket::DensityMatrix {"00"};
        auto density_matrix1 = ket::DensityMatrix {"00"};

        // tensor product on statevectors, *then* create density matrix
        ket::simulate(circuit0, statevector0);
        ket::simulate(circuit1, statevector1);
        const auto statevector_tp = ket::tensor_product(statevector0, statevector1);
        const auto result_tp_then_dm = ket::statevector_to_density_matrix(statevector_tp);

        // creates density matrices, *then* take tensor product
        ket::simulate(circuit0, density_matrix0);
        ket::simulate(circuit1, density_matrix1);
        const auto result_dm_then_tp = ket::tensor_product(density_matrix0, density_matrix1);

        REQUIRE_MSG(ki::almost_eq_with_print_(result_tp_then_dm, result_dm_then_tp), testcase.message);
    }
}

TEST_CASE("partial trace [take tensor product, then partial trace, and check for match]")
{
    SECTION("tensor product of two 1-qubit systems")
    {
        auto dens_mat0 = ket::DensityMatrix {"0"};
        auto dens_mat1 = ket::DensityMatrix {"0"};

        auto circuit = ket::QuantumCircuit {1};
        circuit.add_h_gate(0);
        ket::simulate(circuit, dens_mat1);

        const auto tensor_product = ket::tensor_product(dens_mat0, dens_mat1);

        SECTION("partial trace over qubit 0")
        {
            const auto traced1 = ket::partial_trace(tensor_product, {0});
            REQUIRE(ki::almost_eq_with_print_(traced1, dens_mat1));
        }

        SECTION("partial trace over qubit 1")
        {
            const auto traced0 = ket::partial_trace(tensor_product, {1});
            REQUIRE(ki::almost_eq_with_print_(traced0, dens_mat0));
        }
    }
}
