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

namespace impl_ket
{

template <typename T>
class ClonePtr
{
public:
    ClonePtr(std::unique_ptr<T> data)
        : data_ {std::move(data)}
    {}

    ClonePtr(const ClonePtr& other)
        : data_ {other.data_ ? std::make_unique<T>(*other.data_) : nullptr}
    {}

    ClonePtr& operator=(const ClonePtr& other)
    {
        if (this != &other)
        {
            data_ = other.data_ ? std::make_unique<T>(*other.data_) : nullptr;
        }
        return *this;
    }

    ClonePtr(ClonePtr&& other) noexcept = default;
    ClonePtr& operator=(ClonePtr&& other) noexcept = default;

    auto operator*() const -> T
    {
        return *data_;
    }

private:
    std::unique_ptr<T> data_;
};

}  // namespace impl_ket
