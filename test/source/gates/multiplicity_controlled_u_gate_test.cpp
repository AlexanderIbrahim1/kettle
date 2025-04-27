#include <cstddef>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "kettle/circuit/circuit.hpp"
#include "kettle/gates/common_u_gates.hpp"
#include "kettle/gates/multiplicity_controlled_u_gate.hpp"
#include "kettle/gates/toffoli.hpp"
#include "kettle/simulation/simulate.hpp"
#include "kettle/state/state.hpp"

template <typename CircuitFunction>
static auto create_state(CircuitFunction func, const std::string& init_bitstring, std::size_t n_qubits)
{
    auto circuit = ket::QuantumCircuit {n_qubits};
    func(circuit);
    auto state = ket::QuantumState {init_bitstring};
    ket::simulate(circuit, state);

    return state;
}

TEST_CASE("multiplicity-controlled X gate test")
{
    // clang-format off
    SECTION("mimic CX gate") {
        struct CTPair
        {
            std::size_t control;
            std::size_t target;
        };

        const auto init_bitstring = std::string {GENERATE("00", "10", "01", "11")};
        const auto [i_control, i_target] = GENERATE(CTPair {0, 1}, CTPair {1, 0});

        const auto modify_via_cx = [&] (ket::QuantumCircuit& circuit) {
            circuit.add_cx_gate(i_control, i_target);
        };

        const auto modify_via_mcu = [&] (ket::QuantumCircuit& circuit) {
            ket::apply_multiplicity_controlled_u_gate(circuit, ket::x_gate(), i_target, {i_control});
        };

        const auto state_from_cx = create_state(modify_via_cx, init_bitstring, 2);
        const auto state_from_mcu = create_state(modify_via_mcu, init_bitstring, 2);

        REQUIRE(ket::almost_eq(state_from_cx, state_from_mcu));
    }

    SECTION("mimic toffoli gate") {
        struct ToffoliQubits
        {
            std::size_t control0;
            std::size_t control1;
            std::size_t target;
        };

        const auto init_bitstring = std::string {
            GENERATE("000", "100", "010", "110", "001", "101", "011", "111")
        };

        const auto [control0, control1, target] = GENERATE(
            ToffoliQubits {0, 1, 2},
            ToffoliQubits {1, 0, 2},
            ToffoliQubits {1, 2, 0},
            ToffoliQubits {2, 1, 0},
            ToffoliQubits {0, 2, 1},
            ToffoliQubits {2, 0, 1}
        );

        const auto modify_via_toffoli = [&] (ket::QuantumCircuit& circuit) {
            ket::apply_toffoli_gate(circuit, {control0, control1}, target);
        };

        const auto modify_via_mcu = [&] (ket::QuantumCircuit& circuit) {
            ket::apply_multiplicity_controlled_u_gate(circuit, ket::x_gate(), target, {control0, control1});
        };

        const auto state_from_cx = create_state(modify_via_toffoli, init_bitstring, 3);
        const auto state_from_mcu = create_state(modify_via_mcu, init_bitstring, 3);

        REQUIRE(ket::almost_eq(state_from_cx, state_from_mcu));
    }

    SECTION("3-control toffoli")
    {
        struct TestInfo
        {
            std::array<std::size_t, 3> control_qubits;
            std::size_t target_qubit;
            std::string input_bitstring;
            std::string expected_output_bitstring;
        };

        const auto info = GENERATE(
            TestInfo{{0, 1, 2}, 3, "0000", "0000"},
            TestInfo{{0, 1, 2}, 3, "1000", "1000"},
            TestInfo{{0, 1, 2}, 3, "0100", "0100"},
            TestInfo{{0, 1, 2}, 3, "1100", "1100"},
            TestInfo{{0, 1, 2}, 3, "0010", "0010"},
            TestInfo{{0, 1, 2}, 3, "1010", "1010"},
            TestInfo{{0, 1, 2}, 3, "0110", "0110"},
            TestInfo{{0, 1, 2}, 3, "1110", "1111"},  // changes
            TestInfo{{0, 1, 2}, 3, "0001", "0001"},
            TestInfo{{0, 1, 2}, 3, "1001", "1001"},
            TestInfo{{0, 1, 2}, 3, "0101", "0101"},
            TestInfo{{0, 1, 2}, 3, "1101", "1101"},
            TestInfo{{0, 1, 2}, 3, "0011", "0011"},
            TestInfo{{0, 1, 2}, 3, "1011", "1011"},
            TestInfo{{0, 1, 2}, 3, "0111", "0111"},
            TestInfo{{0, 1, 2}, 3, "1111", "1110"},  // changes
            TestInfo{{0, 1, 3}, 2, "0000", "0000"},
            TestInfo{{0, 1, 3}, 2, "1000", "1000"},
            TestInfo{{0, 1, 3}, 2, "0100", "0100"},
            TestInfo{{0, 1, 3}, 2, "1100", "1100"},
            TestInfo{{0, 1, 3}, 2, "0010", "0010"},
            TestInfo{{0, 1, 3}, 2, "1010", "1010"},
            TestInfo{{0, 1, 3}, 2, "0110", "0110"},
            TestInfo{{0, 1, 3}, 2, "1110", "1110"},
            TestInfo{{0, 1, 3}, 2, "0001", "0001"},
            TestInfo{{0, 1, 3}, 2, "1001", "1001"},
            TestInfo{{0, 1, 3}, 2, "0101", "0101"},
            TestInfo{{0, 1, 3}, 2, "1101", "1111"},  // changes
            TestInfo{{0, 1, 3}, 2, "0011", "0011"},
            TestInfo{{0, 1, 3}, 2, "1011", "1011"},
            TestInfo{{0, 1, 3}, 2, "0111", "0111"},
            TestInfo{{0, 1, 3}, 2, "1111", "1101"},  // changes
            TestInfo{{1, 3, 2}, 0, "0000", "0000"},
            TestInfo{{1, 3, 2}, 0, "1000", "1000"},
            TestInfo{{1, 3, 2}, 0, "0100", "0100"},
            TestInfo{{1, 3, 2}, 0, "1100", "1100"},
            TestInfo{{1, 3, 2}, 0, "0010", "0010"},
            TestInfo{{1, 3, 2}, 0, "1010", "1010"},
            TestInfo{{1, 3, 2}, 0, "0110", "0110"},
            TestInfo{{1, 3, 2}, 0, "1110", "1110"},
            TestInfo{{1, 3, 2}, 0, "0001", "0001"},
            TestInfo{{1, 3, 2}, 0, "1001", "1001"},
            TestInfo{{1, 3, 2}, 0, "0101", "0101"},
            TestInfo{{1, 3, 2}, 0, "1101", "1101"},
            TestInfo{{1, 3, 2}, 0, "0011", "0011"},
            TestInfo{{1, 3, 2}, 0, "1011", "1011"},
            TestInfo{{1, 3, 2}, 0, "0111", "1111"},  // changes
            TestInfo{{1, 3, 2}, 0, "1111", "0111"}   // changes
        );

        const auto modify_via_mcu = [&] (ket::QuantumCircuit& circuit) {
            ket::apply_multiplicity_controlled_u_gate(
                circuit, ket::x_gate(), info.target_qubit, info.control_qubits);
        };

        const auto state_from_mcu = create_state(modify_via_mcu, info.input_bitstring, 4);

        const auto expected_state = ket::QuantumState {info.expected_output_bitstring};

        REQUIRE(ket::almost_eq(state_from_mcu, expected_state));
    }
    // clang-format on
}
