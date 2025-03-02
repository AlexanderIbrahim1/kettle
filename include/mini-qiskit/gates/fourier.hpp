#pragma once

#include <iterator>

#include "mini-qiskit/circuit.hpp"
#include "mini-qiskit/common/mathtools.hpp"
#include "mini-qiskit/common/utils.hpp"

namespace mqis
{

template <impl_mqis::ContainerOfQubitIndices Container = std::initializer_list<std::size_t>>
void apply_forward_fourier_transform(QuantumCircuit& circuit, const Container& container)
{
    const auto size = static_cast<std::size_t>(std::distance(container.begin(), container.end()));

    // perform the combination of Hadamard gates and controlled RZ gates
    for (std::size_t i_target_pre {0}; i_target_pre < size; ++i_target_pre) {
        const auto i_target = impl_mqis::get_container_index(container, i_target_pre);
        circuit.add_h_gate(i_target);

        auto i_angle_denom = std::size_t {2};
        for (std::size_t i_control_pre {i_target_pre + 1}; i_control_pre < size; ++i_control_pre) {
            const auto i_control = impl_mqis::get_container_index(container, i_control_pre);
            const auto angle = 2.0 * M_PI / static_cast<double>(impl_mqis::pow_2_int(i_angle_denom));
            circuit.add_cp_gate(angle, i_control, i_target);
            ++i_angle_denom;
        }
    }

    // apply the swaps
    auto i_left_pre = std::size_t {0};
    auto i_right_pre = size - 1;

    while (i_right_pre > i_left_pre) {
        const auto i_left = impl_mqis::get_container_index(container, i_left_pre);
        const auto i_right = impl_mqis::get_container_index(container, i_right_pre);
        circuit.add_cx_gate(i_left, i_right);
        circuit.add_cx_gate(i_right, i_left);
        circuit.add_cx_gate(i_left, i_right);

        ++i_left_pre;
        --i_right_pre;
    }
}

}  // namespace mqis
