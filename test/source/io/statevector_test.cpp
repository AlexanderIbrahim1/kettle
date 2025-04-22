#include <cstddef>
#include <sstream>

#include <catch2/catch_test_macros.hpp>

#include <kettle/circuit/circuit.hpp>
#include <kettle/simulation/simulate.hpp>
#include <kettle/state/state.hpp>
#include <kettle/io/statevector.hpp>


TEST_CASE("read_numpy_statevector()")
{
    // create the state that we intend to save; circuit makes the state a little less trivial
    const auto state = []() {
        auto circuit = ket::QuantumCircuit {3};
        circuit.add_h_gate({0, 1, 2});
        circuit.add_x_gate({0});
        circuit.add_y_gate({1, 2});
        circuit.add_rx_gate({{0, M_PI_4}, {2, M_PI_2}});

        auto state_ = ket::QuantumState {3};
        ket::simulate(circuit, state_);

        return state_;
    }();

    auto stream = std::stringstream {};
    ket::save_statevector(stream, state);

    const auto loaded_state = ket::load_statevector(stream);

    REQUIRE(ket::almost_eq(state, loaded_state));
}
