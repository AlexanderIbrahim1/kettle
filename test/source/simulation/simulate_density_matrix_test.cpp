#include <functional>
#include <string>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#define REQUIRE_MSG(cond, msg) do { INFO(msg); REQUIRE(cond); } while((void)0, 0)

#include <Eigen/Dense>

#include "kettle/circuit/circuit.hpp"
#include "kettle/gates/common_u_gates.hpp"
#include "kettle/gates/random_u_gates.hpp"
#include "kettle/state/density_matrix.hpp"
#include "kettle/state/statevector.hpp"
#include "kettle/simulation/simulate_density_matrix.hpp"
#include "kettle/simulation/simulate.hpp"

#include "kettle_internal/common/state_test_utils.hpp"
#include "kettle_internal/gates/primitive_gate_map.hpp"


TEST_CASE("compare density matrix u-gate simulations with statevector u-gate simulations")
{
    struct TestCase
    {
        std::string message;
        std::string init_bitstring;
        std::function<void(ket::QuantumCircuit&)> circ_func;
    };

    const auto testcase = GENERATE_COPY(
        TestCase {
            "H on 0, 1 qubit",
            "0",
            [](ket::QuantumCircuit& circ) {
                circ.add_u_gate(ket::h_gate(), 0);
            }
        },
        TestCase {
            "X on 0, 1 qubit",
            "0",
            [](ket::QuantumCircuit& circ) {
                circ.add_u_gate(ket::x_gate(), 0);
            }
        },
        TestCase {
            "H on 0, 2 qubits",
            "00",
            [](ket::QuantumCircuit& circ) {
                circ.add_u_gate(ket::h_gate(), 0);
            }
        },
        TestCase {
            "H on 1, 2 qubits",
            "00",
            [](ket::QuantumCircuit& circ) {
                circ.add_u_gate(ket::h_gate(), 1);
            }
        },
        TestCase {
            "H on 0, X on 1, 2 qubits",
            "00",
            [](ket::QuantumCircuit& circ) {
                circ.add_u_gate(ket::h_gate(), 1);
                circ.add_u_gate(ket::x_gate(), 0);
            }
        },
        TestCase {
            "H on 0, 3 qubits",
            "000",
            [](ket::QuantumCircuit& circ) {
                circ.add_u_gate(ket::h_gate(), 0);
            }
        },
        TestCase {
            "H on 1, 3 qubits",
            "000",
            [](ket::QuantumCircuit& circ) {
                circ.add_u_gate(ket::h_gate(), 1);
            }
        },
        TestCase {
            "H on 2, 3 qubits",
            "000",
            [](ket::QuantumCircuit& circ) {
                circ.add_u_gate(ket::h_gate(), 2);
            }
        },
        TestCase {
            "H on each qubit, 3 qubits",
            "000",
            [](ket::QuantumCircuit& circ) {
            circ.add_u_gate(ket::h_gate(), {0, 1, 2});
        }},
        TestCase {
            "bell(00+)",
            "00",
            [](ket::QuantumCircuit& circ) {
            circ.add_u_gate(ket::h_gate(), 0);
            circ.add_cu_gate(ket::x_gate(), 0, 1);
        }},
        TestCase {
            "bell(00-)",
            "00",
            [](ket::QuantumCircuit& circ) {
            circ.add_u_gate(ket::x_gate(), 0);
            circ.add_u_gate(ket::h_gate(), 0);
            circ.add_cu_gate(ket::x_gate(), 0, 1);
        }},
        TestCase {
            "bell(01+)",
            "00",
            [](ket::QuantumCircuit& circ) {
            circ.add_u_gate(ket::x_gate(), 1);
            circ.add_u_gate(ket::h_gate(), 0);
            circ.add_cu_gate(ket::x_gate(), 0, 1);
        }},
        TestCase {
            "bell(01-)",
            "00",
            [](ket::QuantumCircuit& circ) {
            circ.add_u_gate(ket::x_gate(), {0, 1});
            circ.add_u_gate(ket::h_gate(), 0);
            circ.add_cu_gate(ket::x_gate(), 0, 1);
        }},
        TestCase {
            "GHZ",
            "000",
            [](ket::QuantumCircuit& circ) {
            circ.add_u_gate(ket::h_gate(), 0);
            circ.add_cu_gate(ket::x_gate(), 0, 1);
            circ.add_cu_gate(ket::x_gate(), 0, 2);
        }}
    );

    auto circuit = ket::QuantumCircuit {testcase.init_bitstring.size()};
    testcase.circ_func(circuit);

    auto statevector = ket::Statevector {testcase.init_bitstring};
    ket::simulate(circuit, statevector);
    const auto from_statevector = ket::statevector_to_density_matrix(statevector);

    auto direct = ket::DensityMatrix {testcase.init_bitstring};
    ket::simulate(circuit, direct);

    REQUIRE_MSG(ket::internal::almost_eq_with_print_(direct, from_statevector), testcase.message);
}

TEST_CASE("compare density matrix primitive-gate simulations with statevector primitive-gate simulations")
{
    SECTION("1-qubit circuit, 1T gates")
    {
        struct TestCase {
            std::string message;
            std::function<void(ket::QuantumCircuit&)> circ_func;
        };

        const auto testcase = GENERATE(
            TestCase { "H, 1-qubit", [](ket::QuantumCircuit& circ) { circ.add_h_gate(0); }},
            TestCase { "X, 1-qubit", [](ket::QuantumCircuit& circ) { circ.add_x_gate(0); }},
            TestCase { "Y, 1-qubit", [](ket::QuantumCircuit& circ) { circ.add_y_gate(0); }},
            TestCase { "Z, 1-qubit", [](ket::QuantumCircuit& circ) { circ.add_z_gate(0); }},
            TestCase { "S, 1-qubit", [](ket::QuantumCircuit& circ) { circ.add_s_gate(0); }},
            TestCase { "SDAG, 1-qubit", [](ket::QuantumCircuit& circ) { circ.add_sdag_gate(0); }},
            TestCase { "T, 1-qubit", [](ket::QuantumCircuit& circ) { circ.add_t_gate(0); }},
            TestCase { "TDAG, 1-qubit", [](ket::QuantumCircuit& circ) { circ.add_tdag_gate(0); }},
            TestCase { "SX, 1-qubit", [](ket::QuantumCircuit& circ) { circ.add_sx_gate(0); }},
            TestCase { "SXDAG, 1-qubit", [](ket::QuantumCircuit& circ) { circ.add_sxdag_gate(0); }}
        );

        auto circuit = ket::QuantumCircuit {1};
        circuit.add_u_gate(ket::generate_random_unitary2x2(), 0);
        testcase.circ_func(circuit);

        auto statevector = ket::Statevector {"0"};
        ket::simulate(circuit, statevector);
        const auto from_statevector = ket::statevector_to_density_matrix(statevector);

        auto direct = ket::DensityMatrix {"0"};
        ket::simulate(circuit, direct);

        REQUIRE_MSG(ket::internal::almost_eq_with_print_(direct, from_statevector), testcase.message);
    }

    SECTION("1-qubit circuit, 1T1A gates")
    {
        struct TestCase {
            std::string message;
            std::function<void(ket::QuantumCircuit&, double)> circ_func;
        };

        const auto testcase = GENERATE(
            TestCase { "RX, 1-qubit", [](ket::QuantumCircuit& circ, double angle) { circ.add_rx_gate(0, angle); }},
            TestCase { "RY, 1-qubit", [](ket::QuantumCircuit& circ, double angle) { circ.add_ry_gate(0, angle); }},
            TestCase { "RZ, 1-qubit", [](ket::QuantumCircuit& circ, double angle) { circ.add_rz_gate(0, angle); }},
            TestCase { "P, 1-qubit", [](ket::QuantumCircuit& circ, double angle) { circ.add_p_gate(0, angle); }}
        );

        const auto angle = 2.0 * M_PI * GENERATE(0.0, 0.2, 0.4, 0.6, 0.8, 0.98);

        auto circuit = ket::QuantumCircuit {1};
        circuit.add_u_gate(ket::generate_random_unitary2x2(), 0);
        testcase.circ_func(circuit, angle);

        auto statevector = ket::Statevector {"0"};
        ket::simulate(circuit, statevector);
        const auto from_statevector = ket::statevector_to_density_matrix(statevector);

        auto direct = ket::DensityMatrix {"0"};
        ket::simulate(circuit, direct);

        REQUIRE_MSG(ket::internal::almost_eq_with_print_(direct, from_statevector), testcase.message);
    }

    SECTION("2-qubit circuit, 1C1T gates")
    {
        struct TestCase {
            std::string message;
            ket::Gate gate;
        };

        const auto add_gates = [](ket::QuantumCircuit& circ, ket::Gate gate, std::size_t ic, std::size_t it) {
            const auto func = ket::internal::GATE_TO_FUNCTION_1C1T.at(gate);
            (circ.*func)(ic, it);
        };

        const auto testcase = GENERATE(
            TestCase { "CH, 2-qubit", ket::Gate::CH },
            TestCase { "CX, 2-qubit", ket::Gate::CX },
            TestCase { "CY, 2-qubit", ket::Gate::CY },
            TestCase { "CZ, 2-qubit", ket::Gate::CZ },
            TestCase { "CS, 2-qubit", ket::Gate::CS },
            TestCase { "CSDAG, 2-qubit", ket::Gate::CSDAG },
            TestCase { "CT, 2-qubit", ket::Gate::CT },
            TestCase { "CTDAG, 2-qubit", ket::Gate::CTDAG },
            TestCase { "CSX, 2-qubit", ket::Gate::CSX},
            TestCase { "CSXDAG, 2-qubit", ket::Gate::CSXDAG }
        );

        const auto pair = GENERATE(
            std::pair<std::size_t, std::size_t> {0, 1},
            std::pair<std::size_t, std::size_t> {1, 0}
        );

        auto circuit = ket::QuantumCircuit {2};
        circuit.add_u_gate(ket::generate_random_unitary2x2(), 0);
        circuit.add_u_gate(ket::generate_random_unitary2x2(), 1);
        add_gates(circuit, testcase.gate, pair.first, pair.second);

        auto statevector = ket::Statevector {"00"};
        ket::simulate(circuit, statevector);
        const auto from_statevector = ket::statevector_to_density_matrix(statevector);

        auto direct = ket::DensityMatrix {"00"};
        ket::simulate(circuit, direct);

        REQUIRE_MSG(ket::internal::almost_eq_with_print_(direct, from_statevector), testcase.message);
    }

    SECTION("2-qubit circuit, 1C1T1A gates")
    {
        struct TestCase {
            std::string message;
            ket::Gate gate;
        };

        const auto add_gates = [](ket::QuantumCircuit& circ, ket::Gate gate, std::size_t ic, std::size_t it, double angle) {
            const auto func = ket::internal::GATE_TO_FUNCTION_1C1T1A.at(gate);
            (circ.*func)(ic, it, angle);
        };

        const auto testcase = GENERATE(
            TestCase { "CRX, 2-qubit", ket::Gate::CRX },
            TestCase { "CRY, 2-qubit", ket::Gate::CRY },
            TestCase { "CRZ, 2-qubit", ket::Gate::CRZ },
            TestCase { "CP, 2-qubit", ket::Gate::CP }
        );

        const auto pair = GENERATE(
            std::pair<std::size_t, std::size_t> {0, 1},
            std::pair<std::size_t, std::size_t> {1, 0}
        );

        const auto angle = 2.0 * M_PI * GENERATE(0.0, 0.2, 0.4, 0.6, 0.8, 0.98);

        auto circuit = ket::QuantumCircuit {2};
        circuit.add_u_gate(ket::generate_random_unitary2x2(), 0);
        circuit.add_u_gate(ket::generate_random_unitary2x2(), 1);
        add_gates(circuit, testcase.gate, pair.first, pair.second, angle);

        auto statevector = ket::Statevector {"00"};
        ket::simulate(circuit, statevector);
        const auto from_statevector = ket::statevector_to_density_matrix(statevector);

        auto direct = ket::DensityMatrix {"00"};
        ket::simulate(circuit, direct);

        REQUIRE_MSG(ket::internal::almost_eq_with_print_(direct, from_statevector), testcase.message);
    }
}
