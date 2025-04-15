#pragma once

#include <iterator>
#include <ranges>

#include "mini-qiskit/circuit/circuit.hpp"
#include "mini-qiskit/common/mathtools.hpp"
#include "mini-qiskit/common/utils.hpp"
#include "mini-qiskit/gates/swap.hpp"

namespace impl_mqis
{

template <impl_mqis::QubitIndices Container = impl_mqis::QubitIndicesIList>
void apply_fourier_transform_swaps_(mqis::QuantumCircuit& circuit, const Container& container)
{
    const auto size = static_cast<std::size_t>(std::distance(container.begin(), container.end()));

    // apply the swaps
    auto i_left_pre = std::size_t {0};
    auto i_right_pre = size - 1;

    while (i_right_pre > i_left_pre) {
        const auto i_left = impl_mqis::get_container_index(container, i_left_pre);
        const auto i_right = impl_mqis::get_container_index(container, i_right_pre);
        mqis::apply_swap(circuit, i_left, i_right);

        ++i_left_pre;
        --i_right_pre;
    }
}

}  // namespace impl_mqis


namespace mqis
{

template <impl_mqis::QubitIndices Container = impl_mqis::QubitIndicesIList>
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
            circuit.add_cp_gate(i_control, i_target, angle);
            ++i_angle_denom;
        }
    }

    // apply the swaps
    impl_mqis::apply_fourier_transform_swaps_(circuit, container);
}

template <impl_mqis::QubitIndices Container = impl_mqis::QubitIndicesIList>
void apply_inverse_fourier_transform(QuantumCircuit& circuit, const Container& container)
{
    namespace sv = std::views;

    // apply the swaps
    impl_mqis::apply_fourier_transform_swaps_(circuit, container);

    const auto size = static_cast<std::size_t>(std::distance(container.begin(), container.end()));

    for (std::size_t i_target_pre : sv::iota(0ul, size) | sv::reverse) {
        const auto i_target = impl_mqis::get_container_index(container, i_target_pre);

        auto i_angle_denom = size - i_target_pre;
        for (std::size_t i_control_pre : sv::iota(i_target_pre + 1, size) | sv::reverse) {
            const auto i_control = impl_mqis::get_container_index(container, i_control_pre);
            const auto angle = 2.0 * M_PI / static_cast<double>(impl_mqis::pow_2_int(i_angle_denom));
            circuit.add_cp_gate(i_control, i_target, -angle);
            --i_angle_denom;
        }

        circuit.add_h_gate(i_target);
    }
}

}  // namespace mqis
