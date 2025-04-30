#pragma once

#include <stdexcept>

#include "kettle/circuit/circuit_element.hpp"
#include "kettle/circuit/classical_register.hpp"
#include "kettle/common/clone_ptr.hpp"
#include "kettle/gates/primitive_gate.hpp"
#include "kettle/simulation/simulate.hpp"

/*
    PLAN:
    - it will not take an instanceof the `QuantumCircuit` or `QuantumState` upon construction
      - the simulation can take them with a member function
*/

namespace impl_ket
{

inline void simulate_gate_info_(
    const ket::QuantumCircuit& circuit,
    ket::QuantumState& state,
    const FlatIndexPair& single_pair,
    const FlatIndexPair& double_pair,
    const ket::GateInfo& gate_info,
    int thread_id,
    std::optional<int> prng_seed,
    ket::ClassicalRegister& c_register
)
{
    // TODO: remove the need for passing the number of qubits
    // - that information is stored in the state itself
    // - and it will mean that we don't need to pass the circuit
    using G = ket::Gate;

    switch (gate_info.gate) {
        case G::H : {
            simulate_single_qubit_gate_<G::H>(state, gate_info, circuit.n_qubits(), single_pair);
            break;
        }
        case G::X : {
            simulate_single_qubit_gate_<G::X>(state, gate_info, circuit.n_qubits(), single_pair);
            break;
        }
        case G::Y : {
            simulate_single_qubit_gate_<G::Y>(state, gate_info, circuit.n_qubits(), single_pair);
            break;
        }
        case G::Z : {
            simulate_single_qubit_gate_<G::Z>(state, gate_info, circuit.n_qubits(), single_pair);
            break;
        }
        case G::SX : {
            simulate_single_qubit_gate_<G::SX>(state, gate_info, circuit.n_qubits(), single_pair);
            break;
        }
        case G::RX : {
            simulate_single_qubit_gate_<G::RX>(state, gate_info, circuit.n_qubits(), single_pair);
            break;
        }
        case G::RY : {
            simulate_single_qubit_gate_<G::RY>(state, gate_info, circuit.n_qubits(), single_pair);
            break;
        }
        case G::RZ : {
            simulate_single_qubit_gate_<G::RZ>(state, gate_info, circuit.n_qubits(), single_pair);
            break;
        }
        case G::P : {
            simulate_single_qubit_gate_<G::P>(state, gate_info, circuit.n_qubits(), single_pair);
            break;
        }
        case G::CH : {
            simulate_double_qubit_gate_<G::CH>(state, gate_info, circuit.n_qubits(), double_pair);
            break;
        }
        case G::CX : {
            simulate_double_qubit_gate_<G::CX>(state, gate_info, circuit.n_qubits(), double_pair);
            break;
        }
        case G::CY : {
            simulate_double_qubit_gate_<G::CY>(state, gate_info, circuit.n_qubits(), double_pair);
            break;
        }
        case G::CZ : {
            simulate_double_qubit_gate_<G::CZ>(state, gate_info, circuit.n_qubits(), double_pair);
            break;
        }
        case G::CSX : {
            simulate_double_qubit_gate_<G::CSX>(state, gate_info, circuit.n_qubits(), double_pair);
            break;
        }
        case G::CRX : {
            simulate_double_qubit_gate_<G::CRX>(state, gate_info, circuit.n_qubits(), double_pair);
            break;
        }
        case G::CRY : {
            simulate_double_qubit_gate_<G::CRY>(state, gate_info, circuit.n_qubits(), double_pair);
            break;
        }
        case G::CRZ : {
            simulate_double_qubit_gate_<G::CRZ>(state, gate_info, circuit.n_qubits(), double_pair);
            break;
        }
        case G::CP : {
            simulate_double_qubit_gate_<G::CP>(state, gate_info, circuit.n_qubits(), double_pair);
            break;
        }
        case G::U : {
            const auto& unitary_ptr = unpack_unitary_matrix(gate_info);
            simulate_single_qubit_gate_general_(state, gate_info, circuit.n_qubits(), *unitary_ptr, single_pair);
            break;
        }
        case G::CU : {
            const auto& unitary_ptr = unpack_unitary_matrix(gate_info);
            simulate_double_qubit_gate_general_(state, gate_info, circuit.n_qubits(), *unitary_ptr, double_pair);
            break;
        }
        case G::M : {
            // this operation is more complicated to make multithreaded because the threads have already been
            // spawned before entering the simulation loop; thus, it is easier to just make the measurement
            // a single-threaded operation
            if (thread_id == MEASURING_THREAD_ID) {
                [[maybe_unused]]
                const auto [ignore, bit_index] = unpack_m_gate(gate_info);
                const auto measured = simulate_measurement_(state, gate_info, circuit.n_qubits(), prng_seed);
                c_register.set(bit_index, measured);
            }
            break;
        }
    }
}


inline void simulate_loop_body_iterative_(
    const ket::QuantumCircuit& circuit,
    ket::QuantumState& state,
    const FlatIndexPair& single_pair,
    const FlatIndexPair& double_pair,
    int thread_id,
    std::optional<int> prng_seed,
    ket::ClassicalRegister& c_register
)
{
    using Elements = std::reference_wrapper<const std::vector<impl_ket::CircuitElement>>;

    auto elements_stack = std::vector<Elements> {};
    elements_stack.push_back(std::ref(circuit.circuit_elements()));

    auto instruction_pointers = std::vector<std::size_t> {};
    instruction_pointers.push_back(0);

    while (elements_stack.size() != 0) {
        const auto& elements = elements_stack.back();
        const auto i_ptr = instruction_pointers.back();

        ++instruction_pointers.back();

        if (i_ptr >= elements.get().size()) {
            elements_stack.pop_back();
            instruction_pointers.pop_back();
            continue;
        }

        const auto& element = elements.get()[i_ptr];

        if (element.is_control_flow()) {
            const auto& control_flow = element.get_control_flow();

            if (control_flow.is_if_statement()) {
                const auto& if_stmt = control_flow.get_if_statement();

                if (if_stmt(c_register)) {
                    const auto& subcircuit = *if_stmt.circuit();
                    elements_stack.push_back(std::ref(subcircuit.circuit_elements()));
                    instruction_pointers.push_back(0);
                }
            }
            else if (control_flow.is_if_else_statement()) {
                const auto& if_else_stmt = control_flow.get_if_else_statement();

                const auto& subcircuit = [&]() {
                    if (if_else_stmt(c_register)) {
                        return *if_else_stmt.if_circuit();
                    } else {
                        return *if_else_stmt.else_circuit();
                    }
                }();

                elements_stack.push_back(std::ref(subcircuit.circuit_elements()));
                instruction_pointers.push_back(0);
            }
            else {
                throw std::runtime_error {"DEV ERROR: unimplemented control flow function found\n"};
            }
        }
        else if (element.is_gate()) {
            const auto& gate_info = element.get_gate();

            simulate_gate_info_(
                circuit,
                state,
                single_pair,
                double_pair,
                gate_info,
                thread_id,
                prng_seed,
                c_register
            );
        }
        else {
            throw std::runtime_error {"DEV ERROR: unimplemented circuit element found\n"};
        }
    }
}

}  // namespace impl_ket


namespace ket
{

class StatevectorSimulator
{
public:
    void run(const QuantumCircuit& circuit, QuantumState& state, std::optional<int> prng_seed = std::nullopt)
    {
        namespace im = impl_ket;

        im::check_valid_number_of_qubits_(circuit, state);

        const auto n_single_gate_pairs = im::number_of_single_qubit_gate_pairs_(circuit.n_qubits());
        const auto single_pair = im::FlatIndexPair {.i_lower=0, .i_upper=n_single_gate_pairs};

        const auto n_double_gate_pairs = im::number_of_double_qubit_gate_pairs_(circuit.n_qubits());
        const auto double_pair = im::FlatIndexPair {.i_lower=0, .i_upper=n_double_gate_pairs};

        cregister_ = im::ClonePtr<ClassicalRegister> {ClassicalRegister {circuit.n_bits()}};

        // the `simulate_loop_body_()` function is used by both the single-threaded and multi-threaded
        // code, and certain operations are only done on the thread with thread id 0
        const auto thread_id = impl_ket::MEASURING_THREAD_ID;

        impl_ket::simulate_loop_body_iterative_(circuit, state, single_pair, double_pair, thread_id, prng_seed, *cregister_);

        has_been_run_ = true;
    }

    [[nodiscard]]
    constexpr auto has_been_run() const -> bool
    {
        return has_been_run_;
    }

    [[nodiscard]]
    auto classical_register() const -> const ClassicalRegister&
    {
        if (!cregister_) {
            throw std::runtime_error {"ERROR: Cannot access classical register; no simulation has been run\n"};
        }

        return *cregister_;
    }

    auto classical_register() -> ClassicalRegister&
    {
        if (!cregister_) {
            throw std::runtime_error {"ERROR: Cannot access classical register; no simulation has been run\n"};
        }

        return *cregister_;
    }

private:
    // there is no default constructor for the ClassicalRegsiter (it wouldn't make sense), and we
    // only find out how many bits are needed after the first simulation; hence why we use a pointer
    impl_ket::ClonePtr<ClassicalRegister> cregister_ {nullptr};
    bool has_been_run_ {false};
};

}  // namespace ket
