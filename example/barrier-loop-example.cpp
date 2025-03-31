#include <barrier>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

/*
This is an example of using `std::barrier` for synchronization inside of a loop
*/

template <typename Barrier>
void work_and_leave(Barrier& barrier, std::mutex& mutex, int thread_id)
{
    for (int i {0}; i < 3; ++i) {
        {
            const auto lock = std::lock_guard {mutex};
            std::cout << "BEFORE BARRIER : " << thread_id << '\n';
        }

        barrier.arrive_and_wait();

        {
            const auto lock = std::lock_guard {mutex};
            std::cout << "AFTER BARRIER  : " << thread_id << '\n';
        }
    }
}

template <typename Barrier>
void simulate_loop_(
    Barrier& sync_point,
    std::mutex& cout_mutex,
    const std::vector<int>& elements,
    int thread_id
)
{
    int count = 0;
    for (const auto& elem : elements) {
        std::cout << thread_id << " entering loop body: " << count << '\n';
        // simulate_loop_body_(circuit, state, single_gate_pair, double_gate_pair, gate);
        std::cout << thread_id << " leaving loop body : " << count << '\n';
        ++count;
        sync_point.arrive_and_wait();
        std::cout << thread_id << " after sync point  : " << count << '\n';
    }
}


auto main() -> int
{
    auto mutex = std::mutex {};

    const auto n_threads = std::size_t {5};
    auto barrier = std::barrier {static_cast<std::ptrdiff_t>(n_threads)};

    const auto elements = std::vector<int> {1, 2, 3, 4, 5};

    auto threads = std::vector<std::jthread> {};
    for (int i {0}; i < n_threads; ++i) {
        threads.emplace_back(simulate_loop_<decltype(barrier)>, std::ref(barrier), std::ref(mutex), std::ref(elements), i);
    }

//    auto t0 = std::jthread {work_and_leave<decltype(barrier)>, std::ref(barrier), std::ref(mutex), 0};
//    auto t1 = std::jthread {work_and_leave<decltype(barrier)>, std::ref(barrier), std::ref(mutex), 1};
//    auto t2 = std::jthread {work_and_leave<decltype(barrier)>, std::ref(barrier), std::ref(mutex), 2};
//
    return 0;
}
