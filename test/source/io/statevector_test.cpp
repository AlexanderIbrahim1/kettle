#include <cstddef>
#include <sstream>

#include <catch2/catch_test_macros.hpp>

#include <mini-qiskit/circuit/circuit.hpp>
#include <mini-qiskit/simulation/simulate.hpp>
#include <mini-qiskit/state/state.hpp>
#include <mini-qiskit/io/statevector.hpp>


TEST_CASE("read_numpy_statevector()")
{
    // create the state that we intend to save; circuit makes the state a little less trivial
    const auto state = []() {
        auto circuit = mqis::QuantumCircuit {3};
        circuit.add_h_gate({0, 1, 2});
        circuit.add_x_gate({0});
        circuit.add_y_gate({1, 2});
        circuit.add_rx_gate({{0, M_PI_4}, {2, M_PI_2}});

        auto state_ = mqis::QuantumState {3};
        mqis::simulate(circuit, state_);

        return state_;
    }();

    auto stream = std::stringstream {};
    mqis::save_statevector(stream, state);

    const auto loaded_state = mqis::load_statevector(stream);

    REQUIRE(mqis::almost_eq(state, loaded_state));
}
