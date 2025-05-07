#include <string>
#include <optional>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "kettle/circuit/circuit.hpp"
#include "kettle/circuit_operations/transpile_to_primitive.hpp"
#include "kettle/simulation/simulate.hpp"

#include "kettle_internal/gates/primitive_gate/gate_id.hpp"

using G = ket::Gate;
using Matrices = std::vector<ket::Matrix2X2>;
namespace gid = ket::internal::gate_id;

/*
    Create a unitary 2x2 matrix that is a product of primitive gates.
*/
static auto make_matrix(const std::vector<impl_ket::PrimitiveGateInfo>& pg_infos) -> ket::Matrix2X2
{
    auto output = ket::i_gate();

    for (const auto& pg_info : pg_infos) {
        switch (pg_info.gate)
        {
            case ket::Gate::H : {
                output = ket::h_gate() * output;
                break;
            }
            case ket::Gate::X : {
                output = ket::x_gate() * output;
                break;
            }
            case ket::Gate::Y : {
                output = ket::y_gate() * output;
                break;
            }
            case ket::Gate::Z : {
                output = ket::z_gate() * output;
                break;
            }
            case ket::Gate::SX : {
                output = ket::sx_gate() * output;
                break;
            }
            case ket::Gate::RX : {
                output = ket::rx_gate(pg_info.parameter.value()) * output;
                break;
            }
            case ket::Gate::RY : {
                output = ket::ry_gate(pg_info.parameter.value()) * output;
                break;
            }
            case ket::Gate::RZ : {
                output = ket::rz_gate(pg_info.parameter.value()) * output;
                break;
            }
            case ket::Gate::P : {
                output = ket::p_gate(pg_info.parameter.value()) * output;
                break;
            }
            default : {
                throw std::runtime_error {"Invalid PrimitiveGateInfo gate found.\n"};
            }
        }
    }

    return output;
}


TEST_CASE("transpile_to_primitive()")
{

    SECTION("1 qubit circuit, one gate")
    {
        const auto unitaries = GENERATE_COPY(
            Matrices {make_matrix({{G::H}})},
            Matrices {make_matrix({{G::H}, {G::X}})},
            Matrices {make_matrix({{G::H}, {G::X}, {G::RZ, 0.432}})},
            Matrices {make_matrix({{G::H}, {G::X}, {G::RZ, 0.432}, {G::P, 2.232}})},
            Matrices {make_matrix({{G::H}, {G::X}, {G::RX, 1.2345}, {G::RZ, -2.341}})}
        );

        auto state0 = GENERATE(
            ket::QuantumState {"0"},
            ket::QuantumState {"1"},
            ket::QuantumState {{{M_SQRT1_2, 0.0}, {M_SQRT1_2, 0.0}}},
            ket::QuantumState {{{0.0, M_SQRT1_2}, {0.0, M_SQRT1_2}}},
            ket::QuantumState {{{0.0, 1.0}, {0.0, 0.0}}}
        );

        // create the original circuit
        auto original = ket::QuantumCircuit {1};
        for (const auto& unitary : unitaries) {
            original.add_u_gate(unitary, 0);
        }

        // create the transpiled circuit
        const auto transpiled = ket::transpile_to_primitive(original);

        // create a state and propagate it through both circuits
        auto state1 = state0;

        ket::simulate(original, state0);
        ket::simulate(transpiled, state1);

        for (const auto& circuit_element : transpiled) {
            const auto& gate = circuit_element.get_gate();

            REQUIRE(gid::is_single_qubit_transform_gate(gate.gate));
            REQUIRE(gate.gate != G::U);
        }

        REQUIRE(ket::almost_eq(state0, state1));
    }
}

TEST_CASE("transpile_to_primitive() with control flow if_statement()")
{
    SECTION("2 qubit circuit, qubit 0 is measured, qubit 1 is dependent")
    {
        const auto unitaries = GENERATE_COPY(
            Matrices {make_matrix({{G::H}})},
            Matrices {make_matrix({{G::H}, {G::X}})},
            Matrices {make_matrix({{G::H}, {G::X}, {G::RZ, 0.432}})},
            Matrices {make_matrix({{G::H}, {G::X}, {G::RZ, 0.432}, {G::P, 2.232}})},
            Matrices {make_matrix({{G::H}, {G::X}, {G::RX, 1.2345}, {G::RZ, -2.341}})}
        );

        const auto init_bitstring = std::string {GENERATE("00", "10", "01", "11")};

        // create the original circuit
        auto original = ket::QuantumCircuit {2};

        // the state is in one of the computational basis sets, so the measurement will
        // end up with the same bit value for both the states
        original.add_m_gate(0, 0);

        auto subcircuit = ket::QuantumCircuit {2};
        for (const auto& unitary : unitaries) {
            subcircuit.add_u_gate(unitary, 1);
        }

        original.add_if_statement(0, std::move(subcircuit));

        // create the transpiled circuit
        const auto transpiled = ket::transpile_to_primitive(original);

        // create a state and propagate it through both circuits
        auto state0 = ket::QuantumState {init_bitstring};
        auto state1 = state0;

        ket::simulate(original, state0);
        ket::simulate(transpiled, state1);

        // the very first gate is a measurement gate
        const auto& circuit_element0 = transpiled[0];
        REQUIRE(circuit_element0.is_gate());
        REQUIRE(circuit_element0.get_gate().gate == G::M);

        // the second gate is a control flow
        const auto& circuit_element1 = transpiled[1];
        REQUIRE(circuit_element1.is_control_flow());
        REQUIRE(circuit_element1.get_control_flow().is_if_statement());

        // the remaining gates are the decomposed transformation gates, from the if statement's side
        const auto& transpiled_subcircuit = *circuit_element1.get_control_flow().get_if_statement().circuit();
        for (const auto& element : transpiled_subcircuit) {
            REQUIRE(element.is_gate());
            const auto& gate = element.get_gate();

            REQUIRE(gid::is_single_qubit_transform_gate(gate.gate));
            REQUIRE(gate.gate != G::U);
        }

        REQUIRE(ket::almost_eq(state0, state1));
    }
}

TEST_CASE("transpile_to_primitive() with control flow if_else_statement()")
{
    SECTION("2 qubit circuit, qubit 0 is measured, qubit 1 is dependent")
    {
        const auto if_unitaries = GENERATE_COPY(
            Matrices {make_matrix({{G::H}})},
            Matrices {make_matrix({{G::H}, {G::X}})},
            Matrices {make_matrix({{G::H}, {G::X}, {G::RZ, 0.432}})},
            Matrices {make_matrix({{G::H}, {G::X}, {G::RZ, 0.432}, {G::P, 2.232}})},
            Matrices {make_matrix({{G::H}, {G::X}, {G::RX, 1.2345}, {G::RZ, -2.341}})}
        );

        const auto else_unitaries = GENERATE_COPY(
            Matrices {make_matrix({{G::H}})},
            Matrices {make_matrix({{G::H}, {G::X}})},
            Matrices {make_matrix({{G::H}, {G::X}, {G::RZ, 0.432}})},
            Matrices {make_matrix({{G::H}, {G::X}, {G::RZ, 0.432}, {G::P, 2.232}})},
            Matrices {make_matrix({{G::H}, {G::X}, {G::RX, 1.2345}, {G::RZ, -2.341}})}
        );

        const auto init_bitstring = std::string {GENERATE("00", "10", "01", "11")};

        // create the original circuit
        auto original = ket::QuantumCircuit {2};

        // the state is in one of the computational basis sets, so the measurement will
        // end up with the same bit value for both the states
        original.add_m_gate(0, 0);

        auto if_subcircuit = ket::QuantumCircuit {2};
        for (const auto& unitary : if_unitaries) {
            if_subcircuit.add_u_gate(unitary, 1);
        }

        auto else_subcircuit = ket::QuantumCircuit {2};
        for (const auto& unitary : else_unitaries) {
            else_subcircuit.add_u_gate(unitary, 1);
        }

        original.add_if_else_statement(0, std::move(if_subcircuit), std::move(else_subcircuit));

        // create the transpiled circuit
        const auto transpiled = ket::transpile_to_primitive(original);

        // create a state and propagate it through both circuits
        auto state0 = ket::QuantumState {init_bitstring};
        auto state1 = state0;

        ket::simulate(original, state0);
        ket::simulate(transpiled, state1);

        // the very first gate is a measurement gate
        const auto& circuit_element0 = transpiled[0];
        REQUIRE(circuit_element0.is_gate());
        REQUIRE(circuit_element0.get_gate().gate == G::M);

        // the second gate is a control flow
        const auto& circuit_element1 = transpiled[1];
        REQUIRE(circuit_element1.is_control_flow());
        REQUIRE(circuit_element1.get_control_flow().is_if_else_statement());

        // the remaining gates are the decomposed transformation gates, from the if-else statement
        const auto& if_else_stmt = circuit_element1.get_control_flow().get_if_else_statement();

        const auto& transpiled_if_subcircuit = *if_else_stmt.if_circuit();
        for (const auto& element : transpiled_if_subcircuit) {
            REQUIRE(element.is_gate());
            const auto& gate = element.get_gate();

            REQUIRE(gid::is_single_qubit_transform_gate(gate.gate));
            REQUIRE(gate.gate != G::U);
        }

        const auto& transpiled_else_subcircuit = *if_else_stmt.else_circuit();
        for (const auto& element : transpiled_else_subcircuit) {
            REQUIRE(element.is_gate());
            const auto& gate = element.get_gate();

            REQUIRE(gid::is_single_qubit_transform_gate(gate.gate));
            REQUIRE(gate.gate != G::U);
        }

        REQUIRE(ket::almost_eq(state0, state1));
    }
}
