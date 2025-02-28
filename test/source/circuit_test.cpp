#include <cstdint>
#include <vector>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "mini-qiskit/circuit.hpp"
#include "mini-qiskit/primitive_gate.hpp"

static constexpr auto ABS_TOL = double {1.0e-6};

TEST_CASE("add multiple X gates")
{
    auto circuit = mqis::QuantumCircuit {3};

    const auto number_of_gates = [](const mqis::QuantumCircuit& circ)
    { return std::distance(circ.begin(), circ.end()); };

    SECTION("add 1")
    {
        const auto indices = std::vector<std::size_t> {1};
        circuit.add_x_gate(indices);

        REQUIRE(number_of_gates(circuit) == 1);

        REQUIRE(impl_mqis::unpack_x_gate(circuit[0]) == 1);
        REQUIRE(circuit[0].gate == mqis::Gate::X);
    }

    SECTION("add 0, 2")
    {
        const auto indices = std::vector<std::size_t> {0, 2};
        circuit.add_x_gate(indices);

        REQUIRE(number_of_gates(circuit) == 2);

        REQUIRE(impl_mqis::unpack_x_gate(circuit[0]) == 0);
        REQUIRE(circuit[0].gate == mqis::Gate::X);

        REQUIRE(impl_mqis::unpack_x_gate(circuit[1]) == 2);
        REQUIRE(circuit[1].gate == mqis::Gate::X);
    }

    SECTION("add 0, 1, 2")
    {
        const auto indices = std::vector<std::size_t> {0, 1, 2};
        circuit.add_x_gate(indices);

        REQUIRE(number_of_gates(circuit) == 3);

        REQUIRE(impl_mqis::unpack_x_gate(circuit[0]) == 0);
        REQUIRE(circuit[0].gate == mqis::Gate::X);

        REQUIRE(impl_mqis::unpack_x_gate(circuit[1]) == 1);
        REQUIRE(circuit[1].gate == mqis::Gate::X);

        REQUIRE(impl_mqis::unpack_x_gate(circuit[2]) == 2);
        REQUIRE(circuit[2].gate == mqis::Gate::X);
    }

    SECTION("add 0, 1, 2 via initializer list")
    {
        circuit.add_x_gate({0, 1, 2});

        REQUIRE(number_of_gates(circuit) == 3);

        REQUIRE(impl_mqis::unpack_x_gate(circuit[0]) == 0);
        REQUIRE(circuit[0].gate == mqis::Gate::X);

        REQUIRE(impl_mqis::unpack_x_gate(circuit[1]) == 1);
        REQUIRE(circuit[1].gate == mqis::Gate::X);

        REQUIRE(impl_mqis::unpack_x_gate(circuit[2]) == 2);
        REQUIRE(circuit[2].gate == mqis::Gate::X);
    }
}

TEST_CASE("add multiple RX gates")
{
    auto circuit = mqis::QuantumCircuit {3};

    const auto number_of_gates = [](const mqis::QuantumCircuit& circ)
    { return std::distance(circ.begin(), circ.end()); };

    SECTION("add 0, 1, 2 via initializer list")
    {
        circuit.add_rx_gate({
            {0.25, 0},
            {0.5,  1},
            {0.75, 2}
        });

        REQUIRE(number_of_gates(circuit) == 3);

        const auto rx_gate0 = impl_mqis::unpack_rx_gate(circuit[0]);
        REQUIRE_THAT(std::get<0>(rx_gate0), Catch::Matchers::WithinAbs(0.25, ABS_TOL));
        REQUIRE(std::get<1>(rx_gate0) == 0);
        REQUIRE(circuit[0].gate == mqis::Gate::RX);

        const auto rx_gate1 = impl_mqis::unpack_rx_gate(circuit[1]);
        REQUIRE_THAT(std::get<0>(rx_gate1), Catch::Matchers::WithinAbs(0.5, ABS_TOL));
        REQUIRE(std::get<1>(rx_gate1) == 1);
        REQUIRE(circuit[1].gate == mqis::Gate::RX);

        const auto rx_gate2 = impl_mqis::unpack_rx_gate(circuit[2]);
        REQUIRE_THAT(std::get<0>(rx_gate2), Catch::Matchers::WithinAbs(0.75, ABS_TOL));
        REQUIRE(std::get<1>(rx_gate2) == 2);
        REQUIRE(circuit[2].gate == mqis::Gate::RX);
    }
}

TEST_CASE("add multiple CX gates")
{
    auto circuit = mqis::QuantumCircuit {3};

    const auto number_of_gates = [](const mqis::QuantumCircuit& circ)
    { return std::distance(circ.begin(), circ.end()); };

    SECTION("add 0, 1, 2 via initializer list")
    {
        circuit.add_cx_gate({
            {0, 1},
            {1, 2},
            {2, 0}
        });

        REQUIRE(number_of_gates(circuit) == 3);

        const auto cx_gate0 = impl_mqis::unpack_cx_gate(circuit[0]);
        REQUIRE(std::get<0>(cx_gate0) == 0);
        REQUIRE(std::get<1>(cx_gate0) == 1);
        REQUIRE(circuit[0].gate == mqis::Gate::CX);

        const auto cx_gate1 = impl_mqis::unpack_cx_gate(circuit[1]);
        REQUIRE(std::get<0>(cx_gate1) == 1);
        REQUIRE(std::get<1>(cx_gate1) == 2);
        REQUIRE(circuit[1].gate == mqis::Gate::CX);

        const auto cx_gate2 = impl_mqis::unpack_cx_gate(circuit[2]);
        REQUIRE(std::get<0>(cx_gate2) == 2);
        REQUIRE(std::get<1>(cx_gate2) == 0);
        REQUIRE(circuit[2].gate == mqis::Gate::CX);
    }
}

TEST_CASE("add multiple CRX gates")
{
    auto circuit = mqis::QuantumCircuit {3};

    const auto number_of_gates = [](const mqis::QuantumCircuit& circ)
    { return std::distance(circ.begin(), circ.end()); };

    SECTION("add 0, 1, 2 via initializer list")
    {
        circuit.add_crx_gate({
            {0.25, 0, 1},
            {0.5,  1, 2},
            {0.75, 2, 0}
        });

        REQUIRE(number_of_gates(circuit) == 3);

        const auto crx_gate0 = impl_mqis::unpack_crx_gate(circuit[0]);
        REQUIRE(std::get<0>(crx_gate0) == 0);
        REQUIRE(std::get<1>(crx_gate0) == 1);
        REQUIRE_THAT(std::get<2>(crx_gate0), Catch::Matchers::WithinAbs(0.25, ABS_TOL));
        REQUIRE(circuit[0].gate == mqis::Gate::CRX);

        const auto crx_gate1 = impl_mqis::unpack_crx_gate(circuit[1]);
        REQUIRE(std::get<0>(crx_gate1) == 1);
        REQUIRE(std::get<1>(crx_gate1) == 2);
        REQUIRE_THAT(std::get<2>(crx_gate1), Catch::Matchers::WithinAbs(0.5, ABS_TOL));
        REQUIRE(circuit[1].gate == mqis::Gate::CRX);

        const auto crx_gate2 = impl_mqis::unpack_crx_gate(circuit[2]);
        REQUIRE(std::get<0>(crx_gate2) == 2);
        REQUIRE(std::get<1>(crx_gate2) == 0);
        REQUIRE_THAT(std::get<2>(crx_gate2), Catch::Matchers::WithinAbs(0.75, ABS_TOL));
        REQUIRE(circuit[2].gate == mqis::Gate::CRX);
    }
}
