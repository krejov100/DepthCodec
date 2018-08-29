//
// Created by philip on 8/28/18.
//

#ifndef DEPTHCODEC_CEMATRIX_H
#define DEPTHCODEC_CEMATRIX_H

#include <array>
#include <type_traits>

// TODO: using C++17 (-std=c++1z) as std::array functions are not constexpr in C++14
// g++ -Wall -pedantic -Wno-missing-braces -std=c++1z -O0

template <typename T, std::size_t R, std::size_t C>
struct matrix
{
    std::array<std::array<T, C>, R> data_ {};

    constexpr T & operator()(const std::size_t i, const std::size_t j)
    {
        return data_.at(i).at(j);
    }

    constexpr const T & operator()(const std::size_t i, const std::size_t j) const
    {
        return data_.at(i).at(j);
    }

    template <typename U>
    constexpr bool operator==(const matrix<U, R, C> & rhs) const
    {
        for (std::size_t i = 0; i < R; ++i)
            for (std::size_t j = 0; j < C; ++j)
                if ((*this)(i, j) != rhs(i, j))
                    return false;
        return true;
    }

    template <typename U>
    constexpr bool operator!=(const matrix<U, R, C> & rhs) const
    {
        return !(*this == rhs);
    }

    // matrix addition
    constexpr auto operator+(const matrix<T, R, C> & b) const
    {
        matrix<T, R, C> result {};

        for (std::size_t i = 0; i < R; ++i)
            for (std::size_t j = 0; j < C; ++j)
                result(i, j) = (*this)(i, j) + b(i, j);

        return result;
    }

    // matrix multiplication
    template <typename U, std::size_t R2, std::size_t C2>
    constexpr auto operator*(const matrix<U, R2, C2> & b) const -> std::enable_if_t<C == R2, matrix<T, R, C2>>
    {
        matrix<T, R, C2> result {};

        for (std::size_t i = 0; i < R; ++i)
        {
            for (std::size_t j = 0; j < C2; ++j)
            {
                T v { 0 };
                for (std::size_t k = 0; k < R2; ++k)
                    v += (*this)(i, k) * b(k, j);
                result(i, j) = v;
            }
        }

        return result;
    }
};

template <std::size_t R, std::size_t C>
using imatrix = matrix<int, R, C>;
constexpr void test_multiply()
{
    constexpr imatrix<2, 2> a {0, -1, 1, 0};
    constexpr imatrix<2, 2> b {1, 2, 3, 4};

    // constexpr imatrix<3, 3> e;
    // static_assert(a == e, "equality test"); // --> size mismatch, will not compile

    // test multiplication
    constexpr auto ab = a * b;
    constexpr imatrix<2, 2> aXb {-3, -4, 1, 2};
    static_assert(ab == aXb, "multiply failed");

    // test addition
    constexpr imatrix<2, 2> c {2, -1, 1, 2};
    constexpr auto abc = ab + c;
    constexpr imatrix<2, 2> aXbPc {-1, -5, 2, 4};
    static_assert(abc == aXbPc, "addition failed");
}

#endif //DEPTHCODEC_CEMATRIX_H
