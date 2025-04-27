#pragma once

#include <memory>
#include <variant>

#include "kettle/circuit/classical_register.hpp"
#include "kettle/circuit/control_flow_predicate.hpp"
#include "kettle/common/clone_ptr.hpp"

namespace ket
{

class QuantumCircuit;

}  // namespace ket


namespace impl_ket
{

/*
    A class that holds a single predicate and a single pointer to a circuit that
    is accessible if the predicate evaluates to true.

    This class holds the functionality needed for implementations of the classical
    if-statement, while-loop-statement, and do-while-loop-statement. It is the
    responsibility of other sections of the codebase that interface with control
    flow statements to handle the logic of how the circuit is treated.
*/
class ClassicalOneBranchBooleanStatement
{
public:
    ClassicalOneBranchBooleanStatement(
        ket::ControlFlowPredicate control_flow_predicate,
        std::unique_ptr<ket::QuantumCircuit> circuit
    )
        : control_flow_predicate_ {std::move(control_flow_predicate)}
        , circuit_ {std::move(circuit)}
    {}

    auto operator()(const ket::ClassicalRegister& c_register) const -> bool
    {
        return control_flow_predicate_(c_register);
    }

    [[nodiscard]]
    auto circuit() const -> const ClonePtr<ket::QuantumCircuit>&
    {
        return circuit_;
    }

    [[nodiscard]]
    auto predicate() const -> const ket::ControlFlowPredicate&
    {
        return control_flow_predicate_;
    }

private:
    ket::ControlFlowPredicate control_flow_predicate_;
    ClonePtr<ket::QuantumCircuit> circuit_;
};


class ClassicalIfStatement : public ClassicalOneBranchBooleanStatement
{
public:
    using ClassicalOneBranchBooleanStatement::ClassicalOneBranchBooleanStatement;
};


class ClassicalWhileLoopStatement : public ClassicalOneBranchBooleanStatement
{
public:
    using ClassicalOneBranchBooleanStatement::ClassicalOneBranchBooleanStatement;
};


class ClassicalIfElseStatement
{
public:
    ClassicalIfElseStatement(
        ket::ControlFlowPredicate control_flow_predicate,
        std::unique_ptr<ket::QuantumCircuit> if_circuit,
        std::unique_ptr<ket::QuantumCircuit> else_circuit
    )
        : control_flow_predicate_ {std::move(control_flow_predicate)}
        , if_circuit_ {std::move(if_circuit)}
        , else_circuit_ {std::move(else_circuit)}
    {}

    auto operator()(const ket::ClassicalRegister& c_register) const -> bool
    {
        return control_flow_predicate_(c_register);
    }

    [[nodiscard]]
    auto if_circuit() const -> const ClonePtr<ket::QuantumCircuit>&
    {
        return if_circuit_;
    }

    [[nodiscard]]
    auto else_circuit() const -> const ClonePtr<ket::QuantumCircuit>&
    {
        return else_circuit_;
    }

    [[nodiscard]]
    auto predicate() const -> const ket::ControlFlowPredicate&
    {
        return control_flow_predicate_;
    }

private:
    ket::ControlFlowPredicate control_flow_predicate_;
    ClonePtr<ket::QuantumCircuit> if_circuit_;
    ClonePtr<ket::QuantumCircuit> else_circuit_;
};


struct ClassicalControlFlowInstruction
{
public:
    // NOLINTNEXTLINE(*explicit*)
    ClassicalControlFlowInstruction(ClassicalIfStatement instruction)
        : instruction_ {std::move(instruction)}
    {}

    // NOLINTNEXTLINE(*explicit*)
    ClassicalControlFlowInstruction(ClassicalIfElseStatement instruction)
        : instruction_ {std::move(instruction)}
    {}

    // NOLINTNEXTLINE(*explicit*)
    ClassicalControlFlowInstruction(ClassicalWhileLoopStatement instruction)
        : instruction_ {std::move(instruction)}
    {}

    [[nodiscard]]
    constexpr auto is_if_statement() const -> bool
    {
        return std::holds_alternative<ClassicalIfStatement>(instruction_);
    }

    [[nodiscard]]
    constexpr auto is_if_else_statement() const -> bool
    {
        return std::holds_alternative<ClassicalIfElseStatement>(instruction_);
    }

    [[nodiscard]]
    constexpr auto is_while_loop_statement() const -> bool
    {
        return std::holds_alternative<ClassicalWhileLoopStatement>(instruction_);
    }

    [[nodiscard]]
    constexpr auto get_if_statement() const -> const ClassicalIfStatement&
    {
        return std::get<ClassicalIfStatement>(instruction_);
    }

    [[nodiscard]]
    constexpr auto get_if_else_statement() const -> const ClassicalIfElseStatement&
    {
        return std::get<ClassicalIfElseStatement>(instruction_);
    }

    [[nodiscard]]
    constexpr auto get_while_loop_statement() const -> const ClassicalWhileLoopStatement&
    {
        return std::get<ClassicalWhileLoopStatement>(instruction_);
    }

private:
    std::variant<
        ClassicalIfStatement,
        ClassicalIfElseStatement,
        ClassicalWhileLoopStatement
    > instruction_;
};

}  // namespace impl_ket
