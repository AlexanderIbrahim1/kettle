#pragma once

#include "kettle/state/density_matrix.hpp"
#include "kettle/common/clone_ptr.hpp"

namespace ket
{

class DensityMatrixCircuitLogger
{
public:
    void add_density_matrix(DensityMatrix density_matrix)
    {
        density_matrix_ = ket::ClonePtr<DensityMatrix> {std::move(density_matrix)};
    }

    [[nodiscard]]
    auto density_matrix() const -> const DensityMatrix&
    {
        if (!density_matrix_) {
            throw std::runtime_error {"ERROR: Cannot access density matrix; it is not in the logger\n"};
        }

        return *density_matrix_;
    }

private:
    ket::ClonePtr<DensityMatrix> density_matrix_ {nullptr};
};

}  // namespace ket
