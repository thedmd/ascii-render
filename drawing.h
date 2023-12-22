#pragma once
#include <vector>
#include <cstdint>
#include "font.h"

struct image_t;
struct framebuffer_t;

void generic_fill_rect_2d(framebuffer_t& buffer, int x0, int y0, int x1, int y1, float color);
void generic_circle_2d(framebuffer_t& buffer, int cx, int cy, int radius, float color);
void generic_ellipse_2d(framebuffer_t& buffer, int cx, int cy, int rx, int ry, float color);
void generic_line_2d(framebuffer_t& buffer, int x0, int y0, int x1, int y1, float color);
void generic_hline_2d(framebuffer_t& buffer, int x1, int y, int x2, float c);
void generic_vline_2d(framebuffer_t& buffer, int x, int y1, int y2, float c);
void generic_triangle_2d(framebuffer_t& buffer, int x0, int y0, int x1, int y1, int x2, int y2, float color);
void generic_triangle_2d(framebuffer_t& buffer, const image_t& texture, float x0, float y0, float x1, float y1, float x2, float y2, float u0, float v0, float u1, float v1, float u2, float v2, float c0, float c1, float c2, float a0, float a1, float a2);
void generic_char_2d(framebuffer_t& buffer, const font_t& font, int x, int y, char c, float color);

void generic_triangle_3d(framebuffer_t& buffer,
    float x0, float y0, float z0,
    float x1, float y1, float z1,
    float x2, float y2, float z2,
    float c0, float c1, float c2);

void generic_line_3d(framebuffer_t& buffer,
    float x0, float y0, float z0,
    float x1, float y1, float z1,
    float c0, float c1);

struct image_t
{
    const uint8_t*  data;
    uint16_t        width;
    uint16_t        height;
    uint16_t        pitch;

    virtual ~image_t() {}
};

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

    virtual ~framebuffer_t() {}

    void clear(float c)
    {
        clear_color(c);
    }

    void clear(float c, float d)
    {
        depth.assign(depth.size(), d);

        clear_color(c);
    }

    void commit()
    {
        commit_impl();
    }

    void present()
    {
        present_impl();
    }

    void set(int x, int y, float c)
    {
        if (!clip(x, y))
            set_color(x, y, c);
    }

    void blend(int x, int y, float c, float a)
    {
        if (!clip(x, y))
            blend_color(x, y, c, a);
    }

    void set(int x, int y, float z, float c)
    {
        if (clip(x, y))
            return;

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

    virtual void fill_rect_2d(int x0, int y0, int x1, int y1, float color)
    {
        generic_fill_rect_2d(*this, x0, y0, x1, y1, color);
    }

    virtual void char_2d(const font_t& font, int x, int y, char c, float color)
    {
        generic_char_2d(*this, font, x, y, c, color);
    }

protected:
    virtual void clear_color(float c) = 0;
    virtual void set_color(int x, int y, float c) = 0;
    virtual void blend_color(int x, int y, float c, float a) = 0;
    virtual void commit_impl() = 0;
    virtual void present_impl() = 0;

private:
    bool clip(int x, int y) const
    {
        return x < 0 || y < 0 || x >= width || y >= height;
    }
};

