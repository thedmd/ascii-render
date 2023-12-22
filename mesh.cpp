#define _USE_MATH_DEFINES
#include "mesh.h"

// http://wiki.unity3d.com/index.php/ProceduralPrimitives
mesh_t make_box(float w, float h, float d)
{
    const float hw = w * 0.5f;
    const float hh = h * 0.5f;
    const float hd = w * 0.5f;

    const auto p0 = vec3(-hw, -hh,  hd);
    const auto p1 = vec3( hw, -hh,  hd);
    const auto p2 = vec3( hw, -hh, -hd);
    const auto p3 = vec3(-hw, -hh, -hd);
    const auto p4 = vec3(-hw,  hh,  hd);
    const auto p5 = vec3( hw,  hh,  hd);
    const auto p6 = vec3( hw,  hh, -hd);
    const auto p7 = vec3(-hw,  hh, -hd);

    return mesh_t
    {
        primitive_type_t::triangle_list,
        {
            // bottom
            { p0, vec3( 0, -1,  0), 1.0f },
            { p1, vec3( 0, -1,  0), 1.0f },
            { p2, vec3( 0, -1,  0), 1.0f },
            { p3, vec3( 0, -1,  0), 1.0f },

            // left
            { p7, vec3(-1,  0,  0), 1.0f },
            { p4, vec3(-1,  0,  0), 1.0f },
            { p0, vec3(-1,  0,  0), 1.0f },
            { p3, vec3(-1,  0,  0), 1.0f },

            // front
            { p4, vec3( 0,  0,  1), 1.0f },
            { p5, vec3( 0,  0,  1), 1.0f },
            { p1, vec3( 0,  0,  1), 1.0f },
            { p0, vec3( 0,  0,  1), 1.0f },

            // back
            { p6, vec3( 0,  0, -1), 1.0f },
            { p7, vec3( 0,  0, -1), 1.0f },
            { p3, vec3( 0,  0, -1), 1.0f },
            { p2, vec3( 0,  0, -1), 1.0f },

            // right
            { p5, vec3( 1,  0,  0), 1.0f },
            { p6, vec3( 1,  0,  0), 1.0f },
            { p2, vec3( 1,  0,  0), 1.0f },
            { p1, vec3( 1,  0,  0), 1.0f },

            // top
            { p7, vec3( 0,  1,  0), 1.0f },
            { p6, vec3( 0,  1,  0), 1.0f },
            { p5, vec3( 0,  1,  0), 1.0f },
            { p4, vec3( 0,  1,  0), 1.0f },
        },
        {
            // Bottom
            3, 1, 0,
            3, 2, 1,

            // Left
            3 + 4 * 1, 1 + 4 * 1, 0 + 4 * 1,
            3 + 4 * 1, 2 + 4 * 1, 1 + 4 * 1,

            // Front
            3 + 4 * 2, 1 + 4 * 2, 0 + 4 * 2,
            3 + 4 * 2, 2 + 4 * 2, 1 + 4 * 2,

            // Back
            3 + 4 * 3, 1 + 4 * 3, 0 + 4 * 3,
            3 + 4 * 3, 2 + 4 * 3, 1 + 4 * 3,

            // Right
            3 + 4 * 4, 1 + 4 * 4, 0 + 4 * 4,
            3 + 4 * 4, 2 + 4 * 4, 1 + 4 * 4,

            // Top
            3 + 4 * 5, 1 + 4 * 5, 0 + 4 * 5,
            3 + 4 * 5, 2 + 4 * 5, 1 + 4 * 5,
        }
    };
}

// http://wiki.unity3d.com/index.php/ProceduralPrimitives
mesh_t make_torus(float radius1, float radius2, int segments, int sides)
{
    const auto _2pi = (float)(M_PI * 2);

    mesh_t result{ primitive_type_t::triangle_list };
    auto& vertices = result.vertices;
    auto& indices  = result.indices;

    vertices.resize((segments + 1) * (sides + 1), vertex_t{vec3(), vec3(), 1.0f});

    for (int seg = 0; seg <= segments; ++seg)
    {
        int currSeg = seg == segments ? 0 : seg;

        float t1 = (float)currSeg / segments * _2pi;
        float c1 = cosf(t1);
        float s1 = sinf(t1);
        vec3  r1 = vec3(c1 * radius1, s1 * radius1, 0.0f);

        for (int side = 0; side <= sides; side++)
        {
            int currSide = side == sides ? 0 : side;
            float t2 = (float)currSide / sides * _2pi;

            vec3 n2 = vec3(sinf(t2), 0.0f, cosf(t2));

            n2 = vec3(
                c1 * n2.x - s1 * n2.y,
                s1 * n2.x + c1 * n2.y,
                n2.z);

            vertices[side + seg * (sides + 1)].p = r1 + n2 * radius2;
            vertices[side + seg * (sides + 1)].n = n2.normalized();
        }
    }

    int nbFaces = (segments + 1) * (sides + 1);
    int nbTriangles = nbFaces * 2;
    int nbIndexes = nbTriangles * 3;

    indices.resize(nbIndexes);

    int i = 0;
    for (int seg = 0; seg <= segments; seg++)
    {
        for (int side = 0; side <= sides - 1; side++)
        {
            int current = side + seg * (sides + 1);
            int next = side + (seg < (segments) ? (seg + 1) * (sides + 1) : 0);

            if (i < nbIndexes - 6)
            {
                indices[i++] = current;
                indices[i++] = next + 1;
                indices[i++] = next;

                indices[i++] = current;
                indices[i++] = current + 1;
                indices[i++] = next + 1;
            }
        }
    }

    return result;
}

static const uint16_t kTeapotNumPatches = 32;
static const uint16_t kTeapotNumVertices = 306;
static const uint32_t kTeapotPatches[kTeapotNumPatches][16] =
{
    {   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,  15,  16 }, // rim
    {   4,  17,  18,  19,   8,  20,  21,  22,  12,  23,  24,  25,  16,  26,  27,  28 },
    {  19,  29,  30,  31,  22,  32,  33,  34,  25,  35,  36,  37,  28,  38,  39,  40 },
    {  31,  41,  42,   1,  34,  43,  44,   5,  37,  45,  46,   9,  40,  47,  48,  13 },
    {  13,  14,  15,  16,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60 }, // body
    {  16,  26,  27,  28,  52,  61,  62,  63,  56,  64,  65,  66,  60,  67,  68,  69 },
    {  28,  38,  39,  40,  63,  70,  71,  72,  66,  73,  74,  75,  69,  76,  77,  78 },
    {  40,  47,  48,  13,  72,  79,  80,  49,  75,  81,  82,  53,  78,  83,  84,  57 },
    {  57,  58,  59,  60,  85,  86,  87,  88,  89,  90,  91,  92,  93,  94,  95,  96 }, // body (bottom)
    {  60,  67,  68,  69,  88,  97,  98,  99,  92, 100, 101, 102,  96, 103, 104, 105 },
    {  69,  76,  77,  78,  99, 106, 107, 108, 102, 109, 110, 111, 105, 112, 113, 114 },
    {  78,  83,  84,  57, 108, 115, 116,  85, 111, 117, 118,  89, 114, 119, 120,  93 },
    { 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136 }, // handle
    { 124, 137, 138, 121, 128, 139, 140, 125, 132, 141, 142, 129, 136, 143, 144, 133 },
    { 133, 134, 135, 136, 145, 146, 147, 148, 149, 150, 151, 152,  69, 153, 154, 155 },
    { 136, 143, 144, 133, 148, 156, 157, 145, 152, 158, 159, 149, 155, 160, 161,  69 },
    { 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177 }, // spout
    { 165, 178, 179, 162, 169, 180, 181, 166, 173, 182, 183, 170, 177, 184, 185, 174 },
    { 174, 175, 176, 177, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197 },
    { 177, 184, 185, 174, 189, 198, 199, 186, 193, 200, 201, 190, 197, 202, 203, 194 },
    { 204, 204, 204, 204, 207, 208, 209, 210, 211, 211, 211, 211, 212, 213, 214, 215 }, // lid handle
    { 204, 204, 204, 204, 210, 217, 218, 219, 211, 211, 211, 211, 215, 220, 221, 222 },
    { 204, 204, 204, 204, 219, 224, 225, 226, 211, 211, 211, 211, 222, 227, 228, 229 },
    { 204, 204, 204, 204, 226, 230, 231, 207, 211, 211, 211, 211, 229, 232, 233, 212 },
    { 212, 213, 214, 215, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245 }, // lid body
    { 215, 220, 221, 222, 237, 246, 247, 248, 241, 249, 250, 251, 245, 252, 253, 254 },
    { 222, 227, 228, 229, 248, 255, 256, 257, 251, 258, 259, 260, 254, 261, 262, 263 },
    { 229, 232, 233, 212, 257, 264, 265, 234, 260, 266, 267, 238, 263, 268, 269, 242 },
    { 270, 270, 270, 270, 279, 280, 281, 282, 275, 276, 277, 278, 271, 272, 273, 274 }, // bottom
    { 270, 270, 270, 270, 282, 289, 290, 291, 278, 286, 287, 288, 274, 283, 284, 285 },
    { 270, 270, 270, 270, 291, 298, 299, 300, 288, 295, 296, 297, 285, 292, 293, 294 },
    { 270, 270, 270, 270, 300, 305, 306, 279, 297, 303, 304, 275, 294, 301, 302, 271 }
};

static const float kTeapotVertices[kTeapotNumVertices][3] =
{
    {  1.4000f,  0.0000f,  2.4000f }, {  1.4000f, -0.7840f,  2.4000f }, {  0.7840f, -1.4000f,  2.4000f }, {  0.0000f, -1.4000f,  2.4000f },
    {  1.3375f,  0.0000f,  2.5312f }, {  1.3375f, -0.7490f,  2.5312f }, {  0.7490f, -1.3375f,  2.5312f }, {  0.0000f, -1.3375f,  2.5312f },
    {  1.4375f,  0.0000f,  2.5312f }, {  1.4375f, -0.8050f,  2.5312f }, {  0.8050f, -1.4375f,  2.5312f }, {  0.0000f, -1.4375f,  2.5312f },
    {  1.5000f,  0.0000f,  2.4000f }, {  1.5000f, -0.8400f,  2.4000f }, {  0.8400f, -1.5000f,  2.4000f }, {  0.0000f, -1.5000f,  2.4000f },
    { -0.7840f, -1.4000f,  2.4000f }, { -1.4000f, -0.7840f,  2.4000f }, { -1.4000f,  0.0000f,  2.4000f }, { -0.7490f, -1.3375f,  2.5312f },
    { -1.3375f, -0.7490f,  2.5312f }, { -1.3375f,  0.0000f,  2.5312f }, { -0.8050f, -1.4375f,  2.5312f }, { -1.4375f, -0.8050f,  2.5312f },
    { -1.4375f,  0.0000f,  2.5312f }, { -0.8400f, -1.5000f,  2.4000f }, { -1.5000f, -0.8400f,  2.4000f }, { -1.5000f,  0.0000f,  2.4000f },
    { -1.4000f,  0.7840f,  2.4000f }, { -0.7840f,  1.4000f,  2.4000f }, {  0.0000f,  1.4000f,  2.4000f }, { -1.3375f,  0.7490f,  2.5312f },
    { -0.7490f,  1.3375f,  2.5312f }, {  0.0000f,  1.3375f,  2.5312f }, { -1.4375f,  0.8050f,  2.5312f }, { -0.8050f,  1.4375f,  2.5312f },
    {  0.0000f,  1.4375f,  2.5312f }, { -1.5000f,  0.8400f,  2.4000f }, { -0.8400f,  1.5000f,  2.4000f }, {  0.0000f,  1.5000f,  2.4000f },
    {  0.7840f,  1.4000f,  2.4000f }, {  1.4000f,  0.7840f,  2.4000f }, {  0.7490f,  1.3375f,  2.5312f }, {  1.3375f,  0.7490f,  2.5312f },
    {  0.8050f,  1.4375f,  2.5312f }, {  1.4375f,  0.8050f,  2.5312f }, {  0.8400f,  1.5000f,  2.4000f }, {  1.5000f,  0.8400f,  2.4000f },
    {  1.7500f,  0.0000f,  1.8750f }, {  1.7500f, -0.9800f,  1.8750f }, {  0.9800f, -1.7500f,  1.8750f }, {  0.0000f, -1.7500f,  1.8750f },
    {  2.0000f,  0.0000f,  1.3500f }, {  2.0000f, -1.1200f,  1.3500f }, {  1.1200f, -2.0000f,  1.3500f }, {  0.0000f, -2.0000f,  1.3500f },
    {  2.0000f,  0.0000f,  0.9000f }, {  2.0000f, -1.1200f,  0.9000f }, {  1.1200f, -2.0000f,  0.9000f }, {  0.0000f, -2.0000f,  0.9000f },
    { -0.9800f, -1.7500f,  1.8750f }, { -1.7500f, -0.9800f,  1.8750f }, { -1.7500f,  0.0000f,  1.8750f }, { -1.1200f, -2.0000f,  1.3500f },
    { -2.0000f, -1.1200f,  1.3500f }, { -2.0000f,  0.0000f,  1.3500f }, { -1.1200f, -2.0000f,  0.9000f }, { -2.0000f, -1.1200f,  0.9000f },
    { -2.0000f,  0.0000f,  0.9000f }, { -1.7500f,  0.9800f,  1.8750f }, { -0.9800f,  1.7500f,  1.8750f }, {  0.0000f,  1.7500f,  1.8750f },
    { -2.0000f,  1.1200f,  1.3500f }, { -1.1200f,  2.0000f,  1.3500f }, {  0.0000f,  2.0000f,  1.3500f }, { -2.0000f,  1.1200f,  0.9000f },
    { -1.1200f,  2.0000f,  0.9000f }, {  0.0000f,  2.0000f,  0.9000f }, {  0.9800f,  1.7500f,  1.8750f }, {  1.7500f,  0.9800f,  1.8750f },
    {  1.1200f,  2.0000f,  1.3500f }, {  2.0000f,  1.1200f,  1.3500f }, {  1.1200f,  2.0000f,  0.9000f }, {  2.0000f,  1.1200f,  0.9000f },
    {  2.0000f,  0.0000f,  0.4500f }, {  2.0000f, -1.1200f,  0.4500f }, {  1.1200f, -2.0000f,  0.4500f }, {  0.0000f, -2.0000f,  0.4500f },
    {  1.5000f,  0.0000f,  0.2250f }, {  1.5000f, -0.8400f,  0.2250f }, {  0.8400f, -1.5000f,  0.2250f }, {  0.0000f, -1.5000f,  0.2250f },
    {  1.5000f,  0.0000f,  0.1500f }, {  1.5000f, -0.8400f,  0.1500f }, {  0.8400f, -1.5000f,  0.1500f }, {  0.0000f, -1.5000f,  0.1500f },
    { -1.1200f, -2.0000f,  0.4500f }, { -2.0000f, -1.1200f,  0.4500f }, { -2.0000f,  0.0000f,  0.4500f }, { -0.8400f, -1.5000f,  0.2250f },
    { -1.5000f, -0.8400f,  0.2250f }, { -1.5000f,  0.0000f,  0.2250f }, { -0.8400f, -1.5000f,  0.1500f }, { -1.5000f, -0.8400f,  0.1500f },
    { -1.5000f,  0.0000f,  0.1500f }, { -2.0000f,  1.1200f,  0.4500f }, { -1.1200f,  2.0000f,  0.4500f }, {  0.0000f,  2.0000f,  0.4500f },
    { -1.5000f,  0.8400f,  0.2250f }, { -0.8400f,  1.5000f,  0.2250f }, {  0.0000f,  1.5000f,  0.2250f }, { -1.5000f,  0.8400f,  0.1500f },
    { -0.8400f,  1.5000f,  0.1500f }, {  0.0000f,  1.5000f,  0.1500f }, {  1.1200f,  2.0000f,  0.4500f }, {  2.0000f,  1.1200f,  0.4500f },
    {  0.8400f,  1.5000f,  0.2250f }, {  1.5000f,  0.8400f,  0.2250f }, {  0.8400f,  1.5000f,  0.1500f }, {  1.5000f,  0.8400f,  0.1500f },
    { -1.6000f,  0.0000f,  2.0250f }, { -1.6000f, -0.3000f,  2.0250f }, { -1.5000f, -0.3000f,  2.2500f }, { -1.5000f,  0.0000f,  2.2500f },
    { -2.3000f,  0.0000f,  2.0250f }, { -2.3000f, -0.3000f,  2.0250f }, { -2.5000f, -0.3000f,  2.2500f }, { -2.5000f,  0.0000f,  2.2500f },
    { -2.7000f,  0.0000f,  2.0250f }, { -2.7000f, -0.3000f,  2.0250f }, { -3.0000f, -0.3000f,  2.2500f }, { -3.0000f,  0.0000f,  2.2500f },
    { -2.7000f,  0.0000f,  1.8000f }, { -2.7000f, -0.3000f,  1.8000f }, { -3.0000f, -0.3000f,  1.8000f }, { -3.0000f,  0.0000f,  1.8000f },
    { -1.5000f,  0.3000f,  2.2500f }, { -1.6000f,  0.3000f,  2.0250f }, { -2.5000f,  0.3000f,  2.2500f }, { -2.3000f,  0.3000f,  2.0250f },
    { -3.0000f,  0.3000f,  2.2500f }, { -2.7000f,  0.3000f,  2.0250f }, { -3.0000f,  0.3000f,  1.8000f }, { -2.7000f,  0.3000f,  1.8000f },
    { -2.7000f,  0.0000f,  1.5750f }, { -2.7000f, -0.3000f,  1.5750f }, { -3.0000f, -0.3000f,  1.3500f }, { -3.0000f,  0.0000f,  1.3500f },
    { -2.5000f,  0.0000f,  1.1250f }, { -2.5000f, -0.3000f,  1.1250f }, { -2.6500f, -0.3000f,  0.9375f }, { -2.6500f,  0.0000f,  0.9375f },
    { -2.0000f, -0.3000f,  0.9000f }, { -1.9000f, -0.3000f,  0.6000f }, { -1.9000f,  0.0000f,  0.6000f }, { -3.0000f,  0.3000f,  1.3500f },
    { -2.7000f,  0.3000f,  1.5750f }, { -2.6500f,  0.3000f,  0.9375f }, { -2.5000f,  0.3000f,  1.1250f }, { -1.9000f,  0.3000f,  0.6000f },
    { -2.0000f,  0.3000f,  0.9000f }, {  1.7000f,  0.0000f,  1.4250f }, {  1.7000f, -0.6600f,  1.4250f }, {  1.7000f, -0.6600f,  0.6000f },
    {  1.7000f,  0.0000f,  0.6000f }, {  2.6000f,  0.0000f,  1.4250f }, {  2.6000f, -0.6600f,  1.4250f }, {  3.1000f, -0.6600f,  0.8250f },
    {  3.1000f,  0.0000f,  0.8250f }, {  2.3000f,  0.0000f,  2.1000f }, {  2.3000f, -0.2500f,  2.1000f }, {  2.4000f, -0.2500f,  2.0250f },
    {  2.4000f,  0.0000f,  2.0250f }, {  2.7000f,  0.0000f,  2.4000f }, {  2.7000f, -0.2500f,  2.4000f }, {  3.3000f, -0.2500f,  2.4000f },
    {  3.3000f,  0.0000f,  2.4000f }, {  1.7000f,  0.6600f,  0.6000f }, {  1.7000f,  0.6600f,  1.4250f }, {  3.1000f,  0.6600f,  0.8250f },
    {  2.6000f,  0.6600f,  1.4250f }, {  2.4000f,  0.2500f,  2.0250f }, {  2.3000f,  0.2500f,  2.1000f }, {  3.3000f,  0.2500f,  2.4000f },
    {  2.7000f,  0.2500f,  2.4000f }, {  2.8000f,  0.0000f,  2.4750f }, {  2.8000f, -0.2500f,  2.4750f }, {  3.5250f, -0.2500f,  2.4938f },
    {  3.5250f,  0.0000f,  2.4938f }, {  2.9000f,  0.0000f,  2.4750f }, {  2.9000f, -0.1500f,  2.4750f }, {  3.4500f, -0.1500f,  2.5125f },
    {  3.4500f,  0.0000f,  2.5125f }, {  2.8000f,  0.0000f,  2.4000f }, {  2.8000f, -0.1500f,  2.4000f }, {  3.2000f, -0.1500f,  2.4000f },
    {  3.2000f,  0.0000f,  2.4000f }, {  3.5250f,  0.2500f,  2.4938f }, {  2.8000f,  0.2500f,  2.4750f }, {  3.4500f,  0.1500f,  2.5125f },
    {  2.9000f,  0.1500f,  2.4750f }, {  3.2000f,  0.1500f,  2.4000f }, {  2.8000f,  0.1500f,  2.4000f }, {  0.0000f,  0.0000f,  3.1500f },
    {  0.0000f, -0.0020f,  3.1500f }, {  0.0020f,  0.0000f,  3.1500f }, {  0.8000f,  0.0000f,  3.1500f }, {  0.8000f, -0.4500f,  3.1500f },
    {  0.4500f, -0.8000f,  3.1500f }, {  0.0000f, -0.8000f,  3.1500f }, {  0.0000f,  0.0000f,  2.8500f }, {  0.2000f,  0.0000f,  2.7000f },
    {  0.2000f, -0.1120f,  2.7000f }, {  0.1120f, -0.2000f,  2.7000f }, {  0.0000f, -0.2000f,  2.7000f }, { -0.0020f,  0.0000f,  3.1500f },
    { -0.4500f, -0.8000f,  3.1500f }, { -0.8000f, -0.4500f,  3.1500f }, { -0.8000f,  0.0000f,  3.1500f }, { -0.1120f, -0.2000f,  2.7000f },
    { -0.2000f, -0.1120f,  2.7000f }, { -0.2000f,  0.0000f,  2.7000f }, {  0.0000f,  0.0020f,  3.1500f }, { -0.8000f,  0.4500f,  3.1500f },
    { -0.4500f,  0.8000f,  3.1500f }, {  0.0000f,  0.8000f,  3.1500f }, { -0.2000f,  0.1120f,  2.7000f }, { -0.1120f,  0.2000f,  2.7000f },
    {  0.0000f,  0.2000f,  2.7000f }, {  0.4500f,  0.8000f,  3.1500f }, {  0.8000f,  0.4500f,  3.1500f }, {  0.1120f,  0.2000f,  2.7000f },
    {  0.2000f,  0.1120f,  2.7000f }, {  0.4000f,  0.0000f,  2.5500f }, {  0.4000f, -0.2240f,  2.5500f }, {  0.2240f, -0.4000f,  2.5500f },
    {  0.0000f, -0.4000f,  2.5500f }, {  1.3000f,  0.0000f,  2.5500f }, {  1.3000f, -0.7280f,  2.5500f }, {  0.7280f, -1.3000f,  2.5500f },
    {  0.0000f, -1.3000f,  2.5500f }, {  1.3000f,  0.0000f,  2.4000f }, {  1.3000f, -0.7280f,  2.4000f }, {  0.7280f, -1.3000f,  2.4000f },
    {  0.0000f, -1.3000f,  2.4000f }, { -0.2240f, -0.4000f,  2.5500f }, { -0.4000f, -0.2240f,  2.5500f }, { -0.4000f,  0.0000f,  2.5500f },
    { -0.7280f, -1.3000f,  2.5500f }, { -1.3000f, -0.7280f,  2.5500f }, { -1.3000f,  0.0000f,  2.5500f }, { -0.7280f, -1.3000f,  2.4000f },
    { -1.3000f, -0.7280f,  2.4000f }, { -1.3000f,  0.0000f,  2.4000f }, { -0.4000f,  0.2240f,  2.5500f }, { -0.2240f,  0.4000f,  2.5500f },
    {  0.0000f,  0.4000f,  2.5500f }, { -1.3000f,  0.7280f,  2.5500f }, { -0.7280f,  1.3000f,  2.5500f }, {  0.0000f,  1.3000f,  2.5500f },
    { -1.3000f,  0.7280f,  2.4000f }, { -0.7280f,  1.3000f,  2.4000f }, {  0.0000f,  1.3000f,  2.4000f }, {  0.2240f,  0.4000f,  2.5500f },
    {  0.4000f,  0.2240f,  2.5500f }, {  0.7280f,  1.3000f,  2.5500f }, {  1.3000f,  0.7280f,  2.5500f }, {  0.7280f,  1.3000f,  2.4000f },
    {  1.3000f,  0.7280f,  2.4000f }, {  0.0000f,  0.0000f,  0.0000f }, {  1.5000f,  0.0000f,  0.1500f }, {  1.5000f,  0.8400f,  0.1500f },
    {  0.8400f,  1.5000f,  0.1500f }, {  0.0000f,  1.5000f,  0.1500f }, {  1.5000f,  0.0000f,  0.0750f }, {  1.5000f,  0.8400f,  0.0750f },
    {  0.8400f,  1.5000f,  0.0750f }, {  0.0000f,  1.5000f,  0.0750f }, {  1.4250f,  0.0000f,  0.0000f }, {  1.4250f,  0.7980f,  0.0000f },
    {  0.7980f,  1.4250f,  0.0000f }, {  0.0000f,  1.4250f,  0.0000f }, { -0.8400f,  1.5000f,  0.1500f }, { -1.5000f,  0.8400f,  0.1500f },
    { -1.5000f,  0.0000f,  0.1500f }, { -0.8400f,  1.5000f,  0.0750f }, { -1.5000f,  0.8400f,  0.0750f }, { -1.5000f,  0.0000f,  0.0750f },
    { -0.7980f,  1.4250f,  0.0000f }, { -1.4250f,  0.7980f,  0.0000f }, { -1.4250f,  0.0000f,  0.0000f }, { -1.5000f, -0.8400f,  0.1500f },
    { -0.8400f, -1.5000f,  0.1500f }, {  0.0000f, -1.5000f,  0.1500f }, { -1.5000f, -0.8400f,  0.0750f }, { -0.8400f, -1.5000f,  0.0750f },
    {  0.0000f, -1.5000f,  0.0750f }, { -1.4250f, -0.7980f,  0.0000f }, { -0.7980f, -1.4250f,  0.0000f }, {  0.0000f, -1.4250f,  0.0000f },
    {  0.8400f, -1.5000f,  0.1500f }, {  1.5000f, -0.8400f,  0.1500f }, {  0.8400f, -1.5000f,  0.0750f }, {  1.5000f, -0.8400f,  0.0750f },
    {  0.7980f, -1.4250f,  0.0000f }, {  1.4250f, -0.7980f,  0.0000f }
};

mesh_t make_teapot(float size, int divs)
{
    // http://www.scratchapixel.com/code.php?id=35&origin=/lessons/advanced-rendering/bezier-curve-rendering-utah-teapot

    auto evalBezierCurve = [](const vec3* P, float t) -> vec3
    {
        auto b0 = (1 - t) * (1 - t) * (1 - t);
        auto b1 = 3 * t * (1 - t) * (1 - t);
        auto b2 = 3 * t * t * (1 - t);
        auto b3 = t * t * t;

        return P[0] * b0 + P[1] * b1 + P[2] * b2 + P[3] * b3;
    };

    auto evalBezierPatch = [&evalBezierCurve](const vec3* controlPoints, float u, float v) -> vec3
    {
        vec3 uCurve[4];
        for (int i = 0; i < 4; ++i)
            uCurve[i] = evalBezierCurve(controlPoints + 4 * i, u);

        return evalBezierCurve(uCurve, v);
    };

    auto derivBezier = [](const vec3* P, float t) -> vec3
    {
        return -3 * (1 - t) * (1 - t) * P[0] +
            (3 * (1 - t) * (1 - t) - 6 * t * (1 - t)) * P[1] +
            (6 * t * (1 - t) - 3 * t * t) * P[2] +
            3 * t * t * P[3];
    };

    auto dUBezier = [&evalBezierCurve, &derivBezier](const vec3* controlPoints, float u, float v) -> vec3
    {
        vec3 P[4];
        vec3 vCurve[4];
        for (int i = 0; i < 4; ++i)
        {
            P[0] = controlPoints[i];
            P[1] = controlPoints[4 + i];
            P[2] = controlPoints[8 + i];
            P[3] = controlPoints[12 + i];
            vCurve[i] = evalBezierCurve(P, v);
        }

        return derivBezier(vCurve, u);
    };

    auto dVBezier = [&evalBezierCurve, &derivBezier](const vec3* controlPoints, float u, float v) -> vec3
    {
        vec3 uCurve[4];
        for (int i = 0; i < 4; ++i)
        {
            uCurve[i] = evalBezierCurve(controlPoints + 4 * i, u);
        }

        return derivBezier(uCurve, v);
    };


    const auto verticesInPatch = (divs + 1) * (divs + 1);

    mesh_t result{ primitive_type_t::triangle_list };
    result.indices.resize(divs * divs * 6 * kTeapotNumPatches, 0);// * kTeapotNumPatches);
    auto indices = result.indices.data();// + baseIndex;
    for (int np = 0; np < kTeapotNumPatches; ++np)
    {
        //int np = 5;
        int baseIndex = np * verticesInPatch;
        for (uint16_t j = 0; j < divs; ++j)
        {
            for (uint16_t i = 0; i < divs; ++i, indices += 6)
            {
                auto i0 = (divs + 1) *   j +      i;
                auto i1 = (divs + 1) *   j +      i + 1;
                auto i2 = (divs + 1) *  (j + 1) + i + 1;
                auto i3 = (divs + 1) *  (j + 1) + i;

                indices[0] = baseIndex + i0;
                indices[1] = baseIndex + i1;
                indices[2] = baseIndex + i2;
                indices[3] = baseIndex + i0;
                indices[4] = baseIndex + i2;
                indices[5] = baseIndex + i3;
            }
        }
    }

    result.vertices.resize(kTeapotNumPatches * verticesInPatch, vertex_t{ vec3(), vec3(), 1.0f });

    vec3 controlPoints[16];
    for (int np = 0; np < kTeapotNumPatches; ++np)
    {
        auto vertices = result.vertices.data() + np * verticesInPatch;

        // set the control points for the current patch
        for (uint32_t i = 0; i < 16; ++i)
        {
            controlPoints[i].x = kTeapotVertices[kTeapotPatches[np][i] - 1][0];
            controlPoints[i].y = kTeapotVertices[kTeapotPatches[np][i] - 1][1];
            controlPoints[i].z = kTeapotVertices[kTeapotPatches[np][i] - 1][2];
        }

        // generate grid
        for (uint16_t j = 0, k = 0; j <= divs; ++j)
        {
            float v = j / (float)divs;
            for (uint16_t i = 0; i <= divs; ++i, ++k)
            {
                float u = i / (float)divs;
                vertices[k].p = evalBezierPatch(controlPoints, u, v) * size;
                auto dU = dUBezier(controlPoints, u, v);
                auto dV = dVBezier(controlPoints, u, v);
                vertices[k].n = cross(dU, dV).normalized();
                //N[k] = dU.crossProduct(dV).normalize();
                //st[k].x = u;
                //st[k].y = v;

                if (np >= 20 && np < 24 && j == 0) // repair lid normals
                    vertices[k].n = { 0, 0, 1 };

                if (np >= 28 && np < 32 && j == 0) // repair bottom normals
                    vertices[k].n = { 0, 0, -1 };
            }
        }
    }

    return result;
}

mesh_t make_line(float x0, float y0, float z0, float x1, float y1, float z1)
{
    return mesh_t
    {
        primitive_type_t::line_list,
        {
            { { x0, y0, z0 }, vec3( 0, -1,  0), 1.0f },
            { { x1, y1, z1 }, vec3( 0, -1,  0), 1.0f },
        },
        {
            0, 1,
        }
    };
}

mesh_t make_normals(const mesh_t& mesh, float length)
{
    mesh_t result{ primitive_type_t::line_list };
    result.vertices.reserve(mesh.vertices.size() * 2);
    result.indices.reserve(mesh.vertices.size() * 2);

    for (auto& v : mesh.vertices)
    {
        result.vertices.push_back(v);
        result.vertices.push_back({ v.p + v.n * length, v.n, v.c });
    }

    for (size_t i = 0; i < mesh.vertices.size() * 2; ++i)
    {
        result.indices.push_back(static_cast<index_t>(i));
    }

    return result;
}