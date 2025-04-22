#pragma once

#include <functional>
#include <memory>

#include "kettle/circuit/classical_register.hpp"
#include "kettle/circuit/control_flow_function.hpp"
#include "kettle/gates/primitive_gate.hpp"

namespace ket
{

class QuantumCircuit;

}  // namespace ket


namespace impl_ket
{

class ControlFlowInstruction
{
public:
    ControlFlowInstruction(
        const ket::ControlFlowFunction& control_flow_function,
        std::unique_ptr<ket::QuantumCircuit> primary_circuit,
        std::unique_ptr<ket::QuantumCircuit> secondary_circuit = nullptr
    )
        : control_flow_function_ {control_flow_function}
        , primary_circuit_ {std::move(primary_circuit)}
        , secondary_circuit_ {std::move(secondary_circuit)}
    {}

    auto operator()(const ket::ClassicalRegister& c_register) const -> int
    {
        return control_flow_function_(c_register);
    }

    auto primary_circuit() const -> const ket::QuantumCircuit&
    {
        return *primary_circuit_;
    }

    auto secondary_circuit() const -> const ket::QuantumCircuit&
    {
        if (!secondary_circuit_) {
            throw std::runtime_error {"DEV ERROR: there is no secondary circuit\n"};
        }

        return *secondary_circuit_;
    }

    ControlFlowInstruction(const ControlFlowInstruction& other)
        : control_flow_function_ {other.control_flow_function_}
        , primary_circuit_ {std::make_unique<ket::QuantumCircuit>(*other.primary_circuit_)}
    {
        if (other.secondary_circuit_) {
            secondary_circuit_ = std::make_unique<ket::QuantumCircuit>(*other.secondary_circuit_);
        }
    }

    auto operator=(const ControlFlowInstruction& other) -> ControlFlowInstruction&
    {
        if (this != &other)
        {
            control_flow_function_ = other.control_flow_function_;
            primary_circuit_ = std::make_unique<ket::QuantumCircuit>(*other.primary_circuit_);
            if (other.secondary_circuit_) {
                secondary_circuit_ = std::make_unique<ket::QuantumCircuit>(*other.secondary_circuit_);
            }
        }

        return *this;
    }

    ControlFlowInstruction(ControlFlowInstruction&& other)
        : control_flow_function_ {std::move(other.control_flow_function_)}
        , primary_circuit_ {std::move(other.primary_circuit_)}
        , secondary_circuit_ {std::move(other.secondary_circuit_)}
    {}

    auto operator=(ControlFlowInstruction&& other) -> ControlFlowInstruction&
    {
        if (this != &other)
        {
            control_flow_function_ = std::move(other.control_flow_function_);
            primary_circuit_ = std::move(other.primary_circuit_);
            secondary_circuit_ = std::move(other.secondary_circuit_);
        }

        return *this;
    }

private:
    ket::ControlFlowFunction control_flow_function_;
    std::unique_ptr<ket::QuantumCircuit> primary_circuit_ = nullptr;
    std::unique_ptr<ket::QuantumCircuit> secondary_circuit_ = nullptr;
};

}  // namespace impl_ket
