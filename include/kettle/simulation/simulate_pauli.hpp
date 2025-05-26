#pragma once

#include "kettle/operator/pauli/sparse_pauli_string.hpp"
#include "kettle/state/state.hpp"


namespace ket
{

class StatevectorPauliStringSimulator
{
public:
    void run(const SparsePauliString& pauli_string, QuantumState& state);

    [[nodiscard]]
    auto has_been_run() const -> bool;

private:
    bool has_been_run_ {false};
};

void simulate(const SparsePauliString& pauli_string, QuantumState& state);

}  // namespace ket
