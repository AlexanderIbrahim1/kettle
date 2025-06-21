#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

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

TEST_CASE("...")
{
    SECTION("bell_state(00+)")
    {
        auto sv_circuit = ket::QuantumCircuit {2};
        sv_circuit.add_h_gate(0);
        sv_circuit.add_cx_gate(0, 1);

        auto statevector = ket::Statevector {"00"};
        ket::simulate(sv_circuit, statevector);

        const auto from_statevector = ket::statevector_to_density_matrix(statevector);

        auto dm_circuit = ket::QuantumCircuit {2};
        dm_circuit.add_u_gate(ket::h_gate(), 0);
        dm_circuit.add_cu_gate(ket::x_gate(), 0, 1);

        auto direct = ket::DensityMatrix {"00"};
        ket::simulate(dm_circuit, direct);

        REQUIRE(ket::internal::almost_eq_with_print_(direct, from_statevector));
    }
}
