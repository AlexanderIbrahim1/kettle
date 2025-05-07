#pragma once

#include <optional>
#include <vector>

#include "kettle/circuit/classical_register.hpp"
#include "kettle/circuit/circuit.hpp"
#include "kettle/circuit_loggers/circuit_logger.hpp"
#include "kettle/common/clone_ptr.hpp"
#include "kettle/state/state.hpp"


namespace ket
{

class StatevectorSimulator
{
public:
    void run(const QuantumCircuit& circuit, QuantumState& state, std::optional<int> prng_seed = std::nullopt);

    [[nodiscard]]
    auto has_been_run() const -> bool;

    [[nodiscard]]
    auto classical_register() const -> const ClassicalRegister&;

    auto classical_register() -> ClassicalRegister&;

    [[nodiscard]]
    auto circuit_loggers() const -> const std::vector<CircuitLogger>&;

private:
    // there is no default constructor for the ClassicalRegsiter (it wouldn't make sense), and we
    // only find out how many bits are needed after the first simulation; hence why we use a pointer
    ket::ClonePtr<ClassicalRegister> cregister_ {nullptr};
    bool has_been_run_ {false};
    std::vector<CircuitLogger> circuit_loggers_;
};


void simulate(const QuantumCircuit& circuit, QuantumState& state, std::optional<int> prng_seed = std::nullopt);

}  // namespace ket


// inline void simulate_multithreaded_loop_(
//     std::barrier<>& sync_point,
//     const ket::QuantumCircuit& circuit,
//     ket::QuantumState& state,
//     const FlatIndexPair& single_pair,
//     const FlatIndexPair& double_pair,
//     int thread_id,
//     std::optional<int> prng_seed,
//     ket::ClassicalRegister& c_register
// )
// {
//     for (const auto& element : circuit) {
//         simulate_loop_body_(state, single_pair, double_pair, element, thread_id, prng_seed, c_register);
//         sync_point.arrive_and_wait();
//     }
// }
// /*
//     WARNING: the current multithreaded implementation is slower than the singlethreaded implementation;
//     I'm not sure of the reasons yet (too much waiting at the barrier, multiple states per cache line, etc.)
// 
//     A quick benchmark shows that the threads spend a large amount of time waiting.
// */
// inline void simulate_multithreaded(
//     const QuantumCircuit& circuit,
//     QuantumState& state,
//     std::size_t n_threads,
//     std::optional<int> prng_seed = std::nullopt
// )
// {
//     if (n_threads == 0) {
//         throw std::runtime_error {"Cannot perform simulation with 0 threads.\n"};
//     }
// 
//     im::check_valid_number_of_qubits_(circuit, state);
// 
//     const auto n_single_gate_pairs = im::number_of_single_qubit_gate_pairs_(circuit.n_qubits());
//     const auto single_flat_index_pairs = im::partial_sum_pairs_(n_single_gate_pairs, n_threads);
// 
//     const auto n_double_gate_pairs = im::number_of_double_qubit_gate_pairs_(circuit.n_qubits());
//     const auto double_flat_index_pairs = im::partial_sum_pairs_(n_double_gate_pairs, n_threads);
// 
//     auto c_register = ClassicalRegister {circuit.n_bits()};
// 
//     auto threads = std::vector<std::jthread> {};
//     threads.reserve(n_threads);
// 
//     auto barrier = std::barrier {static_cast<std::ptrdiff_t>(n_threads)};
// 
//     for (std::size_t i {0}; i < n_threads; ++i) {
//         threads.emplace_back(
//             im::simulate_multithreaded_loop_,
//             std::ref(barrier),
//             std::ref(circuit),
//             std::ref(state),
//             single_flat_index_pairs[i],
//             double_flat_index_pairs[i],
//             i,
//             prng_seed,
//             std::ref(c_register)
//         );
//     }
// 
//     for (auto& thread : threads) {
//         thread.join();
//     }
// }
