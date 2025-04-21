# mini-qiskit

Mini-qiskit is a C++20 header-only library for simulating quantum circuits.
No external dependencies are needed, although Catch2 is used for unit testing.

Some of the main features:
  - create a quantum circuit (`mqis::QuantumCircuit`) and add a variety of gates
    - the supported single-qubit gates: {`H`, `X`, `Y`, `Z`, `SX`, `RX`, `RY`, `RZ`, `P`}
    - the supported contolled gates: {`CH`, `CX`, `CY`, `CZ`, `CSX`, `CRX`, `CRY`, `CRZ`, `CP`}
    - general 2x2 unitary single-qubit or controlled gates: {`U`, `CU`}
    - measurement gates: `M`
    - control flow based on measured classical bits (if statements)
  - create a statevector (`mqis::QuantumState`) and propagate it through the circuit (`mqis::simulate()`)
  - perform measurements on the resulting statevector

Other features:
  - appending/extending circuits
  - turning an existing circuit into a controlled subcircuit of another circuit
  - transpiling a circuit with `U` and `CU` gates into more primitive gates
  - create non-primitive gates (quantum fourier transform, swaps, toffoli, etc.)

Here's an example where we create one of the Bell states, and perform measurements on it

```cpp
#include <mini-qiskit/mini-qiskit.hpp>

auto main() -> int
{
    // create a quantum circuit with 2 qubit registers, and apply the H and CX gates
    auto circuit = mqis::QuantumCircuit {2};
    circuit.add_h_gate(0);
    circuit.add_cx_gate(0, 1);

    // begin with a 2-qubit statevector in the |00> state
    auto statevector = mqis::QuantumState {"00"};

    // propagate the state through the circuit, creating the (|00> + |11>) / sqrt(2) state
    mqis::simulate(circuit, statevector);

    // perform measurements on this statevector, with 1024 shots
    const auto counts = mqis::perform_measurements_as_counts(statevector, 1024);
    for (const auto& [bitstring, count] : counts) {
        std::cout << "(state, count) = (" << bitstring << ", " << count << ")\n";
    }

    // OUTPUT:
    // (state, count) = (11, 497)
    // (state, count) = (00, 527)

    return 0;
}
```

## Examples

Some commonly explored quantum algorithms are presented in `/examples/algorithms`, with detailed explanations.
There are also other examples in `/examples/general` to show off other features.

## Limitations
Currently, only noiseless statevector simulations are supported.

This project is still a work-in-progress, and the API is subject to further changes
as the project develops.

# Building and installing

See the [BUILDING](BUILDING.md) document.

# Contributing

See the [CONTRIBUTING](CONTRIBUTING.md) document.

# Licensing

<!--
Please go to https://choosealicense.com/licenses/ and choose a license that
fits your needs. The recommended license for a project of this type is the
GNU AGPLv3.
-->
