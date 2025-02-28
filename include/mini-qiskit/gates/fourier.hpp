#pragma once

#include "mini-qiskit/circuit.hpp"
#include "mini-qiskit/common/mathtools.hpp"
#include "mini-qiskit/common/utils.hpp"

namespace mqis
{

template <impl_mqis::ContainerOfQubitIndices Container = std::initializer_list<std::size_t>>
void apply_forward_fourier_transform(const QuantumCircuit& circuit, const Container& container)
{
    const auto size = std::distance(container.begin(), container.end());

    for (std::size_t i_target_pre {0}; i_target_pre < size; ++i_target_pre) {
        const auto i_target = container[i_target_pre];
        circuit.add_h_gate(i_target);

        for (std::size_t i_control_pre {1}; i_control_pre < size; ++i_control_pre) {
            const auto i_control = container[i_control_pre];
            const auto angle = 2.0 * M_PI / static_cast<double>(impl_mqis::pow_2_int(i_control_pre + 1));
            circuit.add_crz_gate(angle, i_control, i_target);
        }
    }
}

}  // namespace mqis
