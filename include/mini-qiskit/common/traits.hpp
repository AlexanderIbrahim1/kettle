#pragma once


namespace impl_mqis
{

template <typename T>
struct always_false : std::false_type
{};

}  // namespace impl_mqis
