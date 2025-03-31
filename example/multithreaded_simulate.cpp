#include <iostream>

#include <mini-qiskit/mini-qiskit.hpp>

/*
    This file shows an example of performing a multithreaded simulation.
*/

template <typename Barrier>
void simulate_loop_(
    Barrier& sync_point,
    const mqis::QuantumCircuit& circuit,
    mqis::QuantumState& state,
    const impl_mqis::FlatIndexPair& single_gate_pair,
    const impl_mqis::FlatIndexPair& double_gate_pair,
    std::mutex& cout_mutex,
    int thread_id
)
{
    {
        const auto lock = std::lock_guard {cout_mutex};

        std::cout << "single_gate_pair for thread " << thread_id << '\n';
        std::cout << '(' << single_gate_pair.i_lower << ", " << single_gate_pair.i_upper << ")\n";
        std::cout << "double_gate_pair for thread " << thread_id << '\n';
        std::cout << '(' << double_gate_pair.i_lower << ", " << double_gate_pair.i_upper << ")\n";
    }

    int count = 0;
    for (const auto& gate : circuit) {
        std::cout << thread_id << " entering loop body: " << count << '\n';
        impl_mqis::simulate_loop_body_(circuit, state, single_gate_pair, double_gate_pair, gate);
        std::cout << thread_id << " leaving loop body : " << count << '\n';
        ++count;
        sync_point.arrive_and_wait();
        std::cout << thread_id << " after sync point  : " << count << '\n';
    }
}


auto main() -> int
{
    auto circuit = mqis::QuantumCircuit {4};
    circuit.add_h_gate({0, 1, 2, 3});
    circuit.add_cx_gate({{0, 1}, {0, 2}});
    // circuit.add_x_gate({0, 1, 2, 3});
    circuit.add_m_gate({0, 1, 2, 3});

    auto statevector = mqis::QuantumState {"0000"};

    mqis::simulate_multithreaded(circuit, statevector, 2);

//     const auto single_gate_pairs = std::vector<impl_mqis::FlatIndexPair> {{0, 4}, {4, 8}};
//     const auto double_gate_pairs = std::vector<impl_mqis::FlatIndexPair> {{0, 2}, {2, 4}};
// 
//     const auto n_threads = std::size_t {2};
//     auto barrier = std::barrier {static_cast<std::ptrdiff_t>(n_threads)};
//     auto cout_mutex = std::mutex {};
// 
//     auto threads = std::vector<std::jthread> {};
//     for (std::size_t i {0}; i < n_threads; ++i) {
//         threads.emplace_back(
//             simulate_loop_<decltype(barrier)>,
//             std::ref(barrier),
//             std::ref(circuit),
//             std::ref(statevector),
//             single_gate_pairs[i],
//             double_gate_pairs[i],
//             std::ref(cout_mutex),
//             i
//         );
//     }
// 
//     for (auto& thread : threads) {
//         thread.join();
//     }

    const auto counts = mqis::perform_measurements_as_counts_marginal(circuit, statevector, 1024);

    for (const auto& [bitstring, count] : counts) {
        std::cout << "(state, count) = (" << bitstring << ", " << count << ")\n";
    }

    return 0;
}
