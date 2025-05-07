# kettle

Kettle is a C++20 library for simulating quantum circuits.
No external dependencies are needed, although Catch2 is used for unit testing in the `dev` build.

Some of the main features:
  - create a quantum circuit (`ket::QuantumCircuit`) and add a variety of gates
    - the supported single-qubit gates: {`H`, `X`, `Y`, `Z`, `SX`, `RX`, `RY`, `RZ`, `P`}
    - the supported contolled gates: {`CH`, `CX`, `CY`, `CZ`, `CSX`, `CRX`, `CRY`, `CRZ`, `CP`}
    - general 2x2 unitary single-qubit or controlled gates: {`U`, `CU`}
    - measurement gates: `M`
    - control flow based on measured classical bits (if statements)
  - create a statevector (`ket::QuantumState`) and propagate it through the circuit (`ket::simulate()`)
  - perform measurements on the resulting statevector

Other features:
  - appending/extending circuits
  - turning an existing circuit into a controlled subcircuit of another circuit
  - transpiling a circuit with `U` and `CU` gates into more primitive gates
  - create non-primitive gates (quantum fourier transform, swaps, toffoli, etc.)

Here's an example where we create one of the Bell states, and perform measurements on it

```cpp
#include <kettle/kettle.hpp>

auto main() -> int
{
    // create a quantum circuit with 2 qubit registers, and apply the H and CX gates
    auto circuit = ket::QuantumCircuit {2};
    circuit.add_h_gate(0);
    circuit.add_cx_gate(0, 1);

    // begin with a 2-qubit statevector in the |00> state
    auto statevector = ket::QuantumState {"00"};

    // propagate the state through the circuit, creating the (|00> + |11>) / sqrt(2) state
    ket::simulate(circuit, statevector);

    // perform measurements on this statevector, with 1024 shots
    const auto counts = ket::perform_measurements_as_counts(statevector, 1024);
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

## Building

This project uses cmake presets for building.
So far it has only been built and tested on Ubuntu 23.04.

To build examples and unit tests (requires `FetchContent`-related functions to work with cmake):
```sh
cmake --preset=dev
cmake --build --preset=dev
```

To build without unit tests, there are three different modes:
  - `debug`
  - `release`
  - `highperf` (just `release` with `-ffast-math` and `-march=native`)

For example:
```sh
cmake --preset=release
cmake --build --preset=release
```

## Contributing

Thank you for considering making contributions to this project!
You can use the issue tab to open a bug report or feature request.

## Acknowledgements

This project was initially generated with the [`cmake-init`](https://github.com/friendlyanon/cmake-init) project.
