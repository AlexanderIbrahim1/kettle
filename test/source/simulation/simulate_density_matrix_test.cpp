#include <functional>
#include <string>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#define REQUIRE_MSG(cond, msg) do { INFO(msg); REQUIRE(cond); } while((void)0, 0)

#include <Eigen/Dense>

#include "kettle/circuit/circuit.hpp"
#include "kettle/gates/common_u_gates.hpp"
#include "kettle/state/density_matrix.hpp"
#include "kettle/state/statevector.hpp"
#include "kettle/simulation/simulate_density_matrix.hpp"
#include "kettle/simulation/simulate.hpp"

#include "kettle_internal/common/state_test_utils.hpp"

// unit testing ideas:
// - create a statevector using certain gates
//   - turn it into a density matrix
// - create a density matrix using the same gates
// - compare the two

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
