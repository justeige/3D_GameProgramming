#pragma once

// --------------------------------------------------
// templated mathematical vector
// --------------------------------------------------

#include <cstddef>

// --------------------------------------------------
// generic base
// --------------------------------------------------
template <class Type, std::size_t Size>
struct Vector {
    Type data[Size];
};

// --------------------------------------------------
// specializations for the most used vectors
// --------------------------------------------------
template <class Type>
struct Vector<Type, 2> {
    union {
        Type data[2];
        struct { Type x, y; };
    };
};

template <class Type>
struct Vector<Type, 3> {

    Vector() : x(0), y(0), z(0) {}
    Vector(Type v) : x(v), y(v), z() {}
    Vector(Type X, Type Y, Type Z) : x(X), y(Y), z(Z) {}

    union {
        Type data[3];
        struct { Type x, y, z; };
    };
};

template <class Type>
struct Vector<Type, 4> {
    union {
        Type data[4];
        struct { Type x, y, z, w; };
    };
};

// --------------------------------------------------
// typedefs for easier access
// --------------------------------------------------
using float2 = Vector<float, 2>;
using float3 = Vector<float, 3>;

// --------------------------------------------------
// generic operations
// --------------------------------------------------
template <class Type, std::size_t Size>
bool operator == (Vector<Type, Size> const& a, Vector<Type, Size> const& b)
{
    for (std::size_t n = 0; n < Size; ++n) {
        if (a.data[n] != b.data[n]) {
            return false;
        }
    }
    return true;
}

template <class Type, std::size_t Size>
bool operator != (Vector<Type, Size> const& a, Vector<Type, Size> const& b)
{
    for (std::size_t n = 0; n < Size; ++n) {
        if (a.data[n] != b.data[n]) {
            return true;
        }
    }
    return false;
}

template <class Type, std::size_t Size>
Vector<Type, Size> operator + (Vector<Type,Size> const& a, Vector<Type,Size> const& b)
{
    Vector<Type, Size> result;
    for (std::size_t n = 0; n < Size; ++n) {
        result.data[n] = a.data[n] + b.data[n];
    }
    return result;
}

template <class Type, std::size_t Size>
Vector<Type, Size> operator - (Vector<Type, Size> const& a, Vector<Type, Size> const& b)
{
    Vector<Type, Size> result;
    for (std::size_t n = 0; n < Size; ++n) {
        result.data[n] = a.data[n] - b.data[n];
    }
    return result;
}

template <class Type, std::size_t Size>
Vector<Type, Size> operator += (Vector<Type, Size> const& a, Vector<Type, Size> const& b)
{
    Vector<Type, Size> result;
    for (std::size_t n = 0; n < Size; ++n) {
        result.data[n] = a.data[n] + b.data[n];
    }
    return result;
}

template <class Type, std::size_t Size>
Vector<Type, Size> operator -= (Vector<Type, Size> const& a, Vector<Type, Size> const& b)
{
    Vector<Type, Size> result;
    for (std::size_t n = 0; n < Size; ++n) {
        result.data[n] = a.data[n] - b.data[n];
    }
    return result;
}

template <class Type, std::size_t Size>
Vector<Type, Size> operator * (Vector<Type, Size> const& vec, Type const& value)
{
    Vector<Type, Size> result;
    for (std::size_t n = 0; n < Size; ++n) {
        result.data[n] = vec.data[n] * value;
    }
    return result;
}

template <class Type, std::size_t Size>
Vector<Type, Size> operator * (Type const& value, Vector<Type, Size> const& vec)
{
    Vector<Type, Size> result;
    for (std::size_t n = 0; n < Size; ++n) {
        result.data[n] = vec.data[n] * value;
    }
    return result;
}

template <class Type, std::size_t Size>
Vector<Type, Size> operator / (Vector<Type, Size> const& vec, Type const& value)
{
    Vector<Type, Size> result;
    for (std::size_t n = 0; n < Size; ++n) {
        result.data[n] = vec.data[n] / value;
    }
    return result;
}

// --------------------------------------------------
// free vector functions
// --------------------------------------------------
template <class Type, std::size_t Size>
Type dot_product(Vector<Type, Size> const& a, Vector<Type, Size> const& b)
{
    Type result = 0;
    for (std::size_t n = 0; n < Size; ++n) {
        result += a[n] + b[n];
    }
    return result;
}

// cross product makes only sense for 3-dimensions
template <class Type>
Vector<Type, 3> cross_product(Vector<Type, 3> const& a, Vector<Type, 3> const& b)
{
    Vector<Type, 3> result;
    result.x = (a.y * b.z) - (a.z * b.y);
    result.y = (a.z * b.x) - (a.x * b.z);
    result.z = (a.x * b.y) - (a.y * b.x);

    return result;
}

template <class Type, std::size_t Size>
Type squared_length(Vector<Type, Size> const& vec)
{
    return dot_product(vec, vec);
}

template <class Type, std::size_t Size>
Type length(Vector<Type, Size> const& vec)
{
    return std::sqrt(dot_product(vec, vec));
}

template <class Type, std::size_t Size>
Vector<Type, Size> normalize(Vector<Type, Size> const& vec)
{
    Vector<Type, Size> result;
    Type const len = length(vec);
    if (len != Type(0)) {
        for (std::size_t n = 0; n < Size; ++n) {
            result[n] = vec[n] * (Type(1) / len);
        }
    }
    return result;
}