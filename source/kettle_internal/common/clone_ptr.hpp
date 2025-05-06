#pragma once

#include <memory>

/*
    The ClonePtr class is a thin wrapper around `std::unique_ptr<T>` with a copy
    constructor and copy assignment operator that clone the data pointed to by the
    unique pointer.

    This is used in situations where:
      - a `std::shared_ptr<T>` isn't suitable because I want each instance to hold its
        own version of the data
      - I am forced to use a pointer type for whatever reason (such as needing to use
        a pointer to an incomplete type instead of the type itself, to avoid cyclic
        dependencies)
*/

namespace ket::internal
{

template <typename T>
class ClonePtr
{
public:
    constexpr explicit ClonePtr(T data)
        : data_ {std::make_unique<T>(data)}
    {}

    constexpr explicit ClonePtr(std::unique_ptr<T> data) noexcept
        : data_ {std::move(data)}
    {}

    constexpr ClonePtr(const ClonePtr& other)
        : data_ {other.data_ ? std::make_unique<T>(*other.data_) : nullptr}
    {}

    constexpr ClonePtr& operator=(const ClonePtr& other)
    {
        if (this != &other)
        {
            data_ = other.data_ ? std::make_unique<T>(*other.data_) : nullptr;
        }
        return *this;
    }

    constexpr ClonePtr(ClonePtr&& other) noexcept = default;
    constexpr ClonePtr& operator=(ClonePtr&& other) noexcept = default;

    auto operator*() const -> const T&
    {
        return *data_;
    }

    auto operator*() -> T&
    {
        return *data_;
    }

    constexpr explicit operator bool() const
    {
        return data_ != nullptr;
    }

    constexpr ~ClonePtr() = default;

private:
    std::unique_ptr<T> data_;
};

}  // namespace ket::internal
