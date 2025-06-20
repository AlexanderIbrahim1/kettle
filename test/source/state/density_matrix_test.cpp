#include <iostream>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <Eigen/Dense>

#include "kettle/circuit/circuit.hpp"
#include "kettle/simulation/simulate.hpp"
#include "kettle/state/statevector.hpp"
#include "kettle/state/density_matrix.hpp"


static auto almost_eq_with_print(const Eigen::MatrixXcd& left, const Eigen::MatrixXcd& right) {
    if (left.isApprox(right)) {
        return true;
    } else {
        std::cout << "LEFT: \n";
        std::cout << left;
        std::cout << "\nRIGHT: \n";
        std::cout << right;
        return false;
    }
}


TEST_CASE("statevector_to_density_matrix()")
{
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

        REQUIRE(almost_eq_with_print(density_matrix.matrix(), expected.matrix()));
    }
}
