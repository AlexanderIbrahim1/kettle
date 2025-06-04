#include "kettle/gates/compound_gate.hpp"
#include "kettle_internal/gates/compound_gate/gate_id.hpp"


namespace ket::internal
{

auto is_doubly_controled_gate(ket::CompoundGate gate) -> bool
{
    using CG = ket::CompoundGate;
    return gate == CG::CCX || gate == CG::CCY || gate == CG::CCZ;
}

}  // namespace ket::internal
