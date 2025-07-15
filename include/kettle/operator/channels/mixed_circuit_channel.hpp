#pragma once

/*
    A mixed unitary channel is implemented as a probabilistic linear combination of QuantumCircuit instances.

    This is *very* similar to the PauliChannel, so one possibility would be to use a common template. The
    drawback of using a template is that I cannot hide the implementation details from users, and type
    aliases require the definition to be available at the point of the alias.

    Instead, we leave the public declaration un-templated, and have the definitions call templated functions
    within the source files.
*/

#include "kettle/circuit/circuit.hpp"

namespace ket
{

// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init, hicpp-member-init)
struct WeightedCircuit
{
    double coefficient;
    QuantumCircuit unitary;
};

class MixedCircuitChannel
{
public:
    // TODO: get rid of magic number
    // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
    MixedCircuitChannel(std::vector<WeightedCircuit> weighted_operators, double tolerance = 1.0e-6);

    // TODO: get rid of magic number
    MixedCircuitChannel(const std::initializer_list<WeightedCircuit>& weighted_operators, double tolerance = 1.0e-6);

    [[nodiscard]]
    constexpr auto n_qubits() const noexcept -> std::size_t
    {
        return n_qubits_;
    }

    [[nodiscard]]
    constexpr auto size() const noexcept -> std::size_t
    {
        return weighted_operators_.size();
    }

    [[nodiscard]]
    constexpr auto weighted_operators() const noexcept -> const std::vector<WeightedCircuit>&
    {
        return weighted_operators_;
    }

    [[nodiscard]]
    auto at(std::size_t index) const -> const WeightedCircuit&
    {
        return weighted_operators_[index];
    }

    [[nodiscard]]
    auto at(std::size_t index) -> WeightedCircuit&
    {
        return weighted_operators_[index];
    }

private:
    std::size_t n_qubits_;
    std::vector<WeightedCircuit> weighted_operators_;
};

// TODO: replace magic number
auto almost_eq(
    const MixedCircuitChannel& left_op,
    const MixedCircuitChannel& right_op,
    double coeff_tolerance = 1.0e-6
) -> bool;

}  // namespace ket
