#pragma once

#include <variant>

#include "kettle/gates/primitive_gate.hpp"
#include "kettle/circuit/control_flow.hpp"
#include "kettle/circuit_loggers/classical_register_circuit_logger.hpp"
#include "kettle/circuit_loggers/circuit_logger.hpp"

/*
    This header file contains the CircuitElement type, which holds all the different
    constructs that determine how the quantum circuit will evolve (gates, control flow
    instructions, etc.).
*/


namespace impl_ket
{

/*
    A thin wrapper around `std::variant`, with member functions to make it more
    convenient to work with.
*/
struct CircuitElement
{
public:
    // NOLINTNEXTLINE(*-explicit-*)
    CircuitElement(const ket::GateInfo& ginfo)
        : element_ {ginfo}
    {}

    // NOLINTNEXTLINE(*-explicit-*)
    CircuitElement(ClassicalIfStatement instruction)
        : element_ {std::move(instruction)}
    {}

    // NOLINTNEXTLINE(*-explicit-*)
    CircuitElement(ClassicalIfElseStatement instruction)
        : element_ {std::move(instruction)}
    {}

    // NOLINTNEXTLINE(*explicit*)
    CircuitElement(ket::CircuitLogger logger)
        : element_ {std::move(logger)}
    {}

    // NOLINTNEXTLINE(*explicit*)
    CircuitElement(ket::ClassicalRegisterCircuitLogger logger)
        : element_ {std::move(logger)}
    {}

    [[nodiscard]]
    constexpr auto is_gate() const -> bool
    {
        return std::holds_alternative<ket::GateInfo>(element_);
    }

    [[nodiscard]]
    constexpr auto is_control_flow() const -> bool
    {
        return std::holds_alternative<ClassicalControlFlowInstruction>(element_);
    }

    [[nodiscard]]
    constexpr auto is_circuit_logger() const -> bool
    {
        return std::holds_alternative<ket::CircuitLogger>(element_);
    }

    [[nodiscard]]
    constexpr auto get_gate() const -> const ket::GateInfo&
    {
        return std::get<ket::GateInfo>(element_);
    }

    [[nodiscard]]
    constexpr auto get_control_flow() const -> const ClassicalControlFlowInstruction&
    {
        return std::get<ClassicalControlFlowInstruction>(element_);
    }

    [[nodiscard]]
    constexpr auto get_circuit_logger() const -> const ket::CircuitLogger&
    {
        return std::get<ket::CircuitLogger>(element_);
    }

private:
    std::variant<ket::GateInfo, ClassicalControlFlowInstruction, ket::CircuitLogger> element_;
};


}  // namespace impl_ket
