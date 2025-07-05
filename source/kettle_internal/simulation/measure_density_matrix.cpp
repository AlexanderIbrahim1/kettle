#include <type_traits>

#include "kettle/gates/primitive_gate.hpp"
#include "kettle/state/density_matrix.hpp"

#include "kettle_internal/gates/primitive_gate/gate_create.hpp"
#include "kettle_internal/simulation/gate_pair_generator.hpp"
#include "kettle_internal/simulation/measure_density_matrix.hpp"

namespace ket::internal
{

/*
    Helper struct for the static_assert(), to see what int instance is passed that causes
    the template instantiation to fail.
*/
template <int StateToCollapse>
struct state_collapse_always_false : std::false_type
{};

auto probabilities_of_collapsed_states_(
    ket::DensityMatrix& state,
    const ket::GateInfo& info
) -> std::tuple<double, double>
{
    const auto target_index_st = ket::internal::create::unpack_single_qubit_gate_index(info);
    const auto target_index = static_cast<Eigen::Index>(target_index_st);
    const auto n_qubits = static_cast<Eigen::Index>(state.n_qubits());

    auto pair_iterator_outer = SingleQubitGatePairGenerator {target_index, n_qubits};
    auto pair_iterator_inner = SingleQubitGatePairGenerator {target_index, n_qubits};

    auto prob_of_0_states = double {0.0};
    auto prob_of_1_states = double {0.0};
   
    // TODO: change the initial and final pairs when multithreading is reintroduced
    pair_iterator_outer.set_state(0);
    for (auto i_pair_outer {0}; i_pair_outer < pair_iterator_outer.size(); ++i_pair_outer) {
        const auto [i_outer0, i_outer1] = pair_iterator_outer.next();

        pair_iterator_inner.set_state(0);
        for (auto i_pair_inner {0}; i_pair_inner < pair_iterator_outer.size(); ++i_pair_inner) {
            const auto [i_inner0, i_inner1] = pair_iterator_inner.next();

            prob_of_0_states += std::norm(state.matrix()(i_inner0, i_outer0));
            prob_of_1_states += std::norm(state.matrix()(i_inner1, i_outer1));
        }
    }

    return {prob_of_0_states, prob_of_1_states};
}

template <int StateToCollapse>
void collapse_and_renormalize_(
    ket::DensityMatrix& state,
    const ket::GateInfo& info,
    double norm_of_surviving_state
)
{
    const auto target_index_st = ket::internal::create::unpack_single_qubit_gate_index(info);
    const auto target_index = static_cast<Eigen::Index>(target_index_st);
    const auto n_qubits = static_cast<Eigen::Index>(state.n_qubits());

    auto pair_iterator_outer = SingleQubitGatePairGenerator {target_index, n_qubits};
    auto pair_iterator_inner = SingleQubitGatePairGenerator {target_index, n_qubits};

    // TODO: change the initial and final pairs when multithreading is reintroduced
    pair_iterator_outer.set_state(0);
    for (auto i_pair_outer {0}; i_pair_outer < pair_iterator_outer.size(); ++i_pair_outer) {
        const auto [i_outer0, i_outer1] = pair_iterator_outer.next();

        pair_iterator_inner.set_state(0);
        for (auto i_pair_inner {0}; i_pair_inner < pair_iterator_outer.size(); ++i_pair_inner) {
            const auto [i_inner0, i_inner1] = pair_iterator_inner.next();

            state.matrix()(i_inner1, i_outer0) = {0.0, 0.0};
            state.matrix()(i_inner0, i_outer1) = {0.0, 0.0};

            if constexpr (StateToCollapse == 0) {
                state.matrix()(i_inner0, i_outer0) = {0.0, 0.0};
                state.matrix()(i_inner1, i_outer1) *= norm_of_surviving_state;
            }
            else if constexpr (StateToCollapse == 1) {
                state.matrix()(i_inner0, i_outer0) *= norm_of_surviving_state;
                state.matrix()(i_inner1, i_outer1) = {0.0, 0.0};
            }
            else {
                static_assert(
                    state_collapse_always_false<StateToCollapse>::value,
                    "Invalid integer provided for state collapse of density matrix."
                );
            }
        }
    }
}

template
void collapse_and_renormalize_<0>(
    ket::DensityMatrix& state,
    const ket::GateInfo& info,
    double norm_of_surviving_state
);
template
void collapse_and_renormalize_<1>(
    ket::DensityMatrix& state,
    const ket::GateInfo& info,
    double norm_of_surviving_state
);

}  // namespace ket::internal
