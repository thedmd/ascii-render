#pragma once
#include "math.h"

inline vec2::vec2() = default;
inline vec2::vec2(float x, float y): x(x), y(y) {}

inline vec2 vec2::operator + (const vec2& v) const { return vec2(x + v.x, y + v.y); }
inline vec2 vec2::operator - (const vec2& v) const { return vec2(x - v.x, y - v.y); }

inline vec3::vec3() = default;
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

inline vec4::vec4() = default;
inline vec4::vec4(const vec3& v, float w): x(v.x), y(v.y), z(v.z), w(w) {}
inline vec4::vec4(float x, float y, float z, float w): x(x), y(y), z(z), w(w) {}

inline vec4 vec4::operator + (const vec4& v) const { return vec4(x + v.x, y + v.y, z + v.z, w + v.w); }
inline vec4 vec4::operator - (const vec4& v) const { return vec4(x - v.x, y - v.y, z - v.z, w - v.w); }

inline vec4 operator * (const vec4& v, float s) { return vec4(v.x * s, v.y * s, v.z * s, v.w * s); }
inline vec4 operator * (float s, const vec4& v) { return vec4(v.x * s, v.y * s, v.z * s, v.w * s); }

inline vec4 vec4::transformed(const matrix4& m) const
{
    return vec4(
        (m[0] * x + m[4] * y + m[ 8] * z + m[12] * w),
        (m[1] * x + m[5] * y + m[ 9] * z + m[13] * w),
        (m[2] * x + m[6] * y + m[10] * z + m[14] * w),
        (m[3] * x + m[7] * y + m[11] * z + m[15] * w)
    );
}

inline matrix4::matrix4() = default;
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

inline matrix4 matrix4::inverted() const
{
    // 214 multiplications
    //  80 adds/subs
    //   1 division
    const auto d00 = m_Matrix[1][1] * m_Matrix[2][2] * m_Matrix[3][3] + m_Matrix[2][1] * m_Matrix[3][2] * m_Matrix[1][3] + m_Matrix[3][1] * m_Matrix[1][2] * m_Matrix[2][3] - m_Matrix[1][3] * m_Matrix[2][2] * m_Matrix[3][1] - m_Matrix[2][3] * m_Matrix[3][2] * m_Matrix[1][1] - m_Matrix[3][3] * m_Matrix[1][2] * m_Matrix[2][1];
    const auto d01 = m_Matrix[0][1] * m_Matrix[2][2] * m_Matrix[3][3] + m_Matrix[2][1] * m_Matrix[3][2] * m_Matrix[0][3] + m_Matrix[3][1] * m_Matrix[0][2] * m_Matrix[2][3] - m_Matrix[0][3] * m_Matrix[2][2] * m_Matrix[3][1] - m_Matrix[2][3] * m_Matrix[3][2] * m_Matrix[0][1] - m_Matrix[3][3] * m_Matrix[0][2] * m_Matrix[2][1];
    const auto d02 = m_Matrix[0][1] * m_Matrix[1][2] * m_Matrix[3][3] + m_Matrix[1][1] * m_Matrix[3][2] * m_Matrix[0][3] + m_Matrix[3][1] * m_Matrix[0][2] * m_Matrix[1][3] - m_Matrix[0][3] * m_Matrix[1][2] * m_Matrix[3][1] - m_Matrix[1][3] * m_Matrix[3][2] * m_Matrix[0][1] - m_Matrix[3][3] * m_Matrix[0][2] * m_Matrix[1][1];
    const auto d03 = m_Matrix[0][1] * m_Matrix[1][2] * m_Matrix[2][3] + m_Matrix[1][1] * m_Matrix[2][2] * m_Matrix[0][3] + m_Matrix[2][1] * m_Matrix[0][2] * m_Matrix[1][3] - m_Matrix[0][3] * m_Matrix[1][2] * m_Matrix[2][1] - m_Matrix[1][3] * m_Matrix[2][2] * m_Matrix[0][1] - m_Matrix[2][3] * m_Matrix[0][2] * m_Matrix[1][1];

    const auto d10 = m_Matrix[1][0] * m_Matrix[2][2] * m_Matrix[3][3] + m_Matrix[2][0] * m_Matrix[3][2] * m_Matrix[1][3] + m_Matrix[3][0] * m_Matrix[1][2] * m_Matrix[2][3] - m_Matrix[1][3] * m_Matrix[2][2] * m_Matrix[3][0] - m_Matrix[2][3] * m_Matrix[3][2] * m_Matrix[1][0] - m_Matrix[3][3] * m_Matrix[1][2] * m_Matrix[2][0];
    const auto d11 = m_Matrix[0][0] * m_Matrix[2][2] * m_Matrix[3][3] + m_Matrix[2][0] * m_Matrix[3][2] * m_Matrix[0][3] + m_Matrix[3][0] * m_Matrix[0][2] * m_Matrix[2][3] - m_Matrix[0][3] * m_Matrix[2][2] * m_Matrix[3][0] - m_Matrix[2][3] * m_Matrix[3][2] * m_Matrix[0][0] - m_Matrix[3][3] * m_Matrix[0][2] * m_Matrix[2][0];
    const auto d12 = m_Matrix[0][0] * m_Matrix[1][2] * m_Matrix[3][3] + m_Matrix[1][0] * m_Matrix[3][2] * m_Matrix[0][3] + m_Matrix[3][0] * m_Matrix[0][2] * m_Matrix[1][3] - m_Matrix[0][3] * m_Matrix[1][2] * m_Matrix[3][0] - m_Matrix[1][3] * m_Matrix[3][2] * m_Matrix[0][0] - m_Matrix[3][3] * m_Matrix[0][2] * m_Matrix[1][0];
    const auto d13 = m_Matrix[0][0] * m_Matrix[1][2] * m_Matrix[2][3] + m_Matrix[1][0] * m_Matrix[2][2] * m_Matrix[0][3] + m_Matrix[2][0] * m_Matrix[0][2] * m_Matrix[1][3] - m_Matrix[0][3] * m_Matrix[1][2] * m_Matrix[2][0] - m_Matrix[1][3] * m_Matrix[2][2] * m_Matrix[0][0] - m_Matrix[2][3] * m_Matrix[0][2] * m_Matrix[1][0];

    const auto d20 = m_Matrix[1][0] * m_Matrix[2][1] * m_Matrix[3][3] + m_Matrix[2][0] * m_Matrix[3][1] * m_Matrix[1][3] + m_Matrix[3][0] * m_Matrix[1][1] * m_Matrix[2][3] - m_Matrix[1][3] * m_Matrix[2][1] * m_Matrix[3][0] - m_Matrix[2][3] * m_Matrix[3][1] * m_Matrix[1][0] - m_Matrix[3][3] * m_Matrix[1][1] * m_Matrix[2][0];
    const auto d21 = m_Matrix[0][0] * m_Matrix[2][1] * m_Matrix[3][3] + m_Matrix[2][0] * m_Matrix[3][1] * m_Matrix[0][3] + m_Matrix[3][0] * m_Matrix[0][1] * m_Matrix[2][3] - m_Matrix[0][3] * m_Matrix[2][1] * m_Matrix[3][0] - m_Matrix[2][3] * m_Matrix[3][1] * m_Matrix[0][0] - m_Matrix[3][3] * m_Matrix[0][1] * m_Matrix[2][0];
    const auto d22 = m_Matrix[0][0] * m_Matrix[1][1] * m_Matrix[3][3] + m_Matrix[1][0] * m_Matrix[3][1] * m_Matrix[0][3] + m_Matrix[3][0] * m_Matrix[0][1] * m_Matrix[1][3] - m_Matrix[0][3] * m_Matrix[1][1] * m_Matrix[3][0] - m_Matrix[1][3] * m_Matrix[3][1] * m_Matrix[0][0] - m_Matrix[3][3] * m_Matrix[0][1] * m_Matrix[1][0];
    const auto d23 = m_Matrix[0][0] * m_Matrix[1][1] * m_Matrix[2][3] + m_Matrix[1][0] * m_Matrix[2][1] * m_Matrix[0][3] + m_Matrix[2][0] * m_Matrix[0][1] * m_Matrix[1][3] - m_Matrix[0][3] * m_Matrix[1][1] * m_Matrix[2][0] - m_Matrix[1][3] * m_Matrix[2][1] * m_Matrix[0][0] - m_Matrix[2][3] * m_Matrix[0][1] * m_Matrix[1][0];

    const auto d30 = m_Matrix[1][0] * m_Matrix[2][1] * m_Matrix[3][2] + m_Matrix[2][0] * m_Matrix[3][1] * m_Matrix[1][2] + m_Matrix[3][0] * m_Matrix[1][1] * m_Matrix[2][2] - m_Matrix[1][2] * m_Matrix[2][1] * m_Matrix[3][0] - m_Matrix[2][2] * m_Matrix[3][1] * m_Matrix[1][0] - m_Matrix[3][2] * m_Matrix[1][1] * m_Matrix[2][0];
    const auto d31 = m_Matrix[0][0] * m_Matrix[2][1] * m_Matrix[3][2] + m_Matrix[2][0] * m_Matrix[3][1] * m_Matrix[0][2] + m_Matrix[3][0] * m_Matrix[0][1] * m_Matrix[2][2] - m_Matrix[0][2] * m_Matrix[2][1] * m_Matrix[3][0] - m_Matrix[2][2] * m_Matrix[3][1] * m_Matrix[0][0] - m_Matrix[3][2] * m_Matrix[0][1] * m_Matrix[2][0];
    const auto d32 = m_Matrix[0][0] * m_Matrix[1][1] * m_Matrix[3][2] + m_Matrix[1][0] * m_Matrix[3][1] * m_Matrix[0][2] + m_Matrix[3][0] * m_Matrix[0][1] * m_Matrix[1][2] - m_Matrix[0][2] * m_Matrix[1][1] * m_Matrix[3][0] - m_Matrix[1][2] * m_Matrix[3][1] * m_Matrix[0][0] - m_Matrix[3][2] * m_Matrix[0][1] * m_Matrix[1][0];
    const auto d33 = m_Matrix[0][0] * m_Matrix[1][1] * m_Matrix[2][2] + m_Matrix[1][0] * m_Matrix[2][1] * m_Matrix[0][2] + m_Matrix[2][0] * m_Matrix[0][1] * m_Matrix[1][2] - m_Matrix[0][2] * m_Matrix[1][1] * m_Matrix[2][0] - m_Matrix[1][2] * m_Matrix[2][1] * m_Matrix[0][0] - m_Matrix[2][2] * m_Matrix[0][1] * m_Matrix[1][0];

    const auto det = m_Matrix[0][0] * d00 - m_Matrix[1][0] * d01 + m_Matrix[2][0] * d02 - m_Matrix[3][0] * d03;

    if (det == 0.0f)
        return {};

    const auto invDet = 1.0f / det;

    return
    {
         d00 * invDet, -d01 * invDet,  d02 * invDet, -d03 * invDet,
        -d10 * invDet,  d11 * invDet, -d12 * invDet,  d13 * invDet,
         d20 * invDet, -d21 * invDet,  d22 * invDet, -d23 * invDet,
        -d30 * invDet,  d31 * invDet, -d32 * invDet,  d33 * invDet,
    };
}

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

inline matrix4 matrix4::scale(float x, float y, float z)
{
    return matrix4(
           x, 0.0f, 0.0f, 0.0f,
        0.0f,    y, 0.0f, 0.0f,
        0.0f, 0.0f,    z, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f);
}

inline matrix4 matrix4::translation(float x, float y, float z)
{
    return matrix4(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
           x,    y,    z, 1.0f);
}

inline matrix4 matrix4::clip(float x, float y, float w, float h, float n, float f)
{
    return matrix4(
                    1.0f / w,                0.0f,           0.0f, 0.0f,
                        0.0f,            1.0f / h,           0.0f, 0.0f,
                        0.0f,                0.0f, 1.0f / (f - n), 0.0f,
        -1.0f - 2.0f * x / w, 1.0f - 2.0f * y / h,   -n / (f - n), 1.0f);
}

inline float cross(const vec2& lhs, const vec2& rhs)
{
    return lhs.x * rhs.y - lhs.y * rhs.x;
}

inline vec3 cross(const vec3& lhs, const vec3& rhs)
{
    return vec3(
        lhs.y * rhs.z - lhs.z * rhs.y,
        lhs.z * rhs.x - lhs.x * rhs.z,
        lhs.x * rhs.y - lhs.y * rhs.x);
}

inline vec4 project(const vec4& v, const viewport_t& viewport)
{
    const auto w = v.w != 1.0f ? (v.w ? (1.0f / v.w) : 0.0f) : 1.0f;

    return
    {
        (( v.x * w * 0.5f) + 0.5f) * viewport.width  + viewport.x,
        ((-v.y * w * 0.5f) + 0.5f) * viewport.height + viewport.y,
           v.z * w * (viewport.maxZ - viewport.minZ) + viewport.minZ,
           1.0f
    };
}

inline vec4 unproject(const vec4& v, const viewport_t& viewport)
{
    const auto w = v.w != 1.0f ? (v.w ? (1.0f / v.w) : 0.0f) : 1.0f;

    return
    {
        ((v.x - viewport.x) / viewport.width  - 0.5f) *  2.0f,
        ((v.y - viewport.y) / viewport.height - 0.5f) * -2.0f,
         (v.z - viewport.minZ)  / (viewport.maxZ - viewport.minZ),
           1.0f
    };
}

inline vec3 project(const vec3& v, float viewX, float viewY, float viewW, float viewH, float minZ, float maxZ, const matrix4& transformation)
{
    const auto p = v.transformed(transformation);

    return
    {
        (( p.x * 0.5f) + 0.5f) * viewW + viewX,
        ((-p.y * 0.5f) + 0.5f) * viewH + viewY,
           p.z * (maxZ - minZ) + minZ
    };
}

inline vec3 unproject(const vec3& v, float viewX, float viewY, float viewW, float viewH, float minZ, float maxZ, const matrix4& inverseTransformation)
{
    auto p = vec3
    {
        ((v.x - viewX) / viewW - 0.5f) *  2.0f,
        ((v.y - viewY) / viewH - 0.5f) * -2.0f,
         (v.z - minZ)  / (maxZ - minZ)
    };

    return p.transformed(inverseTransformation);
}