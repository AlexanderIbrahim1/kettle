#pragma once

/*
    This header file contains the `transpile_to_primitive()` function, which takes an
    existing `QuantumCircuit` instance that may contain matrices that use unitary 2x2
    matrices as gates, and creates a new `QuantumCircuit` instance composed of only
    primitive gates.
*/

/*
PLAN:
  - take the current QuantumCircuit instance as input
  - create a new QuantumCircuit instance with the same number of qubits and bits
  - loop over all the gates
    - if the gate is already primitive transform or M, push it into the new vector
    - if the gate is U or CU:
      - decompose it, then push those into the new vector
      - no need to worry about matrix indices; the new QuantumCircuit won't have matrices
    - if the gate is CONTROL
      - call this function recursively for the relevant QuantumCircuit instances
      - then create a new ControlFlowInstruction object, and push that into the new vector
  - that should be it; just return the new QuantumCircuit

NOTES:
  - don't forget about the control flow circuits
    - these hold other instances of QuantumCircuit, which may themselves have non-primitive gates
  - this is probably easier if we make this function a friend of the QuantumCircuit class
  - test the decomposition by creating random states and random circuits
*/

namespace mqis
{

}  // namespace mqis
