#pragma once

namespace impl_mqis
{

template <typename T>
struct always_false : std::false_type
{};

static constexpr auto MARGINALIZED_QUBIT = char {'x'};

}  // namespace impl_mqis
