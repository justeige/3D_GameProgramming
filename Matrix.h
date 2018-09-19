#pragma once

// --------------------------------------------------
// templated mathematical matrix
// --------------------------------------------------

#include <cstddef>

// generic base
template <class Type, std::size_t Rows, std::size_t Cols>
struct Matrix {
    Type data[Rows][Cols];
};

// specializations for the most used matrices
template <class Type>
struct Matrix<Type, 4, 4> {

    Matrix() : Matrix(0)
    {
    }

    Matrix(Type value)
    {
        for (std::size_t row = 0; row < 4; ++row) {
            for (std::size_t col = 0; col < 4; ++col) {
                data[row][col] = value;
            }
        }
    }

    Type data[4][4];
};

// typedefs for easier access
using float44 = Matrix<float, 4, 4>;

// generic operations
template <class Type, std::size_t Rows, std::size_t Cols>
bool operator == (Matrix<Type, Rows, Cols> const& a, Matrix<Type, Rows, Cols> const& b)
{
    for (std::size_t row = 0; row < Rows; ++row) {
        for (std::size_t col = 0; col < Cols; ++col) {
            if (a.data[row][col] != b.data[row][col]) {
                return false;
            }
        }
    }
    return true;
}

template <class Type, std::size_t Rows, std::size_t Cols>
bool operator != (Matrix<Type, Rows, Cols> const& a, Matrix<Type, Rows, Cols> const& b)
{
    for (std::size_t row = 0; row < Rows; ++row) {
        for (std::size_t col = 0; col < Cols; ++col) {
            if (a.data[row][col] != b.data[row][col]) {
                return true;
            }
        }
    }
    return false;
}

template <class Type, std::size_t Rows, std::size_t Cols>
Matrix<Type, Rows, Cols> operator + (Matrix<Type, Rows, Cols> const& a, Matrix<Type, Rows, Cols> const& b)
{
    Matrix<Type, Size> result;
    for (std::size_t row = 0; row < Rows; ++row) {
        for (std::size_t col = 0; col < Cols; ++col) {
            result.data[row][col] = a.data[row][col] + b.data[row][col];
        }
    }
    return result;
}

template <class Type, std::size_t Rows, std::size_t Cols>
Matrix<Type, Rows, Cols> operator - (Matrix<Type, Rows, Cols> const& a, Matrix<Type, Rows, Cols> const& b)
{
    Matrix<Type, Size> result;
    for (std::size_t row = 0; row < Rows; ++row) {
        for (std::size_t col = 0; col < Cols; ++col) {
            result.data[row][col] = a.data[row][col] - b.data[row][col];
        }
    }
    return result;
}

// --------------------------------------------------
// matix & scalar operations
// --------------------------------------------------

template <class Type, std::size_t Rows, std::size_t Cols>
Matrix<Type, Rows, Cols> operator * (Matrix<Type, Rows, Cols> const& mat, Type const& value)
{
    Matrix<Type, Size> result;
    for (std::size_t row = 0; row < Rows; ++row) {
        for (std::size_t col = 0; col < Cols; ++col) {
            result.data[row][col] = mat.data[row][col] * value;
        }
    }
    return result;
}

template <class Type, std::size_t Rows, std::size_t Cols>
Matrix<Type, Rows, Cols> operator * (Type const& value, Matrix<Type, Rows, Cols> const& mat)
{
    Matrix<Type, Size> result;
    for (std::size_t row = 0; row < Rows; ++row) {
        for (std::size_t col = 0; col < Cols; ++col) {
            result.data[row][col] = mat.data[row][col] * value;
        }
    }
    return result;
}

template <class Type, std::size_t Rows, std::size_t Cols>
Matrix<Type, Rows, Cols> operator / (Matrix<Type, Rows, Cols> const& mat, Type const& value)
{
    Matrix<Type, Size> result;
    for (std::size_t row = 0; row < Rows; ++row) {
        for (std::size_t col = 0; col < Cols; ++col) {
            result.data[row][col] = mat.data[row][col] / value;
        }
    }
    return result;
}

template <class Type, std::size_t Rows, std::size_t Cols>
Matrix<Type, Rows, Cols> operator + (Matrix<Type, Rows, Cols> const& mat, Type const& value)
{
    Matrix<Type, Size> result;
    for (std::size_t row = 0; row < Rows; ++row) {
        for (std::size_t col = 0; col < Cols; ++col) {
            result.data[row][col] = mat.data[row][col] + value;
        }
    }
    return result;
}

template <class Type, std::size_t Rows, std::size_t Cols>
Matrix<Type, Rows, Cols> operator + (Type const& value, Matrix<Type, Rows, Cols> const& mat)
{
    Matrix<Type, Size> result;
    for (std::size_t row = 0; row < Rows; ++row) {
        for (std::size_t col = 0; col < Cols; ++col) {
            result.data[row][col] = mat.data[row][col] + value;
        }
    }
    return result;
}

template <class Type, std::size_t Rows, std::size_t Cols>
Matrix<Type, Rows, Cols> operator - (Matrix<Type, Rows, Cols> const& mat, Type const& value)
{
    Matrix<Type, Size> result;
    for (std::size_t row = 0; row < Rows; ++row) {
        for (std::size_t col = 0; col < Cols; ++col) {
            result.data[row][col] = mat.data[row][col] - value;
        }
    }
    return result;
}

template <class Type, std::size_t Rows, std::size_t Cols>
Matrix<Type, Rows, Cols> operator - (Type const& value, Matrix<Type, Rows, Cols> const& mat)
{
    Matrix<Type, Size> result;
    for (std::size_t row = 0; row < Rows; ++row) {
        for (std::size_t col = 0; col < Cols; ++col) {
            result.data[row][col] = mat.data[row][col] - value;
        }
    }
    return result;
}

// --------------------------------------------------
// free matrix functions
// --------------------------------------------------

template <class Type, std::size_t Rows, std::size_t Cols>
Matrix<Type, Rows, Cols> identity()
{
    Matrix<Type, Size> result;
    for (std::size_t row = 0; row < Rows; ++row) {
        for (std::size_t col = 0; col < Cols; ++col) {
            const bool is_diagonal = row == col;
            mat[row][col] = is_diagonal ? 1 : 0;
        }
    }
    return result;
}