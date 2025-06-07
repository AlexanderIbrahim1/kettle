#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <kettle/circuit_operations/compare_circuits.hpp>
#include <kettle/gates/compound_gate.hpp>
#include <kettle/gates/primitive_gate.hpp>
#include <kettle/optimize/n_local.hpp>

using G = ket::Gate;
using CG = ket::CompoundGate;
using Entangle = ket::NLocalEntangelement;
using Skip = ket::SkipLastRotationLayerFlag;

TEST_CASE("n_local construction")
{
    SECTION("size check")
    {
        const auto n_qubits = static_cast<std::size_t>(GENERATE(2, 3, 4, 5));
        const auto n_repetitions = static_cast<std::size_t>(GENERATE(1, 2, 3, 4));
        const auto skip = GENERATE(Skip::FALSE, Skip::TRUE);

        [[maybe_unused]]
        const auto [n_local_circuit, ignore] = ket::n_local(n_qubits, {G::X}, {G::CX}, Entangle::LINEAR, n_repetitions, skip);

        const auto n_x_gates_expected = [&]() {
            auto output = n_qubits * n_repetitions;
            if (skip == Skip::FALSE) {
                output += n_qubits;
            }

            return output;
        }();

        const auto n_cx_gates_expected = (n_qubits - 1) * n_repetitions;

        REQUIRE(n_local_circuit.n_circuit_elements() == n_x_gates_expected + n_cx_gates_expected);
    }

    SECTION("n-qubits, rotations=[X], entanglements=[CX]")
    {
        const auto n_qubits = static_cast<std::size_t>(GENERATE(2, 3, 4, 5));
        const auto n_repetitions = static_cast<std::size_t>(GENERATE(1, 2, 3, 4));

        const auto [n_local_circuit, parameters] = ket::n_local(n_qubits, {G::X}, {G::CX}, Entangle::LINEAR, n_repetitions);

        const auto expected = [n_repetitions, n_qubits]() {
            auto circuit = ket::QuantumCircuit {n_qubits};

            for (std::size_t i_rep {0}; i_rep < n_repetitions; ++i_rep) {
                for (std::size_t i_qubit {0}; i_qubit < n_qubits; ++i_qubit) {
                    circuit.add_x_gate(i_qubit);
                }
                
                for (std::size_t i_qubit {0}; i_qubit < n_qubits - 1; ++i_qubit) {
                    circuit.add_cx_gate(i_qubit, i_qubit + 1);
                }
            }
            for (std::size_t i_qubit {0}; i_qubit < n_qubits; ++i_qubit) {
                circuit.add_x_gate(i_qubit);
            }

            return circuit;
        }();

        REQUIRE(ket::almost_eq(n_local_circuit, expected));
        REQUIRE(parameters.size() == 0);
    }

    SECTION("n-qubits, rotations=[X, Y], entanglements=[CX]")
    {
        const auto n_qubits = static_cast<std::size_t>(GENERATE(2, 3, 4, 5));
        const auto n_repetitions = static_cast<std::size_t>(GENERATE(1, 2, 3, 4));

        const auto [n_local_circuit, parameters] = ket::n_local(n_qubits, {G::X, G::Y}, {G::CX}, Entangle::LINEAR, n_repetitions);

        const auto expected = [n_repetitions, n_qubits]() {
            auto circuit = ket::QuantumCircuit {n_qubits};

            for (std::size_t i_rep {0}; i_rep < n_repetitions; ++i_rep) {
                for (std::size_t i_qubit {0}; i_qubit < n_qubits; ++i_qubit) {
                    circuit.add_x_gate(i_qubit);
                }

                for (std::size_t i_qubit {0}; i_qubit < n_qubits; ++i_qubit) {
                    circuit.add_y_gate(i_qubit);
                }
                
                for (std::size_t i_qubit {0}; i_qubit < n_qubits - 1; ++i_qubit) {
                    circuit.add_cx_gate(i_qubit, i_qubit + 1);
                }
            }
            for (std::size_t i_qubit {0}; i_qubit < n_qubits; ++i_qubit) {
                circuit.add_x_gate(i_qubit);
            }
            for (std::size_t i_qubit {0}; i_qubit < n_qubits; ++i_qubit) {
                circuit.add_y_gate(i_qubit);
            }

            return circuit;
        }();

        REQUIRE(ket::almost_eq(n_local_circuit, expected));
        REQUIRE(parameters.size() == 0);
    }

    SECTION("n-qubits, rotations=[RX], entanglements=[CX]")
    {
        const auto n_qubits = static_cast<std::size_t>(GENERATE(2, 3, 4, 5));
        const auto n_repetitions = static_cast<std::size_t>(GENERATE(1, 2, 3, 4));

        const auto [n_local_circuit, parameters] = ket::n_local(n_qubits, {G::RX}, {G::CX}, Entangle::LINEAR, n_repetitions);

        const auto expected = [n_repetitions, n_qubits]() {
            auto circuit = ket::QuantumCircuit {n_qubits};

            for (std::size_t i_rep {0}; i_rep < n_repetitions; ++i_rep) {
                for (std::size_t i_qubit {0}; i_qubit < n_qubits; ++i_qubit) {
                    circuit.add_rx_gate(i_qubit, ket::DEFAULT_NLOCAL_GATE_PARAMETER);
                }
                
                for (std::size_t i_qubit {0}; i_qubit < n_qubits - 1; ++i_qubit) {
                    circuit.add_cx_gate(i_qubit, i_qubit + 1);
                }
            }
            for (std::size_t i_qubit {0}; i_qubit < n_qubits; ++i_qubit) {
                circuit.add_rx_gate(i_qubit, ket::DEFAULT_NLOCAL_GATE_PARAMETER);
            }

            return circuit;
        }();

        REQUIRE(ket::almost_eq(n_local_circuit, expected));
        REQUIRE(parameters.size() == n_qubits * (n_repetitions + 1));
    }

    SECTION("n-qubits, rotations=[RX, RY], entanglements=[CX]")
    {
        const auto n_qubits = static_cast<std::size_t>(GENERATE(2, 3, 4, 5));
        const auto n_repetitions = static_cast<std::size_t>(GENERATE(1, 2, 3, 4));

        const auto [n_local_circuit, parameters] = ket::n_local(n_qubits, {G::RX, G::RY}, {G::CX}, Entangle::LINEAR, n_repetitions);

        const auto expected = [n_repetitions, n_qubits]() {
            auto circuit = ket::QuantumCircuit {n_qubits};

            for (std::size_t i_rep {0}; i_rep < n_repetitions; ++i_rep) {
                for (std::size_t i_qubit {0}; i_qubit < n_qubits; ++i_qubit) {
                    circuit.add_rx_gate(i_qubit, ket::DEFAULT_NLOCAL_GATE_PARAMETER);
                }
                for (std::size_t i_qubit {0}; i_qubit < n_qubits; ++i_qubit) {
                    circuit.add_ry_gate(i_qubit, ket::DEFAULT_NLOCAL_GATE_PARAMETER);
                }
                
                for (std::size_t i_qubit {0}; i_qubit < n_qubits - 1; ++i_qubit) {
                    circuit.add_cx_gate(i_qubit, i_qubit + 1);
                }
            }
            for (std::size_t i_qubit {0}; i_qubit < n_qubits; ++i_qubit) {
                circuit.add_rx_gate(i_qubit, ket::DEFAULT_NLOCAL_GATE_PARAMETER);
            }
            for (std::size_t i_qubit {0}; i_qubit < n_qubits; ++i_qubit) {
                circuit.add_ry_gate(i_qubit, ket::DEFAULT_NLOCAL_GATE_PARAMETER);
            }

            return circuit;
        }();

        REQUIRE(ket::almost_eq(n_local_circuit, expected));
        REQUIRE(parameters.size() == 2 * n_qubits * (n_repetitions + 1));
    }

    SECTION("n qubits, rotations=[RX, RY], entanglements=[CCX], one repetition")
    {
        const auto n_qubits = static_cast<std::size_t>(GENERATE(3, 4, 5));

        const auto [n_local_circuit, parameters] = ket::n_local(n_qubits, {G::RX, G::RY}, {CG::CCX}, Entangle::LINEAR, 1);

        const auto expected = [n_qubits]() {
            auto circuit = ket::QuantumCircuit {n_qubits};

            for (std::size_t i_qubit {0}; i_qubit < n_qubits; ++i_qubit) {
                circuit.add_rx_gate(i_qubit, ket::DEFAULT_NLOCAL_GATE_PARAMETER);
            }
            for (std::size_t i_qubit {0}; i_qubit < n_qubits; ++i_qubit) {
                circuit.add_ry_gate(i_qubit, ket::DEFAULT_NLOCAL_GATE_PARAMETER);
            }
            
            for (std::size_t i_qubit {0}; i_qubit < n_qubits - 2; ++i_qubit) {
                circuit.add_ccx_gate(i_qubit, i_qubit + 1, i_qubit + 2);
            }

            for (std::size_t i_qubit {0}; i_qubit < n_qubits; ++i_qubit) {
                circuit.add_rx_gate(i_qubit, ket::DEFAULT_NLOCAL_GATE_PARAMETER);
            }
            for (std::size_t i_qubit {0}; i_qubit < n_qubits; ++i_qubit) {
                circuit.add_ry_gate(i_qubit, ket::DEFAULT_NLOCAL_GATE_PARAMETER);
            }

            return circuit;
        }();

        REQUIRE(ket::almost_eq(n_local_circuit, expected));
        REQUIRE(parameters.size() == 4 * n_qubits);
    }

    SECTION("n-qubits, rotations=[RX, RY], entanglements=[CRX, CRZ]")
    {
        const auto n_qubits = static_cast<std::size_t>(GENERATE(2, 3, 4, 5));
        const auto n_repetitions = static_cast<std::size_t>(GENERATE(1, 2, 3, 4));

        const auto [n_local_circuit, parameters] = ket::n_local(n_qubits, {G::RX, G::RY}, {G::CRX, G::CRZ}, Entangle::LINEAR, n_repetitions);

        const auto expected = [n_repetitions, n_qubits]() {
            auto circuit = ket::QuantumCircuit {n_qubits};

            for (std::size_t i_rep {0}; i_rep < n_repetitions; ++i_rep) {
                for (std::size_t i_qubit {0}; i_qubit < n_qubits; ++i_qubit) {
                    circuit.add_rx_gate(i_qubit, ket::DEFAULT_NLOCAL_GATE_PARAMETER);
                }
                for (std::size_t i_qubit {0}; i_qubit < n_qubits; ++i_qubit) {
                    circuit.add_ry_gate(i_qubit, ket::DEFAULT_NLOCAL_GATE_PARAMETER);
                }
                
                for (std::size_t i_qubit {0}; i_qubit < n_qubits - 1; ++i_qubit) {
                    circuit.add_crx_gate(i_qubit, i_qubit + 1, ket::DEFAULT_NLOCAL_GATE_PARAMETER);
                }
                for (std::size_t i_qubit {0}; i_qubit < n_qubits - 1; ++i_qubit) {
                    circuit.add_crz_gate(i_qubit, i_qubit + 1, ket::DEFAULT_NLOCAL_GATE_PARAMETER);
                }
            }
            for (std::size_t i_qubit {0}; i_qubit < n_qubits; ++i_qubit) {
                circuit.add_rx_gate(i_qubit, ket::DEFAULT_NLOCAL_GATE_PARAMETER);
            }
            for (std::size_t i_qubit {0}; i_qubit < n_qubits; ++i_qubit) {
                circuit.add_ry_gate(i_qubit, ket::DEFAULT_NLOCAL_GATE_PARAMETER);
            }

            return circuit;
        }();

        const auto n_rotation_parameters = 2 * n_qubits * (n_repetitions + 1);
        const auto n_entangle_parameters = 2 * (n_qubits - 1) * n_repetitions;

        REQUIRE(ket::almost_eq(n_local_circuit, expected));
        REQUIRE(parameters.size() == n_rotation_parameters + n_entangle_parameters);
    }
}


TEST_CASE("n_local assign parameters")
{
    // set the parameter values at the very beginning
    const auto parameter_values = std::vector<double> {1.1, 2.2, 3.3, 4.4, 5.5, 6.6};

    // create the n-local circuit from the n-local function, and assign the parameters
    const auto n_qubits = 2;
    const auto n_repetitions = 2;
    auto [n_local_circuit, parameter_ids] = ket::n_local(n_qubits, {G::RX}, {G::CX}, Entangle::LINEAR, n_repetitions);

    REQUIRE(parameter_ids.size() == parameter_values.size());

    for (std::size_t i {0}; i < parameter_ids.size(); ++i) {
        n_local_circuit.set_parameter_value(parameter_ids[i], parameter_values[i]);
    }

    // create an identical circuit manually
    const auto identical_circuit = [&]() {
        auto circuit = ket::QuantumCircuit {2};
        circuit.add_rx_gate(0, parameter_values[0]);
        circuit.add_rx_gate(1, parameter_values[1]);
        circuit.add_cx_gate(0, 1);
        circuit.add_rx_gate(0, parameter_values[2]);
        circuit.add_rx_gate(1, parameter_values[3]);
        circuit.add_cx_gate(0, 1);
        circuit.add_rx_gate(0, parameter_values[4]);
        circuit.add_rx_gate(1, parameter_values[5]);

        return circuit;
    }();

    REQUIRE(ket::almost_eq(n_local_circuit, identical_circuit));
}
