#include <iostream>

#include <kettle/kettle.hpp>

/*
    This file contains a demonstration of how to use the library to simulate
    the Deutsch algorithm. To pick which case the query function satisfies,
    select the appropriate QueryCase instance in the first non-comment line
    of the main() function below.
*/

enum class QueryCase
{
    CONSTANT_0,     // both bits map to 0
    CONSTANT_1,     // both bits map to 1
    BALANCED_SAME,  // both bits map to themselves
    BALANCED_SWAP   // 0 maps to 1, and 1 maps to 0
};

/*
    Apply the Deutsch query to the circuit; which query is applied, is determined by
    the choice of `query`
*/
void apply_query(ket::QuantumCircuit& circuit, QueryCase parity)
{
    using QC = QueryCase;

    switch (parity) {
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
    auto statevector = ket::QuantumState {"01"};

    // create the circuit with the gates needed for the Deutsch algorithm
    auto circuit = ket::QuantumCircuit {2};
    circuit.add_h_gate({0, 1});
    apply_query(circuit, query);
    circuit.add_h_gate(0);

    // propagate the state through the circuit
    ket::simulate(circuit, statevector);

    // get a map of the bitstrings to the counts; the ancilla qubit (at index `1`) is being marginalized
    const auto counts = ket::perform_measurements_as_counts_marginal(statevector, 1000, {1});

    for (const auto& [bitstring, count] : counts) {
        std::cout << "(state, count) = (" << bitstring << ", " << count << ")\n";
    }
    // using `QueryCase::CONSTANT_1` for the query function, an example output is:
    // ```
    // (state, count) = (00, 528)
    // (state, count) = (01, 472)
    // ```
    // in other words, only states where the 0th bit is 0 were sampled
    //
    // if the counts were calculated marginally, the output will be:
    // ```
    // (state, count) = (0x, 1000)
    // ```

    return 0;
}
