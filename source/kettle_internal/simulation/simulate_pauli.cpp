#include <stdexcept>
#include <type_traits>

#include "kettle/operator/pauli/sparse_pauli_string.hpp"
#include "kettle/simulation/simulate_pauli.hpp"
#include "kettle/state/statevector.hpp"

#include "kettle_internal/simulation/gate_pair_generator.hpp"
#include "kettle_internal/simulation/simulate_utils.hpp"
#include "kettle_internal/simulation/operations.hpp"


namespace ki = ket::internal;

namespace
{

/*
    Helper struct for the static_assert(), to see what ket::PauliTerm instance is passed that causes
    the template instantiation to fail.
*/
template <ket::PauliTerm Pauli>
struct pauli_always_false : std::false_type
{};


template <ket::PauliTerm Pauli>
void simulate_pauli_gate_(
    ket::Statevector& state,
    std::size_t target_index,
    const ki::FlatIndexPair<std::size_t>& pair
)
{
    const auto n_qubits = state.n_qubits();

    auto pair_iterator = ki::SingleQubitGatePairGenerator {target_index, n_qubits};
    pair_iterator.set_state(pair.i_lower);

    for (std::size_t i {pair.i_lower}; i < pair.i_upper; ++i) {
        const auto [state0_index, state1_index] = pair_iterator.next();

        if constexpr (Pauli == ket::PauliTerm::X) {
            ki::apply_x_gate(state, state0_index, state1_index);
        }
        else if constexpr (Pauli == ket::PauliTerm::Y) {
            ki::apply_y_gate(state, state0_index, state1_index);
        }
        else if constexpr (Pauli == ket::PauliTerm::Z) {
            ki::apply_z_gate(state, state1_index);
        }
        else {
            static_assert(pauli_always_false<Pauli>::value, "Invalid Pauli term.");
        }
    }
}


void simulate_pauli_gates_(
    ket::Statevector& state,
    const ki::FlatIndexPair<std::size_t>& single_pair,
    const ket::SparsePauliString& pauli_string
)
{
    using PT = ket::PauliTerm;

    for (const auto& [target_index, pauli_term] : pauli_string.terms())
    {
        switch(pauli_term) {
            case PT::X : {
                simulate_pauli_gate_<PT::X>(state, target_index, single_pair);
                break;
            }
            case PT::Y : {
                simulate_pauli_gate_<PT::Y>(state, target_index, single_pair);
                break;
            }
            case PT::Z : {
                simulate_pauli_gate_<PT::Z>(state, target_index, single_pair);
                break;
            }
            default : {
                continue;
            }
        }
    }
}

void check_valid_number_of_qubits_(const ket::SparsePauliString& pauli_string, const ket::Statevector& state)
{
    if (pauli_string.n_qubits() != state.n_qubits()) {
        throw std::runtime_error {"Invalid simulation; SparsePauliString and state have different number of qubits."};
    }

    if (pauli_string.n_qubits() == 0) {
        throw std::runtime_error {"Cannot simulate a SparsePauliString or state with zero qubits."};
    }
}

}  // namespace

namespace ket
{

void StatevectorPauliStringSimulator::run(const SparsePauliString& pauli_string, Statevector& state)
{
    namespace ki = ket::internal;

    check_valid_number_of_qubits_(pauli_string, state);

    const auto n_single_gate_pairs = ki::number_of_single_qubit_gate_pairs_(pauli_string.n_qubits());
    const auto single_pair = ki::FlatIndexPair<std::size_t> {.i_lower=0, .i_upper=n_single_gate_pairs};

    simulate_pauli_gates_(state, single_pair, pauli_string);

    has_been_run_ = true;
}

[[nodiscard]]
auto StatevectorPauliStringSimulator::has_been_run() const -> bool
{
    return has_been_run_;
}

void simulate(const SparsePauliString& pauli_string, Statevector& state)
{
    auto simulator = StatevectorPauliStringSimulator {};
    simulator.run(pauli_string, state);
}


}  // namespace ket
