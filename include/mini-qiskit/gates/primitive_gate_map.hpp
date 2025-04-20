#pragma once

#include <algorithm>
#include <array>
#include <stdexcept>

#include "mini-qiskit/common/linear_bijective_map.hpp"
#include "mini-qiskit/gates/primitive_gate.hpp"

namespace impl_mqis
{

static constexpr auto UNCONTROLLED_TO_CONTROLLED_GATE = LinearBijectiveMap<mqis::Gate, mqis::Gate, 10> {
    std::pair {mqis::Gate::H, mqis::Gate::CH},
    std::pair {mqis::Gate::X, mqis::Gate::CX},
    std::pair {mqis::Gate::Y, mqis::Gate::CY},
    std::pair {mqis::Gate::Z, mqis::Gate::CZ},
    std::pair {mqis::Gate::SX, mqis::Gate::CSX},
    std::pair {mqis::Gate::RX, mqis::Gate::CRX},
    std::pair {mqis::Gate::RY, mqis::Gate::CRY},
    std::pair {mqis::Gate::RZ, mqis::Gate::CRZ},
    std::pair {mqis::Gate::P, mqis::Gate::CP},
    std::pair {mqis::Gate::U, mqis::Gate::CU}
};

}  // namespace impl_mqis
