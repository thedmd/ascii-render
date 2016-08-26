#pragma once
#include <cmath>

struct matrix4;

struct vec3
{
    float x, y, z;

    vec3();
    vec3(float x, float y, float z);

    vec3 operator + (const vec3& v) const;
    vec3 operator - (const vec3& v) const;

    friend vec3 operator * (const vec3& v, float s);
    friend vec3 operator * (float s, const vec3& v);

    float dot(const vec3& rhs) const;

    vec3 normalized() const;

    vec3 transformed(const matrix4& m) const;
    vec3 transformed_vector(const matrix4& m) const;
};

template <typename T>
struct matrix4_consts
{
    static const T identity;
};

template <typename T> const T matrix4_consts<T>::identity(
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f);

struct matrix4: matrix4_consts<matrix4>
{
    float m_Matrix[4][4];

    matrix4();
    matrix4(float m11, float m12, float m13, float m14,
            float m21, float m22, float m23, float m24,
            float m31, float m32, float m33, float m34,
            float m41, float m42, float m43, float m44);

    matrix4 operator * (const matrix4& m) const;

          float& operator[](int index);
    const float& operator[](int index) const;

    inline matrix4 transposed() const;

    static matrix4 lookAtLH(const vec3& eye, const vec3& at, const vec3& up);

    static matrix4 matrix4::perspectiveFovLH(float fovy, float aspect, float n, float f);

    static matrix4 rotationYawPitchRoll(float yaw, float pitch, float roll);

    static matrix4 translation(float x, float y, float z);
};

vec3 cross(const vec3& lhs, const vec3& rhs);

inline vec3 project(const vec3& v, float viewX, float viewY, float viewW, float viewH, float minZ, float maxZ, const matrix4& transformation);

#include "math.inl"