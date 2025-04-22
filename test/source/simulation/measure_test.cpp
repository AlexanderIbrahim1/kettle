#include <complex>
#include <cstddef>
#include <functional>
#include <random>
#include <vector>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <kettle/common/print.hpp>
#include <kettle/circuit/circuit.hpp>
#include <kettle/state/state.hpp>
#include <kettle/simulation/simulate.hpp>
#include <kettle/simulation/measure.hpp>


template <int Output>
struct RiggedDiscreteDistribution
{
public:
    using result_type = int;

    RiggedDiscreteDistribution([[maybe_unused]] std::initializer_list<double> ignore)
    {}

    auto operator()([[maybe_unused]] std::mt19937& prng) -> int
    {
        return Output;
    }
};


auto create_random_complex(std::mt19937& prng) -> std::complex<double>
{
    auto uniform = std::uniform_real_distribution<double> {-1.0, 1.0};
    const auto real = uniform(prng);
    const auto imag = uniform(prng);

    return {real, imag};
}


void normalize(std::vector<std::complex<double>>& values)
{
    auto norm_sq = double {0.0};
    for (auto value : values) {
        norm_sq += std::norm(value);
    }

    const auto inv_norm = std::sqrt(1.0 / norm_sq);

    for (auto& value : values) {
        value *= inv_norm;
    }
}

void simulate_measurement_wrapper(
    ket::QuantumState& state,
    const ket::GateInfo& info,
    int measured_state
)
{
    const auto n_qubits = state.n_qubits();

    if (measured_state == 0) {
        impl_ket::simulate_measurement_<RiggedDiscreteDistribution<0>>(state, info, n_qubits);
    }
    else if (measured_state == 1) {
        impl_ket::simulate_measurement_<RiggedDiscreteDistribution<1>>(state, info, n_qubits);
    }
    else {
        throw std::runtime_error {"Invalid measured state provided to the test case.\n"};
    }
}

TEST_CASE("simulate_measurement_()")
{
    struct TestCase
    {
        std::size_t measured_qubit;
        int measured_state;
        ket::QuantumState expected;
    };

    SECTION("2 qubits; Hadamard on each")
    {
        auto testcase = GENERATE_REF(
            TestCase {
                .measured_qubit=0,
                .measured_state=0,
                .expected=ket::QuantumState {{{M_SQRT1_2, 0.0}, {0.0, 0.0}, {M_SQRT1_2, 0.0}, {0.0, 0.0}}}
            },
            TestCase {
                .measured_qubit=1,
                .measured_state=0,
                .expected=ket::QuantumState {{{M_SQRT1_2, 0.0}, {M_SQRT1_2, 0.0}, {0.0, 0.0}, {0.0, 0.0}}}
            },
            TestCase {
                .measured_qubit=0,
                .measured_state=1,
                .expected=ket::QuantumState {{{0.0, 0.0}, {M_SQRT1_2, 0.0}, {0.0, 0.0}, {M_SQRT1_2, 0.0}}}
            },
            TestCase {
                .measured_qubit=1,
                .measured_state=1,
                .expected=ket::QuantumState {{{0.0, 0.0}, {0.0, 0.0}, {M_SQRT1_2, 0.0}, {M_SQRT1_2, 0.0}}}
            }
        );

        // the measured bit doesn't matter for now
        const auto info = impl_ket::create_m_gate(testcase.measured_qubit, 0);

        auto state = ket::QuantumState {"00"};
        auto circuit = ket::QuantumCircuit {2};
        circuit.add_h_gate({0, 1});
        ket::simulate(circuit, state);

        simulate_measurement_wrapper(state, info, testcase.measured_state);

        REQUIRE(ket::almost_eq_with_print(state, testcase.expected));
    }

    SECTION("3 qubits")
    {
        auto testcase = GENERATE_REF(
            TestCase {
                .measured_qubit=0,
                .measured_state=0,
                .expected=ket::QuantumState {{
                    {0.5, 0.0}, {0.0, 0.0}, {0.5, 0.0}, {0.0, 0.0},
                    {0.5, 0.0}, {0.0, 0.0}, {0.5, 0.0}, {0.0, 0.0}
                }}
            },
            TestCase {
                .measured_qubit=0,
                .measured_state=1,
                .expected=ket::QuantumState {{
                    {0.0, 0.0}, {0.5, 0.0}, {0.0, 0.0}, {0.5, 0.0},
                    {0.0, 0.0}, {0.5, 0.0}, {0.0, 0.0}, {0.5, 0.0}
                }}
            },
            TestCase {
                .measured_qubit=1,
                .measured_state=0,
                .expected=ket::QuantumState {{
                    {0.5, 0.0}, {0.5, 0.0}, {0.0, 0.0}, {0.0, 0.0},
                    {0.5, 0.0}, {0.5, 0.0}, {0.0, 0.0}, {0.0, 0.0}
                }}
            },
            TestCase {
                .measured_qubit=1,
                .measured_state=1,
                .expected=ket::QuantumState {{
                    {0.0, 0.0}, {0.0, 0.0}, {0.5, 0.0}, {0.5, 0.0},
                    {0.0, 0.0}, {0.0, 0.0}, {0.5, 0.0}, {0.5, 0.0}
                }}
            },
            TestCase {
                .measured_qubit=2,
                .measured_state=0,
                .expected=ket::QuantumState {{
                    {0.5, 0.0}, {0.5, 0.0}, {0.5, 0.0}, {0.5, 0.0},
                    {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}
                }}
            },
            TestCase {
                .measured_qubit=2,
                .measured_state=1,
                .expected=ket::QuantumState {{
                    {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0},
                    {0.5, 0.0}, {0.5, 0.0}, {0.5, 0.0}, {0.5, 0.0}
                }}
            }
        );

        // the measured bit doesn't matter for now
        const auto info = impl_ket::create_m_gate(testcase.measured_qubit, 0);

        auto state = ket::QuantumState {"000"};
        auto circuit = ket::QuantumCircuit {3};
        circuit.add_h_gate({0, 1, 2});

        ket::simulate(circuit, state);

        simulate_measurement_wrapper(state, info, testcase.measured_state);

        REQUIRE(ket::almost_eq_with_print(state, testcase.expected));
    }

    SECTION("random")
    {
        struct RandomTestCase
        {
            std::size_t measured_qubit;
            int measured_state;
            std::vector<std::complex<double>> initial_amplitudes;
            std::vector<std::complex<double>> expected_amplitudes;
        };

        auto prng = std::mt19937 {std::random_device {}()};

        SECTION("2 qubits")
        {
            const auto coeff00 = create_random_complex(prng);
            const auto coeff10 = create_random_complex(prng);
            const auto coeff01 = create_random_complex(prng);
            const auto coeff11 = create_random_complex(prng);

            auto testcase = GENERATE_REF(
                RandomTestCase {
                    .measured_qubit=0,
                    .measured_state=0,
                    .initial_amplitudes={coeff00, coeff10, coeff01, coeff11},
                    .expected_amplitudes={coeff00, {0.0, 0.0}, coeff01, {0.0, 0.0}}
                },
                RandomTestCase {
                    .measured_qubit=1,
                    .measured_state=0,
                    .initial_amplitudes={coeff00, coeff10, coeff01, coeff11},
                    .expected_amplitudes={coeff00, coeff10, {0.0, 0.0}, {0.0, 0.0}},
                },
                RandomTestCase {
                    .measured_qubit=0,
                    .measured_state=1,
                    .initial_amplitudes={coeff00, coeff10, coeff01, coeff11},
                    .expected_amplitudes={{0.0, 0.0}, coeff10, {0.0, 0.0}, coeff11},
                },
                RandomTestCase {
                    .measured_qubit=1,
                    .measured_state=1,
                    .initial_amplitudes={coeff00, coeff10, coeff01, coeff11},
                    .expected_amplitudes={{0.0, 0.0}, {0.0, 0.0}, coeff01, coeff11},
                }
            );

            normalize(testcase.initial_amplitudes);
            normalize(testcase.expected_amplitudes);

            // the measured bit doesn't matter for now
            const auto info = impl_ket::create_m_gate(testcase.measured_qubit, 0);

            auto state = ket::QuantumState {testcase.initial_amplitudes};
            auto expected_state = ket::QuantumState {testcase.expected_amplitudes};

            simulate_measurement_wrapper(state, info, testcase.measured_state);

            REQUIRE(ket::almost_eq_with_print(state, expected_state));
        }

        SECTION("3 qubits")
        {
            const auto c000 = create_random_complex(prng);
            const auto c100 = create_random_complex(prng);
            const auto c010 = create_random_complex(prng);
            const auto c110 = create_random_complex(prng);
            const auto c001 = create_random_complex(prng);
            const auto c101 = create_random_complex(prng);
            const auto c011 = create_random_complex(prng);
            const auto c111 = create_random_complex(prng);

            auto testcase = GENERATE_REF(
                RandomTestCase {
                    .measured_qubit=0,
                    .measured_state=0,
                    .initial_amplitudes={c000, c100, c010, c110, c001, c101, c011, c111},
                    .expected_amplitudes={c000, 0.0, c010, 0.0, c001, 0.0, c011, 0.0}
                },
                RandomTestCase {
                    .measured_qubit=1,
                    .measured_state=0,
                    .initial_amplitudes={c000, c100, c010, c110, c001, c101, c011, c111},
                    .expected_amplitudes={c000, c100, 0.0, 0.0, c001, c101, 0.0, 0.0}
                },
                RandomTestCase {
                    .measured_qubit=2,
                    .measured_state=0,
                    .initial_amplitudes={c000, c100, c010, c110, c001, c101, c011, c111},
                    .expected_amplitudes={c000, c100, c010, c110, 0.0, 0.0, 0.0, 0.0}
                },
                RandomTestCase {
                    .measured_qubit=0,
                    .measured_state=1,
                    .initial_amplitudes={c000, c100, c010, c110, c001, c101, c011, c111},
                    .expected_amplitudes={0.0, c100, 0.0, c110, 0.0, c101, 0.0, c111}
                },
                RandomTestCase {
                    .measured_qubit=1,
                    .measured_state=1,
                    .initial_amplitudes={c000, c100, c010, c110, c001, c101, c011, c111},
                    .expected_amplitudes={0.0, 0.0, c010, c110, 0.0, 0.0, c011, c111}
                },
                RandomTestCase {
                    .measured_qubit=2,
                    .measured_state=1,
                    .initial_amplitudes={c000, c100, c010, c110, c001, c101, c011, c111},
                    .expected_amplitudes={0.0, 0.0, 0.0, 0.0, c001, c101, c011, c111}
                }
            );

            normalize(testcase.initial_amplitudes);
            normalize(testcase.expected_amplitudes);

            // the measured bit doesn't matter for now
            const auto info = impl_ket::create_m_gate(testcase.measured_qubit, 0);

            auto state = ket::QuantumState {testcase.initial_amplitudes};
            auto expected_state = ket::QuantumState {testcase.expected_amplitudes};

            simulate_measurement_wrapper(state, info, testcase.measured_state);

            REQUIRE(ket::almost_eq_with_print(state, expected_state));
        }
    }
}
