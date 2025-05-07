#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <unordered_set>

#include <kettle/kettle.hpp>

/*
    A basic implementation of Shor's algorithm, inspired by the code from:
        https://github.com/Qiskit/textbook/blob/main/notebooks/ch-algorithms/shor.ipynb

    The goal of this algorithm is to take as inputs two integers: `a` and `N`
      - `a < `N`
      - `gcd(a, N) = 1`
    and find the "period" `r`, which is defined as the smallest positive integer such that
      - `a^r == 1 (mod N)`
    
    This example fixes `N = 15`, and allows choices of `a = 2, 4, 7, 8, 11, 13`

    This executable takes a command line argument (the integer `a`)

    For this explanation:
      - the qubits used to represent the first and second inputs to the collision function
        are referred to as the "counting qubits"
      - the additional qubits used to represent the f-query are the "ancilla qubits"
*/

/*
    This is an exhaustive set of all positive integers less than 15 that are
    mutually prime with 15
*/
const auto VALID_BASES = std::unordered_set<int> {2, 4, 7, 8, 11, 13};


/*
    This function applies the unitary operator of interest to the circuit in the manner
    required for QPE.

    More specifically, it applies the unitary operator in a multiplicity-binary controlled
    manner, where the gate is applied `2^n` times for the `n`th register qubit.

    The specific gates chosen in each case come directly from the aforementioned resource.
*/
void control_multiplication_mod15(
    ket::QuantumCircuit& circuit,
    int base,
    std::size_t control_qubit,
    std::size_t n_counting_qubits,
    std::size_t n_iterations
)
{
    if (!VALID_BASES.contains(base)) {
        throw std::runtime_error {"An invalid base has been entered"};
    }

    const auto i0 = n_counting_qubits;
    const auto i1 = 1 + n_counting_qubits;
    const auto i2 = 2 + n_counting_qubits;
    const auto i3 = 3 + n_counting_qubits;

    for (std::size_t i {0}; i < n_iterations; ++i) {
        if (base == 2 || base == 13) {
            ket::apply_control_swap(circuit, control_qubit, i2, i3);
            ket::apply_control_swap(circuit, control_qubit, i1, i2);
            ket::apply_control_swap(circuit, control_qubit, i0, i1);
        }

        if (base == 7 || base == 8) {
            ket::apply_control_swap(circuit, control_qubit, i0, i1);
            ket::apply_control_swap(circuit, control_qubit, i1, i2);
            ket::apply_control_swap(circuit, control_qubit, i2, i3);
        }

        if (base == 4 || base == 11) {
            ket::apply_control_swap(circuit, control_qubit, i1, i3);
            ket::apply_control_swap(circuit, control_qubit, i0, i2);
        }

        if (base == 7 || base == 11 || base == 13) {
            circuit.add_cx_gate(control_qubit, i0);
            circuit.add_cx_gate(control_qubit, i1);
            circuit.add_cx_gate(control_qubit, i2);
            circuit.add_cx_gate(control_qubit, i3);
        }
    }
}

/*
    Parses and performs error handling for the command line input
*/
auto parse_base(int argc, char** argv) -> int
{
    if (argc != 2) {
        throw std::runtime_error {"shor <base-integer>\n"};
    }

    auto base = std::stoi(argv[1]);

    return base;
}

/*
    A (naive) function for estimating the numerator and denominator from a
    floating-point number.
*/
auto numerator_and_denominator(
    double value,
    std::size_t max_denominator
) -> std::tuple<std::size_t, std::size_t>
{
    using Pair = std::tuple<std::size_t, std::size_t>;

    if (value < 0.0 || value > 1.0) {
        throw std::runtime_error {"This function only works if the value lies in [0, 1]\n"};
    }

    auto best = Pair {0, 1};
    auto diff = value;

    if (value > 0.5) {
        best = Pair {1, 1};
        diff = std::fabs(1.0 - value);
    }

    for (std::size_t denom {2}; denom < max_denominator; ++denom) {
        for (std::size_t numer {1}; numer < denom; ++numer) {
            const auto estimate = static_cast<double>(numer) / static_cast<double>(denom);
            const auto new_diff = std::fabs(estimate - value);

            if (new_diff < diff) {
                best = Pair {numer, denom};
                diff = new_diff;
            }
        }
    }

    return best;
}

auto main(int argc, char** argv) -> int
{
    // collect the base of the power function as a command line argument
    const auto base = parse_base(argc, argv);

    // determine the number of qubits needed for the problem
    // - the first 8 qubits are the counting qubits
    // - the last 4 qubits are the ancilla qubits
    const auto counting_qubits = ket::arange(8UL);
    const auto ancilla_qubits = ket::arange(8UL, 12UL);
    const auto n_counting_qubits = counting_qubits.size();
    const auto n_ancilla_qubits = ancilla_qubits.size();
    const auto n_total_qubits = n_counting_qubits + n_ancilla_qubits;

    // create the circuit
    auto circuit = ket::QuantumCircuit {n_total_qubits};

    // apply the gates needed to:
    // - turn the counting qubits into a uniform superposition of all possible states
    // - turn the ancilla qubits into the |1> state
    circuit.add_h_gate(counting_qubits);
    circuit.add_x_gate(n_counting_qubits);

    // apply the unitary operator for QPE
    for (auto i : ket::revarange(n_counting_qubits)) {
        const auto n_iterations = 1UL << i;
        control_multiplication_mod15(circuit, base, i, n_counting_qubits, n_iterations);
    }

    // the final step of QPE requires the inverse QFT
    ket::apply_inverse_fourier_transform(circuit, ket::revarange(n_counting_qubits));

    // create the statevector and evolve the quantum state
    auto statevector = ket::QuantumState {n_total_qubits};
    ket::simulate(circuit, statevector);

    // get a map of the bitstrings to the counts; in Shor's algorithm, we are concerned
    // with the output of the counting qubits, and thus we marginalize the ancilla qubits
    const auto counts = ket::perform_measurements_as_counts_marginal(statevector, 1 << 10, ancilla_qubits);

    for (const auto& [bitstring, count] : counts) {
        // the manner in which we apply the controlled unitary gates for QPE affects the output;
        // - in this example, the 0th qubit was applied once, the 1st qubit was applied twice, etc.
        // - this means the largest contributor is on the right of the bitstring
        //   - and we need to reverse the bitstring being calculating the binary fraction expansion
        auto rstripped_bitstring = ket::rstrip_marginal_bits(bitstring);
        std::ranges::reverse(rstripped_bitstring);

        const auto binary_fraction = ket::binary_fraction_expansion(rstripped_bitstring);
        const auto [numer, denom] = numerator_and_denominator(binary_fraction, 15);

        std::cout << "(state, count)     = (" << bitstring << ", " << count << ")\n";
        std::cout << "    phase          = " << binary_fraction << '\n';
        std::cout << "    fraction guess = " << numer << "/" << denom << '\n';
    }

    // Example output for an input of `7`:
    // ```
    // (state, count)     = (00000011xxxx, 271)
    //     phase          = 0.75
    //     fraction guess = 3/4
    // (state, count)     = (00000010xxxx, 244)
    //     phase          = 0.25
    //     fraction guess = 1/4
    // (state, count)     = (00000000xxxx, 255)
    //     phase          = 0
    //     fraction guess = 0/1
    // (state, count)     = (00000001xxxx, 254)
    //     phase          = 0.5
    //     fraction guess = 1/2
    // ```
    //
    // The correct answer for the "period" is `4`;
    // - two of the outputs give the correct result outright (1/4 and 3/4)
    // - one can't really be deciphered (0/4)
    // - the last is really (2/4), but 2 and 4 are coprime, and the outcome is (1/2)

    return 0;
}
