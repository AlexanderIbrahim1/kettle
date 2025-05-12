#include <cmath>
#include <functional>
#include <vector>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "kettle/circuit/circuit.hpp"
#include "kettle/parameter/parameter.hpp"
#include "kettle/simulation/simulate.hpp"
#include "kettle/state/state.hpp"


static auto get_zero_state_result(double angle) -> ket::QuantumState {
    const auto real0 = std::cos(angle / 2.0);
    const auto imag0 = 0.0;
    const auto real1 = 0.0;
    const auto imag1 = - std::sin(angle / 2.0);

    return ket::QuantumState {{ {real0, imag0}, {real1, imag1}}};
}


static auto get_plus_state_result(double angle) -> ket::QuantumState {
    const auto real0 = std::cos(angle / 2.0) / M_SQRT2;
    const auto imag0 = - std::sin(angle / 2.0) / M_SQRT2;
    const auto real1 = real0;
    const auto imag1 = imag0;

    return ket::QuantumState {{ {real0, imag0}, {real1, imag1}}};
}


TEST_CASE("simulate single RX gate with different angles")
{
    struct TestCase
    {
        ket::QuantumState input_statevector;
        std::function<ket::QuantumState(double)> expected;
    };

    const auto testcase = GENERATE(
        TestCase {ket::QuantumState {{{1.0, 0.0}, {0.0, 0.0}}}, &get_zero_state_result},
        TestCase {ket::QuantumState {{{M_SQRT1_2, 0.0}, {M_SQRT1_2, 0.0}}}, &get_plus_state_result}
    );

    const auto input_angle = 2.0 * M_PI * GENERATE(0.0, 0.1, 0.25, 0.4, 0.5, 0.8, 1.0);
    const auto expected_output_state = testcase.expected(input_angle);

    auto circuit = ket::QuantumCircuit {1};
    const auto id = circuit.add_rx_gate(0, input_angle, ket::param::parameterized {});

    auto statevector = testcase.input_statevector;
    ket::simulate(circuit, statevector);

    REQUIRE(ket::almost_eq(statevector, expected_output_state));
    REQUIRE_THAT(circuit.parameter_values_map().at(id), Catch::Matchers::WithinRel(input_angle));
}


TEST_CASE("manually set parameter of RX gate")
{
    const auto first_angle = 1.2345 * M_PI;
    const auto second_angle = 0.5432 * M_PI;

    auto circuit = ket::QuantumCircuit {1};
    const auto id = circuit.add_rx_gate(0, first_angle, ket::param::parameterized {});

    // make sure that the parameterized circuit works with the first angle
    auto first_statevector = ket::QuantumState {"0"};
    ket::simulate(circuit, first_statevector);
    const auto expected_first_statevector = get_zero_state_result(first_angle);

    REQUIRE(ket::almost_eq(first_statevector, expected_first_statevector));

    // make sure that the parameterized circuit works with the second angle, after changing to it
    circuit.set_parameter_value(id, second_angle);

    auto second_statevector = ket::QuantumState {"0"};
    ket::simulate(circuit, second_statevector);
    const auto expected_second_statevector = get_zero_state_result(second_angle);

    REQUIRE(ket::almost_eq(second_statevector, expected_second_statevector));
}


TEST_CASE("throw if no parameter id found")
{
    const auto param = ket::param::Parameter {"theta"};

    auto circuit = ket::QuantumCircuit {1};
    
    SECTION("no parameters are in")
    {
        REQUIRE_THROWS_AS(circuit.set_parameter_value(param.id(), 1.2345), std::out_of_range);
    }

    SECTION("another parameter is in")
    {
        circuit.add_rx_gate(0, 0.54321, ket::param::parameterized {});
        REQUIRE_THROWS_AS(circuit.set_parameter_value(param.id(), 1.2345), std::out_of_range);
    }
}


TEST_CASE("simulate with two identical parameters")
{
    const auto angle = 1.2345 * M_PI;

    // create a circuit with two RX gates with a fixed angle, and propagate a statevector through it
    auto circuit0 = ket::QuantumCircuit {1};
    circuit0.add_rx_gate(0, angle);
    circuit0.add_rx_gate(0, angle);

    auto statevector0 = ket::QuantumState {"0"};
    ket::simulate(circuit0, statevector0);

    // create a circuit with two RX gates with a parameterized angle, and propagate a statevector through it
    auto circuit1 = ket::QuantumCircuit {1};
    const auto id = circuit1.add_rx_gate(0, angle, ket::param::parameterized {});
    circuit1.add_rx_gate(0, id);

    auto statevector1 = ket::QuantumState {"0"};
    ket::simulate(circuit1, statevector1);

    REQUIRE(ket::almost_eq(statevector0, statevector1));
    REQUIRE(circuit1.parameter_values_map().size() == 1);
    REQUIRE(circuit1.parameter_data_map().at(id).count == 2);
}
