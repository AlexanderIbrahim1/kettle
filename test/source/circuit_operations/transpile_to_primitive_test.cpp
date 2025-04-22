#include <string>
#include <optional>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "kettle/circuit/circuit.hpp"
#include "kettle/circuit_operations/transpile_to_primitive.hpp"
#include "kettle/simulation/simulate.hpp"


/*
    Create a unitary 2x2 matrix that is a product of primitive gates.
*/
constexpr auto make_matrix(const std::vector<impl_ket::PrimitiveGateInfo>& pg_infos) -> ket::Matrix2X2
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


constexpr auto n_gates(const ket::QuantumCircuit& circuit) -> std::ptrdiff_t
{
    return std::distance(circuit.begin(), circuit.end());
}


TEST_CASE("transpile_to_primitive()")
{
    using G = ket::Gate;
    using Matrices = std::vector<ket::Matrix2X2>;

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

        for (const auto& gate : transpiled) {
            REQUIRE(impl_ket::gate_id::is_single_qubit_transform_gate(gate.gate));
            REQUIRE(gate.gate != ket::Gate::U);
        }

        REQUIRE(ket::almost_eq(state0, state1));
    }
}
