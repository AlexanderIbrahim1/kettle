#pragma once

#include <optional>

#include <Eigen/Dense>

#include "kettle/circuit/classical_register.hpp"
#include "kettle/common/matrix2x2.hpp"
#include "kettle/gates/common_u_gates.hpp"
#include "kettle/operator/pauli/sparse_pauli_string.hpp"
#include "kettle/parameter/parameter.hpp"
#include "kettle/state/density_matrix.hpp"
#include "kettle/operator/channels/mixed_unitary_channel.hpp"
#include "kettle/operator/channels/multi_qubit_kraus_channel.hpp"
#include "kettle/operator/channels/one_qubit_kraus_channel.hpp"
#include "kettle/operator/channels/pauli_channel.hpp"

#include "kettle_internal/parameter/parameter_expression_internal.hpp"
#include "kettle_internal/simulation/simulate_utils.hpp"
#include "kettle_internal/simulation/gate_pair_generator.hpp"
#include "kettle_internal/simulation/operations_density_matrix.hpp"

/*
    This file contains code for applying a quantum channel to a density matrix, thus evolving
    it into another density matrix.
*/

namespace ket
{

// NOLINTNEXTLINE(cert-err58-cpp)
const auto MAP_PAULI_TERM_TO_PAULI_MATRIX2X2 = std::unordered_map<PauliTerm, Matrix2X2> {
    {PauliTerm::X, x_gate()},
    {PauliTerm::Y, y_gate()},
    {PauliTerm::Z, z_gate()}
};

inline void simulate_one_qubit_kraus_channel(
    DensityMatrix& state,
    const OneQubitKrausChannel& channel,
    const internal::FlatIndexPair<Eigen::Index>& pair,
    Eigen::MatrixXcd& writing_buffer,
    Eigen::MatrixXcd& left_mul_buffer,
    Eigen::MatrixXcd& right_mul_buffer
)
{
    const auto target_index = static_cast<Eigen::Index>(channel.target_index());
    const auto n_qubits = static_cast<Eigen::Index>(state.n_qubits());
    auto pair_iterator_outer = ket::internal::SingleQubitGatePairGenerator {target_index, n_qubits};
    auto pair_iterator_inner = ket::internal::SingleQubitGatePairGenerator {target_index, n_qubits};

    const auto n_kraus_matrices = channel.matrices().size();

    // function reference to reduce line length
    const auto& left_mul = internal::apply_left_one_qubit_matrix_;
    const auto& right_mul = internal::apply_right_one_qubit_matrix_;

    for (std::size_t i {0}; i < n_kraus_matrices; ++i) {
        const auto mat = channel.matrices()[i];
        const auto mat_adj = ket::conjugate_transpose(mat);

        left_mul(state.matrix(), left_mul_buffer, pair_iterator_outer, pair_iterator_inner, pair, mat);
        right_mul(left_mul_buffer, right_mul_buffer, pair_iterator_outer, pair_iterator_inner, pair, mat_adj);

        // skip setting all elements in the buffer to 0, by overwriting on the first iteration
        if (i == 0) {
            writing_buffer = right_mul_buffer;
        } else {
            writing_buffer += right_mul_buffer;
        }
    }

    state.matrix() = writing_buffer;
}


inline void simulate_multi_qubit_kraus_channel(
    DensityMatrix& state,
    const MultiQubitKrausChannel& channel,
    Eigen::MatrixXcd& writing_buffer
)
{
    const auto n_kraus_matrices = channel.matrices().size();

    for (std::size_t i {0}; i < n_kraus_matrices; ++i) {
        const auto& matrix = channel.matrices()[i];
        if (i == 0) {
            writing_buffer = matrix * state.matrix() * matrix.adjoint();
        } else {
            writing_buffer += matrix * state.matrix() * matrix.adjoint();
        }
    }

    state.matrix() = writing_buffer;
}


inline void apply_pauli_string_(
    Eigen::Index n_qubits,
    const SparsePauliString& pauli_string,
    const internal::FlatIndexPair<Eigen::Index>& pair,
    Eigen::MatrixXcd& multiplication_buffer,
    Eigen::MatrixXcd& state_buffer
)
{
    for (const auto& [target_index, pauli_term] : pauli_string.terms()) {
        if (pauli_term == PauliTerm::I) {
            continue;
        }

        const auto target_qubit = static_cast<Eigen::Index>(target_index);
        const auto gate = MAP_PAULI_TERM_TO_PAULI_MATRIX2X2.at(pauli_term);

        simulate_u_gate_(state_buffer, multiplication_buffer, target_qubit, n_qubits, gate, pair);
    }
}


inline void simulate_pauli_channel(
    DensityMatrix& state,
    const PauliChannel& channel,
    const internal::FlatIndexPair<Eigen::Index>& pair,
    Eigen::MatrixXcd& accumulation_buffer,
    Eigen::MatrixXcd& multiplication_buffer,
    Eigen::MatrixXcd& state_buffer
)
{
    const auto n_qubits = static_cast<Eigen::Index>(state.n_qubits());

    for (std::size_t i {0}; i < channel.size(); ++i) {
        const auto& [coefficient, pauli_string] = channel.weighted_unitaries()[i];

        state_buffer = state.matrix();
        apply_pauli_string_(n_qubits, pauli_string, pair, multiplication_buffer, state_buffer);

        // skip setting all elements in the buffer to 0, by overwriting on the first iteration
        if (i == 0) {
            accumulation_buffer = (coefficient * state_buffer);
        } else {
            accumulation_buffer += (coefficient * state_buffer);
        }
    }

    state.matrix() = accumulation_buffer;
}

inline void simulate_mixed_unitary_channel(
    DensityMatrix& state,
    const MixedUnitaryChannel& channel,
    const internal::FlatIndexPair<Eigen::Index>& single_pair,
    const internal::FlatIndexPair<Eigen::Index>& double_pair,
    Eigen::MatrixXcd& accumulation_buffer,
    Eigen::MatrixXcd& multiplication_buffer,
    Eigen::MatrixXcd& state_buffer,
    const std::optional<ket::param::EvaluatedParameterDataMap>& param_map = std::nullopt
)
{
    const auto n_qubits = state.n_qubits();

    const auto dummy_thread_id = std::size_t {0};
    const auto dummy_prng_seed = std::size_t {0};
    auto dummy_classical_register = ket::ClassicalRegister {n_qubits};

    auto eval_param_map = param_map.value_or(ket::param::EvaluatedParameterDataMap {});

    state_buffer = state.matrix();

    for (std::size_t i {0}; i < channel.size(); ++i) {
        const auto& [coefficient, unitary] = channel.weighted_unitaries()[i];

        if (i != 0) {
            state.matrix() = state_buffer;
        }

        for (const auto& circ_element : unitary) {
            if (!circ_element.is_gate()) {
                throw std::runtime_error {"ERROR: the MixedUnitaryChannel only supports gate simulation.\n"};
            }

            const auto& gate_info = circ_element.get_gate();

            // TODO: maybe make multithreaded later when this all works
            simulate_gate_info_(
                eval_param_map,
                state,
                single_pair,
                double_pair,
                gate_info,
                dummy_thread_id,
                dummy_prng_seed,
                dummy_classical_register,
                multiplication_buffer
            );
        }

        // skip setting all elements in the buffer to 0, by overwriting on the first iteration
        if (i == 0) {
            accumulation_buffer = (coefficient * state.matrix());
        } else {
            accumulation_buffer += (coefficient * state.matrix());
        }
    }

    state.matrix() = accumulation_buffer;
}

}  // namespace ket
