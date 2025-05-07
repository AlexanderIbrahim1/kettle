#pragma once

#include <cmath>

#include "kettle/circuit/circuit.hpp"


namespace ket
{

/*
    Checks if two `ket::QuantumCircuit` instances are equal, gate-by-gate.

    If two circuits result in the same propagation, but have the gates in the same order, then
    this function considers them different. For example, the following `circuit0` and `circuit1`
    instances are considered different:

        :
        auto circuit0 = ket::QuantumCircuit {2};
        circuit0.add_x_gate(0);
        circuit0.add_x_gate(1);
        auto circuit1 = ket::QuantumCircuit {2};
        circuit1.add_x_gate(1);
        circuit1.add_x_gate(0);
        :

    However, if one circuit has a primitive gate, and another circuit has a U-gate that performs the
    same transformation as that primitive gate (to within floating-point precision), then those two
    gates are considered matching. For example, the following `circuit0` and `circuit1` are considered
    the same:

        :
        auto circuit0 = ket::QuantumCircuit {1};
        circuit0.add_x_gate(0);
        auto circuit1 = ket::QuantumCircuit {1};
        circuit1.add_u_gate(ket::x_gate(), 0);
        :
    
    Loggers are ignored entirely
*/
auto almost_eq(
    const QuantumCircuit& left,
    const QuantumCircuit& right,
    double tol_sq = ket::internal::COMPLEX_ALMOST_EQ_TOLERANCE_SQ
) -> bool;

}  // namespace ket
