# kettle

Kettle is a C++20 library for simulating quantum circuits.

There are a few required dependencies, all of which are automatically installed with CMake:
  - nlopt (for VQE)
  - Eigen (for density matrices and matrix-related operations)
  - Catch2 (for unit testing in the `dev` build)

Some of the main features:
  - create a quantum circuit (`ket::QuantumCircuit`) and add a variety of gates
    - the supported single-qubit gates: {`H`, `X`, `Y`, `Z`, `S`, `SDAG`, `T`, `TDAG`, `SX`, `SXDAG`, `RX`, `RY`, `RZ`, `P`}
    - the supported controlled gates: {`CH`, `CX`, `CY`, `CZ`, `CS`, `CSDAG`, `CT`, `CTDAG`, `CSX`, `CSXDAG`, `CRX`, `CRY`, `CRZ`, `CP`}
    - general 2x2 unitary single-qubit or controlled gates: {`U`, `CU`}
    - measurement gates: `M`
    - control flow based on measured classical bits (if and if-else statements)
  - create a statevector (`ket::Statevector`) or density matrix (`ket::DensityMatrix`) and propagate it through the circuit (`ket::simulate()`)
  - perform measurements on the resulting state
  - evaluate Hamiltonians in the form of linear combinations of Pauli strings
  - create parameterized circuits
  - perform VQE with these parameterized circuits

Other features:
  - appending/extending circuits
  - turning an existing circuit into a controlled subcircuit of another circuit
  - transpiling a circuit with `U` and `CU` gates into more primitive gates
  - create non-primitive gates (quantum fourier transform, swaps, toffoli, etc.)
  - taking tensor products and partial traces of states

Here's an example where we create one of the Bell states, and perform measurements on it:
```cpp
#include <kettle/kettle.hpp>

auto main() -> int
{
    // create a quantum circuit with 2 qubit registers, and apply the H and CX gates
    auto circuit = ket::QuantumCircuit {2};
    circuit.add_h_gate(0);
    circuit.add_cx_gate(0, 1);

    // begin with a 2-qubit statevector in the |00> state
    auto statevector = ket::Statevector {"00"};

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

## Future Plans
In the near future, I plan to add:
  - Kraus channels and other channels in the near future
  - other channels, and conversions between these channels
  - functions for collecting quantum information (fidelity, etc.)

See the GitHub issues section for more details.

This project is still a work-in-progress, and the API is subject to further changes as the project develops.

## Building as the top level project
This project uses CMake presets for building.
So far it has only been built and tested on Ubuntu 23.04.

To build examples and unit tests as the top-level project (requires `FetchContent`-related functions to work with CMake):
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

## Integration via CMake
This project can be integrated into another project using CMake's `FetchContent`.

Add the following to your `CMakeLists.txt`:
```cmake
include(FetchContent)

FetchContent_Declare(
    kettle
    GIT_REPOSITORY git@github.com:AlexanderIbrahim1/kettle.git
    GIT_TAG main
)

FetchContent_MakeAvailable(kettle)  # kettle::kettle
```

Then link it to your executable:
```cmake
add_executable(my_executable main.cpp)
target_link_libraries(my_executable PRIVATE kettle::kettle)
```

The contents of `kettle` will be made available by including the `<kettle/kettle.hpp>` header,
within the `ket::` namespace.

## Contributing

Thank you for considering making contributions to this project!
You can use the issue tab to open a bug report or feature request.

## Acknowledgements

This project was initially generated with the [`cmake-init`](https://github.com/friendlyanon/cmake-init) project.
