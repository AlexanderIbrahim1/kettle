#pragma once

#include "mini-qiskit/common/matrix2x2.hpp"

namespace mqis
{

static constexpr auto X_GATE = Matrix2X2 {
    {0.0, 0.0},
    {1.0, 0.0},
    {1.0, 0.0},
    {0.0, 0.0}
};

static constexpr auto Y_GATE = Matrix2X2 {
    {0.0, 0.0},
    {0.0, -1.0},
    {0.0, 1.0},
    {0.0, 0.0}
};

static constexpr auto Z_GATE = Matrix2X2 {
    {1.0, 0.0},
    {0.0, 0.0},
    {0.0, 0.0},
    {-1.0, 0.0}
};

}  // namespace mqis
