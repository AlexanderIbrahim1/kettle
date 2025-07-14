#pragma once

/*
    A mixed unitary channel is implemented as a probabilistic linear combination
    of QuantumCircuit instances.

    NOTE: this is *VERY* similar to the PauliChannel
      - all I would need to do is create a template class
      - and then create type aliases:
        - `using MixedUnitaryChannel = UnitaryChannelBase<ProbabilisticUnitary>;`
        - `using PauliChannel = UnitaryChannelBase<ProbabilisticPauliString>;`
    
    The drawback of using a template is that I can't hide the implementation from users

    Another option is to:
      - leave the public declarations of the two classes un-templated
      - have the private code templated
      - then have the definitions in the source files call the templated code
      + this way the public-private separation still works!
    
*/

#include "kettle/circuit/circuit.hpp"

namespace ket
{

// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init, hicpp-member-init)
struct ProbabilisticUnitary
{
    double coefficient;
    QuantumCircuit unitary;
};

class MixedUnitaryChannel
{
public:
    // TODO: get rid of magic number
    // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
    MixedUnitaryChannel(std::vector<ProbabilisticUnitary> weighted_pauli_strings, double tolerance = 1.0e-6);

    // TODO: get rid of magic number
    MixedUnitaryChannel(const std::initializer_list<ProbabilisticUnitary>& weighted_pauli_strings, double tolerance = 1.0e-6);

    [[nodiscard]]
    constexpr auto n_qubits() const noexcept -> std::size_t
    {
        return n_qubits_;
    }

    [[nodiscard]]
    constexpr auto size() const noexcept -> std::size_t
    {
        return weighted_unitaries_.size();
    }

    [[nodiscard]]
    constexpr auto weighted_unitaries() const noexcept -> const std::vector<ProbabilisticUnitary>&
    {
        return weighted_unitaries_;
    }

    [[nodiscard]]
    auto at(std::size_t index) const -> const ProbabilisticUnitary&
    {
        return weighted_unitaries_[index];
    }

    [[nodiscard]]
    auto at(std::size_t index) -> ProbabilisticUnitary&
    {
        return weighted_unitaries_[index];
    }

private:
    std::size_t n_qubits_;
    std::vector<ProbabilisticUnitary> weighted_unitaries_;
};

auto almost_eq(
    const MixedUnitaryChannel& left_op,
    const MixedUnitaryChannel& right_op,
    double coeff_tolerance
) -> bool;

}  // namespace ket
