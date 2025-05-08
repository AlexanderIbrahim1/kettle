#pragma once

#include "kettle/common/tolerance.hpp"

/*
    This header file contains the `transpile_to_primitive()` function, which takes an
    existing `QuantumCircuit` instance that may contain matrices that use unitary 2x2
    matrices as gates, and creates a new `QuantumCircuit` instance composed of only
    primitive gates.
*/

namespace ket
{

class QuantumCircuit;

auto transpile_to_primitive(
    const QuantumCircuit& circuit,
    double tolerance_sq = ket::COMPLEX_ALMOST_EQ_TOLERANCE_SQ
) -> QuantumCircuit;

}  // namespace ket
