#pragma once

#include <algorithm>
#include <array>
#include <stdexcept>

#include "mini-qiskit/gates/primitive_gate.hpp"

namespace impl_mqis
{

template <typename Key, typename Value, std::size_t Size>
struct ConstexprLinearMap
{
    std::array<std::pair<Key, Value>, Size> data_;

    [[nodiscard]]
    constexpr auto at(const Key& key) const -> Value
    {
        const auto is_item = [&key](const auto& item) { return item.first == key; };
        const auto it = std::ranges::find_if(data_, is_item);

        if (it != std::end(data_)) {
            return it->second;
        } else {
            throw std::range_error("Key not found in ConstexprLinearMap\n");
        }
    }
};

static constexpr auto UNCONTROLLED_TO_CONTROLLED_GATE = ConstexprLinearMap<mqis::Gate, mqis::Gate, 10> {
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
