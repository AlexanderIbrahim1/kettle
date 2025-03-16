#pragma once

#include <algorithm>
#include <cstddef>
#include <vector>

#include "mini-qiskit/common/matrix2x2.hpp"


namespace mqis
{

enum class DecomposedGateState {
    SINGLEGATE = 0,
    ALLCONTROL = 1
};

struct DecomposedGateInfo
{
    DecomposedGateState state;
    std::size_t qubit_index;
    Matrix2X2 matrix;
};

inline auto decomposed_circuit_size(const std::vector<DecomposedGateInfo>& gates) -> std::size_t
{
    auto max_qubit_index_seen = std::size_t {0};

    for (const auto& gate : gates) {
        if (gate.qubit_index > max_qubit_index_seen) {
            max_qubit_index_seen = gate.qubit_index;
        }
    }

    // add 1 due to the 0-based index offset
    const auto circuit_size = max_qubit_index_seen + 1;

    return circuit_size;
}

}  // namespace mqis
