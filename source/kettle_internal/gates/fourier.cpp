#include <iterator>
#include <ranges>

#include "kettle/circuit/circuit.hpp"
#include "kettle/common/mathtools.hpp"
#include "kettle/common/utils.hpp"
#include "kettle/gates/fourier.hpp"
#include "kettle/gates/swap.hpp"

#include "kettle_internal/common/utils_internal.hpp"

namespace
{

template <ket::QubitIndices Container = ket::QubitIndicesIList>
void apply_fourier_transform_swaps_(ket::QuantumCircuit& circuit, const Container& container)
{
    const auto size = static_cast<std::size_t>(std::distance(container.begin(), container.end()));

    // apply the swaps
    auto i_left_pre = std::size_t {0};
    auto i_right_pre = size - 1;

    while (i_right_pre > i_left_pre) {
        const auto i_left = ket::internal::get_container_index(container, i_left_pre);
        const auto i_right = ket::internal::get_container_index(container, i_right_pre);
        ket::apply_swap(circuit, i_left, i_right);

        ++i_left_pre;
        --i_right_pre;
    }
}
template void apply_fourier_transform_swaps_<ket::QubitIndicesVector>(
    ket::QuantumCircuit& circuit, const ket::QubitIndicesVector& container
);
template void apply_fourier_transform_swaps_<ket::QubitIndicesIList>(
    ket::QuantumCircuit& circuit, const ket::QubitIndicesIList& container
);

}  // namespace


namespace ket
{

template <QubitIndices Container>
void apply_forward_fourier_transform(QuantumCircuit& circuit, const Container& container)
{
    // TODO: maybe replace with get_container_size?
    const auto size = static_cast<std::size_t>(std::distance(container.begin(), container.end()));

    // perform the combination of Hadamard gates and controlled RZ gates
    for (std::size_t i_target_pre {0}; i_target_pre < size; ++i_target_pre) {
        const auto i_target = ket::internal::get_container_index(container, i_target_pre);
        circuit.add_h_gate(i_target);

        auto i_angle_denom = std::size_t {2};
        for (std::size_t i_control_pre {i_target_pre + 1}; i_control_pre < size; ++i_control_pre) {
            const auto i_control = ket::internal::get_container_index(container, i_control_pre);
            const auto angle = 2.0 * M_PI / static_cast<double>(ket::internal::pow_2_int(i_angle_denom));
            circuit.add_cp_gate(i_control, i_target, angle);
            ++i_angle_denom;
        }
    }

    // apply the swaps
    apply_fourier_transform_swaps_(circuit, container);
}
template void apply_forward_fourier_transform<ket::QubitIndicesVector>(
    ket::QuantumCircuit& circuit, const ket::QubitIndicesVector& container
);
template void apply_forward_fourier_transform<ket::QubitIndicesIList>(
    ket::QuantumCircuit& circuit, const ket::QubitIndicesIList& container
);

template <QubitIndices Container>
void apply_inverse_fourier_transform(QuantumCircuit& circuit, const Container& container)
{
    namespace sv = std::views;

    // apply the swaps
    apply_fourier_transform_swaps_(circuit, container);

    const auto size = static_cast<std::size_t>(std::distance(container.begin(), container.end()));

    for (std::size_t i_target_pre : sv::iota(0UL, size) | sv::reverse) {
        const auto i_target = ket::internal::get_container_index(container, i_target_pre);

        auto i_angle_denom = size - i_target_pre;
        for (std::size_t i_control_pre : sv::iota(i_target_pre + 1, size) | sv::reverse) {
            const auto i_control = ket::internal::get_container_index(container, i_control_pre);
            const auto angle = 2.0 * M_PI / static_cast<double>(ket::internal::pow_2_int(i_angle_denom));
            circuit.add_cp_gate(i_control, i_target, -angle);
            --i_angle_denom;
        }

        circuit.add_h_gate(i_target);
    }
}
template void apply_inverse_fourier_transform<ket::QubitIndicesVector>(
    ket::QuantumCircuit& circuit, const ket::QubitIndicesVector& container
);
template void apply_inverse_fourier_transform<ket::QubitIndicesIList>(
    ket::QuantumCircuit& circuit, const ket::QubitIndicesIList& container
);

}  // namespace ket
