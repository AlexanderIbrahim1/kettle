#pragma once

#include "kettle/circuit/classical_register.hpp"
#include "kettle/common/clone_ptr.hpp"

namespace ket
{

class ClassicalRegisterCircuitLogger
{
public:
    void add_classical_register(ClassicalRegister cregister)
    {
        cregister_ = impl_ket::ClonePtr<ClassicalRegister> {std::move(cregister)};
    }

    [[nodiscard]]
    auto classical_register() const -> const ClassicalRegister&
    {
        if (!cregister_) {
            throw std::runtime_error {"ERROR: Cannot access classical register; it is not in the logger\n"};
        }

        return *cregister_;
    }

private:
    // there is no default constructor for the ClassicalRegsiter (it wouldn't make sense), and we
    // only find out how many bits are needed after the first simulation; hence why we use a pointer
    impl_ket::ClonePtr<ClassicalRegister> cregister_ {nullptr};
};

}  // namespace ket
