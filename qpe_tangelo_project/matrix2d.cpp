#include "kettle/state/endian.hpp"
#include <complex>
#include <cstddef>
#include <format>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <vector>

#include <kettle/kettle.hpp>

/*
    This file contains code for a custom 2D matrix class, since I don't want to commit to making
    Eigen a dependency, and my requirements are fairly limited right now.
*/

using Vector = std::vector<std::complex<double>>;


class SquareMatrix2D
{
public:
    explicit SquareMatrix2D(std::size_t size)
        : size_ {size}
    {
        if (size == 0) {
            throw std::runtime_error {"The size of the square matrix cannot be 0\n"};
        }

        data_.resize(size_ * size_);
    }

    [[nodiscard]]
    constexpr auto size() const -> std::size_t
    {
        return size_;
    }

    void set(std::size_t i0, std::size_t i1, std::complex<double> value)
    {
        verify_index_bounds_(i0, i1);
        data_[index_(i0, i1)] = value;
    }

    [[nodiscard]]
    auto get(std::size_t i0, std::size_t i1) const -> std::complex<double>
    {
        return data_[index_(i0, i1)];
    }

private:
    std::size_t size_;
    std::vector<std::complex<double>> data_;

    void verify_index_bounds_(std::size_t i0, std::size_t i1) const
    {
        if (i0 >= size_ || i1 >= size_) {
            throw std::runtime_error {std::format("({}, {}) is out of bounds of matrix\n", i0, i1)};
        }
    }

    [[nodiscard]]
    auto index_(std::size_t i0, std::size_t i1) const -> std::size_t
    {
        return i1 + (i0 * size_);
    }
};


auto load_square_matrix(std::ifstream& instream, std::size_t size) -> SquareMatrix2D
{
    auto output = SquareMatrix2D {size};

    std::string line;
    std::size_t i0;  // NOLINT
    std::size_t i1;  // NOLINT
    double real;  // NOLINT
    double imag;  // NOLINT

    while (std::getline(instream, line)) {
        auto linestream = std::stringstream {line};
        linestream >> i0;
        linestream >> i1;
        linestream >> real;
        linestream >> imag;

        output.set(i0, i1, {real, imag});
    }

    return output;
}


auto multiply(const SquareMatrix2D& matrix, const Vector& amplitudes) -> Vector
{
    if (matrix.size() != amplitudes.size()) {
        throw std::runtime_error {"ERROR: cannot multiply; size mismatch\n"};
    }

    auto output = Vector {};
    output.reserve(amplitudes.size());

    for (std::size_t i0 {0}; i0 < matrix.size(); ++i0) {
        auto value = std::complex<double> {};
        for (std::size_t i1 {0}; i1 < matrix.size(); ++i1) {
            value += (matrix.get(i0, i1) * amplitudes[i1]);
        }
        output.push_back(value);
    }

    return output;
}


auto extract_amplitudes(const ket::QuantumState& state) -> Vector
{
    auto output = Vector {};
    output.reserve(state.n_states());

    for (std::size_t i {0}; i < state.n_states(); ++i) {
        output.emplace_back(state.at(i));
    }

    return output;
}


void endian_flip(Vector& data, std::size_t n_relevant_bits)
{
    for (std::size_t i {0}; i < data.size(); ++i) {
        const auto i_flip = ket::endian_flip(i, n_relevant_bits);
        if (i < i_flip) {
            std::swap(data[i], data[i_flip]);
        }
    }
}


auto expectation_value(const SquareMatrix2D& hamiltonian, const ket::QuantumState& state) -> std::complex<double>
{
    // the state stores data in little-endian format, but the hamiltonian is in big-endian format
    auto amplitudes = extract_amplitudes(state);
    endian_flip(amplitudes, state.n_qubits());  // now in BIG-format

    // now that the endian-ness of the two layouts is the same, perform the multiplication
    auto product = multiply(hamiltonian, amplitudes);

    // we need to swap back to get the inner product
    endian_flip(product, state.n_qubits());  // now in LITTLE-format
    const auto product_state = ket::QuantumState {product, ket::QuantumStateEndian::LITTLE, 1.0e30};

    return ket::inner_product(state, product_state);
}
