#pragma once
#include "math.h"

inline vec3::vec3() {}
inline vec3::vec3(float x, float y, float z): x(x), y(y), z(z) {}

inline vec3 vec3::operator + (const vec3& v) const { return vec3(x + v.x, y + v.y, z + v.z); }
inline vec3 vec3::operator - (const vec3& v) const { return vec3(x - v.x, y - v.y, z - v.z); }

inline vec3 operator * (const vec3& v, float s) { return vec3(v.x * s, v.y * s, v.z * s); }
inline vec3 operator * (float s, const vec3& v) { return vec3(v.x * s, v.y * s, v.z * s); }

inline float vec3::dot(const vec3& rhs) const { return x * rhs.x + y * rhs.y + z * rhs.z; }

inline vec3 vec3::normalized() const
{
    const auto denom = 1.0f / sqrtf(dot(*this));
    return vec3(x * denom, y * denom, z * denom);
}

inline vec3 vec3::transformed(const matrix4& m) const
{
    float w = 1.0f / (m[3] * x + m[7] * y + m[11] * z + m[15]);
    return vec3(
        (m[0] * x + m[4] * y + m[ 8] * z + m[12]) * w,
        (m[1] * x + m[5] * y + m[ 9] * z + m[13]) * w,
        (m[2] * x + m[6] * y + m[10] * z + m[14]) * w);
}

inline vec3 vec3::transformed_vector(const matrix4& m) const
{
    return vec3(
        m[0] * x + m[4] * y + m[ 8] * z,
        m[1] * x + m[5] * y + m[ 9] * z,
        m[2] * x + m[6] * y + m[10] * z);
}

inline matrix4::matrix4() {}
inline matrix4::matrix4(float m11, float m12, float m13, float m14,
                        float m21, float m22, float m23, float m24,
                        float m31, float m32, float m33, float m34,
                        float m41, float m42, float m43, float m44)
{
    m_Matrix[0][0] = m11;  m_Matrix[0][1] = m12;  m_Matrix[0][2] = m13;  m_Matrix[0][3] = m14;
    m_Matrix[1][0] = m21;  m_Matrix[1][1] = m22;  m_Matrix[1][2] = m23;  m_Matrix[1][3] = m24;
    m_Matrix[2][0] = m31;  m_Matrix[2][1] = m32;  m_Matrix[2][2] = m33;  m_Matrix[2][3] = m34;
    m_Matrix[3][0] = m41;  m_Matrix[3][1] = m42;  m_Matrix[3][2] = m43;  m_Matrix[3][3] = m44;
}

inline matrix4 matrix4::operator * (const matrix4& m) const
{
    return matrix4(m.m_Matrix[0][0] * m_Matrix[0][0] + m.m_Matrix[1][0] * m_Matrix[0][1] + m.m_Matrix[2][0] * m_Matrix[0][2] + m.m_Matrix[3][0] * m_Matrix[0][3],
                   m.m_Matrix[0][1] * m_Matrix[0][0] + m.m_Matrix[1][1] * m_Matrix[0][1] + m.m_Matrix[2][1] * m_Matrix[0][2] + m.m_Matrix[3][1] * m_Matrix[0][3],
                   m.m_Matrix[0][2] * m_Matrix[0][0] + m.m_Matrix[1][2] * m_Matrix[0][1] + m.m_Matrix[2][2] * m_Matrix[0][2] + m.m_Matrix[3][2] * m_Matrix[0][3],
                   m.m_Matrix[0][3] * m_Matrix[0][0] + m.m_Matrix[1][3] * m_Matrix[0][1] + m.m_Matrix[2][3] * m_Matrix[0][2] + m.m_Matrix[3][3] * m_Matrix[0][3],

                   m.m_Matrix[0][0] * m_Matrix[1][0] + m.m_Matrix[1][0] * m_Matrix[1][1] + m.m_Matrix[2][0] * m_Matrix[1][2] + m.m_Matrix[3][0] * m_Matrix[1][3],
                   m.m_Matrix[0][1] * m_Matrix[1][0] + m.m_Matrix[1][1] * m_Matrix[1][1] + m.m_Matrix[2][1] * m_Matrix[1][2] + m.m_Matrix[3][1] * m_Matrix[1][3],
                   m.m_Matrix[0][2] * m_Matrix[1][0] + m.m_Matrix[1][2] * m_Matrix[1][1] + m.m_Matrix[2][2] * m_Matrix[1][2] + m.m_Matrix[3][2] * m_Matrix[1][3],
                   m.m_Matrix[0][3] * m_Matrix[1][0] + m.m_Matrix[1][3] * m_Matrix[1][1] + m.m_Matrix[2][3] * m_Matrix[1][2] + m.m_Matrix[3][3] * m_Matrix[1][3],

                   m.m_Matrix[0][0] * m_Matrix[2][0] + m.m_Matrix[1][0] * m_Matrix[2][1] + m.m_Matrix[2][0] * m_Matrix[2][2] + m.m_Matrix[3][0] * m_Matrix[2][3],
                   m.m_Matrix[0][1] * m_Matrix[2][0] + m.m_Matrix[1][1] * m_Matrix[2][1] + m.m_Matrix[2][1] * m_Matrix[2][2] + m.m_Matrix[3][1] * m_Matrix[2][3],
                   m.m_Matrix[0][2] * m_Matrix[2][0] + m.m_Matrix[1][2] * m_Matrix[2][1] + m.m_Matrix[2][2] * m_Matrix[2][2] + m.m_Matrix[3][2] * m_Matrix[2][3],
                   m.m_Matrix[0][3] * m_Matrix[2][0] + m.m_Matrix[1][3] * m_Matrix[2][1] + m.m_Matrix[2][3] * m_Matrix[2][2] + m.m_Matrix[3][3] * m_Matrix[2][3],

                   m.m_Matrix[0][0] * m_Matrix[3][0] + m.m_Matrix[1][0] * m_Matrix[3][1] + m.m_Matrix[2][0] * m_Matrix[3][2] + m.m_Matrix[3][0] * m_Matrix[3][3],
                   m.m_Matrix[0][1] * m_Matrix[3][0] + m.m_Matrix[1][1] * m_Matrix[3][1] + m.m_Matrix[2][1] * m_Matrix[3][2] + m.m_Matrix[3][1] * m_Matrix[3][3],
                   m.m_Matrix[0][2] * m_Matrix[3][0] + m.m_Matrix[1][2] * m_Matrix[3][1] + m.m_Matrix[2][2] * m_Matrix[3][2] + m.m_Matrix[3][2] * m_Matrix[3][3],
                   m.m_Matrix[0][3] * m_Matrix[3][0] + m.m_Matrix[1][3] * m_Matrix[3][1] + m.m_Matrix[2][3] * m_Matrix[3][2] + m.m_Matrix[3][3] * m_Matrix[3][3]);
}

inline       float& matrix4::operator[](int index)       { return reinterpret_cast<      float*>(m_Matrix)[index]; }
inline const float& matrix4::operator[](int index) const { return reinterpret_cast<const float*>(m_Matrix)[index]; }

inline matrix4 matrix4::transposed() const
{
    return matrix4(
        m_Matrix[0][0], m_Matrix[1][0], m_Matrix[2][0], m_Matrix[3][0],
        m_Matrix[0][1], m_Matrix[1][1], m_Matrix[2][1], m_Matrix[3][1],
        m_Matrix[0][2], m_Matrix[1][2], m_Matrix[2][2], m_Matrix[3][2],
        m_Matrix[0][3], m_Matrix[1][3], m_Matrix[2][3], m_Matrix[3][3]);
}

inline matrix4 matrix4::lookAtLH(const vec3& eye, const vec3& at, const vec3& up)
{
    const vec3 z = (at - eye).normalized();
    const vec3 x = cross(up, z).normalized();
    const vec3 y = cross(z, x);

    return matrix4(
                x.x,         x.y,         x.z, 0.0f,
                y.x,         y.y,         y.z, 0.0f,
                z.x,         z.y,         z.z, 0.0f,
        -eye.dot(x), -eye.dot(y), -eye.dot(z), 1.0f);
}

inline matrix4 matrix4::perspectiveFovLH(float fovy, float aspect, float n, float f)
{
    const float h = 1.0f / tanf(fovy / 2.0f);
    const float w = h / aspect;

    return matrix4(
            w, 0.0f,              0.0f, 0.0f,
         0.0f,    h,              0.0f, 0.0f,
         0.0f, 0.0f,       f / (f - n), 1.0f,
         0.0f, 0.0f, (n * f) / (n - f), 0.0f);
}

inline matrix4 matrix4::rotationYawPitchRoll(float yaw, float pitch, float roll)
{
    const float xs = sinf(pitch);
    const float xc = cosf(pitch);
    const float ys = sinf(yaw);
    const float yc = cosf(yaw);
    const float zs = sinf(roll);
    const float zc = cosf(roll);

    return matrix4(
        yc * zc + ys * xs * zs, xc * zs, yc * xs * zs - ys * zc, 0.0f,
        ys * xs * zc - yc * zs, xc * zc, yc * xs * zc + ys * zs, 0.0f,
                       ys * xc,     -xs,                yc * xc, 0.0f,
                          0.0f,    0.0f,                   0.0f, 1.0f);
}

inline matrix4 matrix4::translation(float x, float y, float z)
{
    return matrix4(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
           x,    y,    z, 1.0f);
}

inline vec3 cross(const vec3& lhs, const vec3& rhs)
{
    return vec3(
        lhs.y * rhs.z - lhs.z * rhs.y,
        lhs.z * rhs.x - lhs.x * rhs.z,
        lhs.x * rhs.y - lhs.y * rhs.x);
}

inline vec3 project(const vec3& v, float viewX, float viewY, float viewW, float viewH, float minZ, float maxZ, const matrix4& transformation)
{
    const auto& m = transformation;

    auto x = m[0] * v.x + m[4] * v.y + m[ 8] * v.z + m[12] * 1.0f;
    auto y = m[1] * v.x + m[5] * v.y + m[ 9] * v.z + m[13] * 1.0f;
    auto z = m[2] * v.x + m[6] * v.y + m[10] * v.z + m[14] * 1.0f;
    auto w = m[3] * v.x + m[7] * v.y + m[11] * v.z + m[15] * 1.0f;

    auto px = (( x / w * 0.5f) + 0.5f) * viewW + viewX;
    auto py = ((-y / w * 0.5f) + 0.5f) * viewH + viewY;
    auto pz = (z / w) * (maxZ - minZ) + minZ;

    return vec3(px, py, pz);
}