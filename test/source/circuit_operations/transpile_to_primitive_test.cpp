#include <string>
#include <optional>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "mini-qiskit/circuit/circuit.hpp"
#include "mini-qiskit/circuit_operations/transpile_to_primitive.hpp"
#include "mini-qiskit/simulation/simulate.hpp"


/*
    Create a unitary 2x2 matrix that is a product of primitive gates.
*/
constexpr auto make_matrix(const std::vector<impl_mqis::PrimitiveGateInfo>& pg_infos) -> mqis::Matrix2X2
{
    auto output = mqis::i_gate();

    for (const auto& pg_info : pg_infos) {
        switch (pg_info.gate)
        {
            case mqis::Gate::H : {
                output = mqis::h_gate() * output;
                break;
            }
            case mqis::Gate::X : {
                output = mqis::x_gate() * output;
                break;
            }
            case mqis::Gate::Y : {
                output = mqis::y_gate() * output;
                break;
            }
            case mqis::Gate::Z : {
                output = mqis::z_gate() * output;
                break;
            }
            case mqis::Gate::SX : {
                output = mqis::sx_gate() * output;
                break;
            }
            case mqis::Gate::RX : {
                output = mqis::rx_gate(pg_info.parameter.value()) * output;
                break;
            }
            case mqis::Gate::RY : {
                output = mqis::ry_gate(pg_info.parameter.value()) * output;
                break;
            }
            case mqis::Gate::RZ : {
                output = mqis::rz_gate(pg_info.parameter.value()) * output;
                break;
            }
            case mqis::Gate::P : {
                output = mqis::p_gate(pg_info.parameter.value()) * output;
                break;
            }
            default : {
                throw std::runtime_error {"Invalid PrimitiveGateInfo gate found.\n"};
            }
        }
    }

    return output;
}


constexpr auto n_gates(const mqis::QuantumCircuit& circuit) -> std::ptrdiff_t
{
    return std::distance(circuit.begin(), circuit.end());
}


TEST_CASE("transpile_to_primitive()")
{
    using G = mqis::Gate;
    using Matrices = std::vector<mqis::Matrix2X2>;

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
            mqis::QuantumState {"0"},
            mqis::QuantumState {"1"},
            mqis::QuantumState {{{M_SQRT1_2, 0.0}, {M_SQRT1_2, 0.0}}},
            mqis::QuantumState {{{0.0, M_SQRT1_2}, {0.0, M_SQRT1_2}}},
            mqis::QuantumState {{{0.0, 1.0}, {0.0, 0.0}}}
        );

        // create the original circuit
        auto original = mqis::QuantumCircuit {1};
        for (const auto& unitary : unitaries) {
            original.add_u_gate(unitary, 0);
        }

        // create the transpiled circuit
        const auto transpiled = mqis::transpile_to_primitive(original);

        // create a state and propagate it through both circuits
        auto state1 = state0;

        mqis::simulate(original, state0);
        mqis::simulate(transpiled, state1);

        for (const auto& gate : transpiled) {
            REQUIRE(impl_mqis::gate_id::is_single_qubit_transform_gate(gate.gate));
            REQUIRE(gate.gate != mqis::Gate::U);
        }

        REQUIRE(mqis::almost_eq(state0, state1));
    }
}
