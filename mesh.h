#pragma once
#include "math.h"
#include <vector>
#include <cstdint>

struct vertex_t
{
    vec3  p;
    vec3  n;
    float c;
};

typedef uint16_t index_t;

struct mesh_t
{
    std::vector<vertex_t> vertices;
    std::vector<index_t>  indices;
};

mesh_t make_box(float w, float h, float d);
mesh_t make_torus(float radius1, float radius2, int segments, int sides);
mesh_t make_teapot(float size, int divs);