#pragma once
#include <cmath>

struct matrix4;

struct vec2
{
    float x{}, y{};

    vec2();
    vec2(float x, float y);

    vec2 operator + (const vec2& v) const;
    vec2 operator - (const vec2& v) const;
};


struct vec3
{
    float x{}, y{}, z{};

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

struct vec4
{
    float x{}, y{}, z{}, w{};

    vec4();
    vec4(const vec3& v, float w);
    vec4(float x, float y, float z, float w);

    vec4 operator + (const vec4& v) const;
    vec4 operator - (const vec4& v) const;

    friend vec3 operator * (const vec3& v, float s);
    friend vec3 operator * (float s, const vec3& v);

    vec2 xy() const { return vec2(x, y); }

    vec4 transformed(const matrix4& m) const;
};

struct matrix4
{
    float m_Matrix[4][4]{};

    matrix4();
    matrix4(float m11, float m12, float m13, float m14,
            float m21, float m22, float m23, float m24,
            float m31, float m32, float m33, float m34,
            float m41, float m42, float m43, float m44);

    matrix4 operator * (const matrix4& m) const;

          float& operator[](int index);
    const float& operator[](int index) const;

    inline matrix4 inverted() const;

    inline matrix4 transposed() const;

    static matrix4 lookAtLH(const vec3& eye, const vec3& at, const vec3& up);

    static matrix4 perspectiveFovLH(float fovy, float aspect, float n, float f);

    static matrix4 rotationYawPitchRoll(float yaw, float pitch, float roll);

    static matrix4 scale(float x, float y, float z);

    static matrix4 translation(float x, float y, float z);

    static matrix4 clip(float x, float y, float w, float h, float n, float f);

    static const matrix4 identity;
};

inline const matrix4 matrix4::identity
{
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
};

float cross(const vec2& lhs, const vec2& rhs);
vec3 cross(const vec3& lhs, const vec3& rhs);

struct viewport_t
{
    float x          =  0.0f;
    float y          =  0.0f;
    float width      =  0.0f;
    float height     =  0.0f;
    float minZ       =  0.0f;
    float maxZ       =  1.0f;
    float clipX      = -1.0f;
    float clipY      =  1.0f;
    float clipWidth  =  2.0f;
    float clipHeight =  2.0f;
};

inline vec4 project(const vec4& v, const viewport_t& viewport);
inline vec4 unproject(const vec4& v, const viewport_t& viewport);

inline vec3 project(const vec3& v, float viewX, float viewY, float viewW, float viewH, float minZ, float maxZ, const matrix4& transformation);
inline vec3 unproject(const vec3& v, float viewX, float viewY, float viewW, float viewH, float minZ, float maxZ, const matrix4& inverseTransformation);

#include "math.inl"