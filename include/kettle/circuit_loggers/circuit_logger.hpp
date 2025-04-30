#pragma once

#include <variant>

#include "kettle/circuit_loggers/classical_register_circuit_logger.hpp"
#include "kettle/circuit_loggers/statevector_circuit_logger.hpp"

namespace ket
{

class CircuitLogger
{
public:
    // NOLINTNEXTLINE(*explicit*)
    CircuitLogger(ClassicalRegisterCircuitLogger circuit_logger)
        : circuit_logger_ {std::move(circuit_logger)}
    {}

    // NOLINTNEXTLINE(*explicit*)
    CircuitLogger(StatevectorCircuitLogger circuit_logger)
        : circuit_logger_ {std::move(circuit_logger)}
    {}

    [[nodiscard]]
    constexpr auto is_classical_register_circuit_logger() const -> bool
    {
        return std::holds_alternative<ClassicalRegisterCircuitLogger>(circuit_logger_);
    }

    [[nodiscard]]
    constexpr auto get_classical_register_circuit_logger() const -> const ClassicalRegisterCircuitLogger&
    {
        return std::get<ClassicalRegisterCircuitLogger>(circuit_logger_);
    }

    [[nodiscard]]
    constexpr auto is_statevector_circuit_logger() const -> bool
    {
        return std::holds_alternative<StatevectorCircuitLogger>(circuit_logger_);
    }

    [[nodiscard]]
    constexpr auto get_statevector_circuit_logger() const -> const StatevectorCircuitLogger&
    {
        return std::get<StatevectorCircuitLogger>(circuit_logger_);
    }

private:
    std::variant<ClassicalRegisterCircuitLogger, StatevectorCircuitLogger> circuit_logger_;
};

}  // namespace ket
