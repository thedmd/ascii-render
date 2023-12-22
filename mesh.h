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

enum class primitive_type_t
{
    triangle_list,
    //triangle_strip,
    line_list,
    //line_strip,
    //point_list
};

struct mesh_t
{
    primitive_type_t      primitive_type;
    std::vector<vertex_t> vertices;
    std::vector<index_t>  indices;
};

mesh_t make_box(float w, float h, float d);
mesh_t make_torus(float radius1, float radius2, int segments, int sides);
mesh_t make_teapot(float size, int divs);
mesh_t make_line(float x0, float y0, float z0, float x1, float y1, float z1);
mesh_t make_normals(const mesh_t& mesh, float length);