#include <iostream>

#include <mini-qiskit/mini-qiskit.hpp>

enum class QueryCase
{
    CONSTANT_0 = 0,
    CONSTANT_1 = 1,
    BALANCED_SAME = 2,
    BALANCED_SWAP = 3
};


void apply_query(mqis::QuantumCircuit& circuit, QueryCase parity)
{
    // NOTE: I could simplify this into two separate if-statements, but:
    // - leaving it like this makes the intent easier to see
    // - there aren't that many cases, so the downside of repetition isn't that bad
    using QC = QueryCase;

    switch (parity)
    {
        case QC::CONSTANT_0 : {
            break;
        }
        case QC::CONSTANT_1 : {
            circuit.add_x_gate(1);
            break;
        }
        case QC::BALANCED_SAME : {
            circuit.add_cx_gate(0, 1);
            break;
        }
        case QC::BALANCED_SWAP : {
            circuit.add_cx_gate(0, 1);
            circuit.add_x_gate(1);
            break;
        }
    }
}

auto main() -> int
{
    // first, we choose the query function; this is done by selecting one of the four cases as an
    // enum, and allowing the `apply_query()` function to choose the gates that correspond to the
    // given query function.
    const auto query = QueryCase::CONSTANT_1;

    // construct the initial state, in this case using a bitstring
    auto state = mqis::QuantumState {"01"};

    // include the gates needed for the Deutsch algorithm
    // NOTE: in the current version of mini-qiskit, it is assumed that a measurement gate is
    // applied to every qubit at the very end of the circuit
    auto circuit = mqis::QuantumCircuit {2};
    circuit.add_h_gate(1);
    circuit.add_h_gate(0);
    apply_query(circuit, query);
    circuit.add_h_gate(0);

    // propagate the state through the circuit
    mqis::simulate(circuit, state);

    // get the probabilities from the state; then perform the measurements
    const auto probabilities = mqis::calculate_probabilities(state);
    const auto measurements = mqis::perform_measurements(probabilities, 1000);

    // the measurements come out by default as a vector of all the results; for an easier
    // time viewing the results, convert it to a map of counts
    const auto counts = mqis::measurements_to_counts(measurements);

    // by default, the keys that represent the states are just integers corresponding to
    // how the state maps integers to bitstrings; we need to convert the integers to the
    // bitstrings themselves
    for (const auto [i_state, count] : counts) {
        const auto bitstring = mqis::state_as_bitstring(i_state, 2);
        std::cout << "(state, count) = (" << bitstring << ", " << count << ")\n";
    }

    // using `QueryCase::CONSTANT_1` for the query function, an example output is:
    // ```
    // (state, count) = (00, 528)
    // (state, count) = (01, 472)
    // ```
    // in other words, only states where the 0th bit is 0 were sampled

    return 0;
}
