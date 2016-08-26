#pragma once
#include <vector>
#include <cstdint>
#include "font.h"

struct framebuffer_t
{
    std::vector<float>  depth;
    int                 width;
    int                 height;

    framebuffer_t(int width, int height):
        depth(width * height),
        width(width),
        height(height)
    {
    }

    virtual ~framebuffer_t()
    {
    }

    void clear(float c)
    {
        clear_color(c);
    }

    void clear(float c, float d)
    {
        depth.assign(depth.size(), d);

        clear_color(c);
    }

    void set(int x, int y, float c)
    {
        set_color(x, y, c);
    }

    void set(int x, int y, float z, float c)
    {
        auto& d = depth[width * y + x];
        if (z > d)
            return;

        d = z;

        if (c > 1.0f)
            c = 1.0f;
        else if (c < 0.0f)
            c = 0.0f;

        set_color(x, y, c);
    }

protected:
    virtual void clear_color(float c) = 0;
    virtual void set_color(int x, int y, float c) = 0;
};

void fill_rect_2d(framebuffer_t& buffer, int x0, int y0, int x1, int y1, float color);
void circle_2d(framebuffer_t& buffer, int cx, int cy, int radius, float color);
void ellipse_2d(framebuffer_t& buffer, int cx, int cy, int rx, int ry, float color);
void line_2d(framebuffer_t& buffer, int x0, int y0, int x1, int y1, float color);
void hline_2d(framebuffer_t& buffer, int x1, int y, int x2, float c);
void vline_2d(framebuffer_t& buffer, int x, int y1, int y2, float c);
void triangle_2d(framebuffer_t& buffer, int x0, int y0, int x1, int y1, int x2, int y2, float color);
void char_2d(framebuffer_t& buffer, const font_t& font, int x, int y, char c, float color);

void triangle_3d(framebuffer_t& buffer,
    float x0, float y0, float z0,
    float x1, float y1, float z1,
    float x2, float y2, float z2,
    float c0, float c1, float c2);