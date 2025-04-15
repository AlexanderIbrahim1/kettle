#pragma once

#include <functional>
#include <memory>

#include "mini-qiskit/circuit/classical_register.hpp"
#include "mini-qiskit/primitive_gate.hpp"


namespace mqis
{

using ControlFlowFunction = std::function<int(const ClassicalRegister&)>;

class QuantumCircuit;

}  // namespace mqis


namespace impl_mqis
{

class ControlFlowInstruction
{
public:
    ControlFlowInstruction(
        const mqis::ControlFlowFunction& control_flow_function,
        std::unique_ptr<mqis::QuantumCircuit> primary_circuit,
        std::unique_ptr<mqis::QuantumCircuit> secondary_circuit = nullptr
    )
        : control_flow_function_ {control_flow_function}
        , primary_circuit_ {std::move(primary_circuit)}
        , secondary_circuit_ {std::move(secondary_circuit)}
    {}

    auto operator()(const mqis::ClassicalRegister& c_register) const -> int
    {
        return control_flow_function_(c_register);
    }

    auto primary_circuit() const -> const mqis::QuantumCircuit&
    {
        return *primary_circuit_;
    }

    auto secondary_circuit() const -> const mqis::QuantumCircuit&
    {
        if (!secondary_circuit_) {
            throw std::runtime_error {"DEV ERROR: there is no secondary circuit\n"};
        }

        return *secondary_circuit_;
    }

    ControlFlowInstruction(const ControlFlowInstruction& other)
        : control_flow_function_ {other.control_flow_function_}
        , primary_circuit_ {std::make_unique<mqis::QuantumCircuit>(*other.primary_circuit_)}
        , secondary_circuit_ {std::make_unique<mqis::QuantumCircuit>(*other.secondary_circuit_)}
    {}

    auto operator=(const ControlFlowInstruction& other) -> ControlFlowInstruction&
    {
        if (this != &other)
        {
            control_flow_function_ = other.control_flow_function_;
            primary_circuit_ = std::make_unique<mqis::QuantumCircuit>(*other.primary_circuit_);
            secondary_circuit_ = std::make_unique<mqis::QuantumCircuit>(*other.secondary_circuit_);
        }

        return *this;
    }

private:
    mqis::ControlFlowFunction control_flow_function_;
    std::unique_ptr<mqis::QuantumCircuit> primary_circuit_;
    std::unique_ptr<mqis::QuantumCircuit> secondary_circuit_;
};

}
