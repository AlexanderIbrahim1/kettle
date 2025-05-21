#pragma once

#include <cstdint>
#include <vector>


namespace ket
{

enum class PauliTerm : std::uint8_t
{
    I,
    X,
    Y,
    Z,
};

enum class PauliPhase : std::uint8_t
{
    PLUS_ONE,
    PLUS_EYE,
    MINUS_ONE,
    MINUS_EYE,
};

class PauliString
{
public:
    explicit PauliString(std::size_t n_qubits)
        : phase_ {PauliPhase::PLUS_ONE}
        , pauli_terms_(n_qubits, PauliTerm::I)
    {}

    [[nodiscard]]
    constexpr auto phase() const noexcept -> PauliPhase
    {
        return phase_;
    }

private:
    PauliPhase phase_ {};
    std::vector<PauliTerm> pauli_terms_;
};

}  // namespace ket
