#pragma once

#include <cstddef>
#include <functional>
#include <sstream>
#include <stdexcept>

#include "mini-qiskit/circuit/classical_register.hpp"


namespace mqis
{

using ControlFlowFunction = std::function<int(const ClassicalRegister&)>;

}  // namespace mqis


namespace impl_mqis
{

enum class ControlBooleanKind
{
    IF,
    IF_NOT
};

inline void check_if_measured_(std::size_t bit_index, const mqis::ClassicalRegister& c_register)
{
    if (!c_register.is_measured(bit_index)) {
        auto err_msg = std::stringstream {};
        err_msg << "There is no measured bit at classical register " << bit_index << '\n';
        throw std::runtime_error {err_msg.str()};
    }
}

inline auto flip_0_and_1_(int value) -> int
{
    if (value == 0) {
        return 1;
    }
    else if (value == 1) {
        return 0;
    }
    else {
        throw std::runtime_error {"DEV ERROR: unreachable, function only takes 0 or 1"};
    }
}

class SingleBitControlFlowFunction
{
public:
    SingleBitControlFlowFunction(std::size_t bit_index, ControlBooleanKind control_kind)
        : bit_index_ {bit_index}
        , control_kind_ {control_kind}
    {}

    auto operator()(const mqis::ClassicalRegister& c_register) const -> int
    {
        check_if_measured_(bit_index_, c_register);

        if (control_kind_ == ControlBooleanKind::IF) {
            return c_register.get(bit_index_);
        } else {
            return flip_0_and_1_(c_register.get(bit_index_));
        }
    }

private:
    std::size_t bit_index_;
    ControlBooleanKind control_kind_;
};

class RepeatControlFlowFunction
{
public:
    RepeatControlFlowFunction(int n_repetitions)
        : n_repetitions_ {n_repetitions}
    {}

    auto operator()([[maybe_unused]] const mqis::ClassicalRegister& c_register) const -> int
    {
        return n_repetitions_;
    }

private:
    int n_repetitions_;
};

}  // namespace impl_mqis
