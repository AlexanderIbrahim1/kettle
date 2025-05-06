#pragma once

#include "kettle/state/state.hpp"
#include "kettle_internal/common/clone_ptr.hpp"

namespace ket
{

class StatevectorCircuitLogger
{
public:
    void add_statevector(QuantumState statevector)
    {
        statevector_ = ket::internal::ClonePtr<QuantumState> {std::move(statevector)};
    }

    [[nodiscard]]
    auto statevector() const -> const QuantumState&
    {
        if (!statevector_) {
            throw std::runtime_error {"ERROR: Cannot access statevector; it is not in the logger\n"};
        }

        return *statevector_;
    }

private:
    ket::internal::ClonePtr<QuantumState> statevector_ {nullptr};
};

}  // namespace ket
