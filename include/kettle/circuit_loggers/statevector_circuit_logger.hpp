#pragma once

#include "kettle/state/statevector.hpp"
#include "kettle/common/clone_ptr.hpp"

namespace ket
{

class StatevectorCircuitLogger
{
public:
    void add_statevector(Statevector statevector)
    {
        statevector_ = ket::ClonePtr<Statevector> {std::move(statevector)};
    }

    [[nodiscard]]
    auto statevector() const -> const Statevector&
    {
        if (!statevector_) {
            throw std::runtime_error {"ERROR: Cannot access statevector; it is not in the logger\n"};
        }

        return *statevector_;
    }

private:
    ket::ClonePtr<Statevector> statevector_ {nullptr};
};

}  // namespace ket
