#pragma once

#include "kettle/gates/compound_gate.hpp"


namespace ket::internal::gate_id
{

auto is_doubly_controled_gate(ket::CompoundGate gate) -> bool;

}  // namespace ket::internal::gate_id
