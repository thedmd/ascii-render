#define _USE_MATH_DEFINES
#include "toaster/PixelToaster.h"
#include "drawing.h"
#include "math.h"
#include "mesh.h"

#include <vector>
#include <algorithm>
#include <iterator>
#include <cstdlib>

struct toaster_framebuffer_t final: framebuffer_t
{
    typedef PixelToaster::TrueColorPixel pixel_t;

    PixelToaster::Display& display;
    std::vector<pixel_t>   colors;

    toaster_framebuffer_t(PixelToaster::Display& display):
        framebuffer_t(display.width(), display.height()),
        display(display),
        colors(width * height, pixel_t(0, 0, 0, 0))
    {
    }

protected:
    virtual void clear_color(float c) override final
    {
        colors.assign(width * height, color_to_pixel(c));
    }

    virtual void set_color(int x, int y, float c) override final
    {
        auto back  = colors[x + width * y];
        auto pixel = color_to_pixel(c);

        // pixel.a = 128;
        //
        // pixel.r = (int)back.r + ((int)pixel.r - (int)back.r) * pixel.a / 255;
        // pixel.g = (int)back.g + ((int)pixel.g - (int)back.g) * pixel.a / 255;
        // pixel.b = (int)back.b + ((int)pixel.b - (int)back.b) * pixel.a / 255;

        colors[x + width * y] = pixel;
    }

private:
    pixel_t color_to_pixel(float c) const
    {
        auto brightness = std::max(0, std::min(255, (int)(255 * c)));
        return pixel_t(brightness, brightness, brightness, 255);
    }
};

struct ascii_font_t
{
    const font_t& font;
    const char*   palette;
    const int     padding;
};

struct ascii_framebuffer_t final: framebuffer_t
{
    framebuffer_t&    buffer;
    const font_t&     font;
    std::vector<char> color;
    int               font_width;
    int               font_height;
    std::vector<char> palette;

    ascii_framebuffer_t(framebuffer_t& buffer, const ascii_font_t& font):
        framebuffer_t(buffer.width / (font.font.w + font.padding), buffer.height / (font.font.h + font.padding)),
        buffer(buffer),
        font(font.font),
        color(width * height),
        font_width(font.font.w + font.padding),
        font_height(font.font.h + font.padding),
        palette(font.palette, font.palette + strlen(font.palette))
    {
    }

protected:
    char color_to_char(float c) const
    {
        c = std::min(1.0f, std::max(0.0f, c));

        auto index = std::min<int>((int)(palette.size() * c), palette.size() - 1);

        return palette[index];
    }

    virtual void clear_color(float c) override final
    {
        auto v = color_to_char(c);
        color.assign(width * height, v);

        buffer.clear(c);
    }

    virtual void set_color(int x, int y, float c) override final
    {
        auto v = color_to_char(c);
        color[x + y * width] = v;

        char_2d(buffer, font, x * font_width, y * font_height, v, 1.0f);
    }
};

int wmain()
{
    namespace pt = PixelToaster;

    pt::Display display("ASCII Renderer", 1440, 800);

    pt::Timer timer;

    toaster_framebuffer_t display_buffer(display);

    const auto ascii_font_5x7  = ascii_font_t{ get_font_5x7(),  " .',\";o%O8@#", 1 };
    const auto ascii_font_8x8  = ascii_font_t{ get_font_8x8(),  " .',\";o%O8@#", 0 };
    const auto ascii_font_8x13 = ascii_font_t{ get_font_8x13(), " .',;\"o#@%O8", 0 };

    auto& font   = get_font_8x8();
    auto  buffer = ascii_framebuffer_t(display_buffer, ascii_font_8x8);
    //auto& buffer = display_buffer;

    const float window_w      = (float)display_buffer.width;
    const float window_h      = (float)display_buffer.height;
    const float window_aspect = window_w / window_h;

    const float viewportX = 0.0f;
    const float viewportY = 0.0f;
    const float viewportW = (float)buffer.width;
    const float viewportH = (float)buffer.height;
    const float minZ      = 0.0f;
    const float maxZ      = 1.0f;

    const auto view       = matrix4::lookAtLH(vec3(0, -50, 0), vec3(0, 0, 0), vec3(0, 0, 1));
    const auto projection = matrix4::perspectiveFovLH((float)M_PI / 4.0f, window_aspect, 1.0f, 500.0f);

    std::vector<vertex_t> vertices;

    struct object_t
    {
        mesh_t  mesh;
        matrix4 transformation;
    };

    object_t torus  = { make_torus(12, 6, 24, 16), matrix4::identity };
    object_t box    = { make_box(15, 15, 15),      matrix4::identity };
    object_t teapot = { make_teapot(5, 4),         matrix4::identity };

    object_t* objects[] =
    {
        &torus,
        &box,
        &teapot
    };
    int object_count = sizeof(objects) / sizeof(*objects);

    timer.reset();
    while (display.open())
    {
        buffer.clear(0, 1.0f);

        auto time = (float)timer.time();

        torus.transformation = matrix4::rotationYawPitchRoll(time, time * 0.4f, time * -0.25f);
        torus.transformation[12] = -15 * sinf(time * 1.25f);

        //box.transformation = matrix4::rotationYawPitchRoll(0, 0, cosf(time));
        box.transformation = matrix4::rotationYawPitchRoll(time + 1, -time * 0.2f, time * -0.35f);
        box.transformation[12]   =  15 * sinf(time * 1.25f);

        teapot.transformation =
            matrix4::translation(0, 0, -6) *
            matrix4::rotationYawPitchRoll(time - 1, time * 0.1f, time * 0.45f) *
            matrix4::translation(0, 0, 0);

        const auto camera_transformation = view * projection;

        for (int i = 0; i < object_count; ++i)
        {
            auto& object = *objects[i];

            const auto& indices  = object.mesh.indices;
            vertices.assign(object.mesh.vertices.begin(), object.mesh.vertices.end());

            //float angle = (time + i);
            //auto rotation = matrix4::rotationYawPitchRoll(angle, angle * 0.4f, angle * -0.25f);
//             if (&object == &teapot)
//                 rotation = matrix4::identity;

            auto transformation = object.transformation * camera_transformation;
            auto transposed     = (object.transformation * view).transposed();

            for (auto& vtx : vertices)
            {
                vtx.p = project(vtx.p, viewportX, viewportY, viewportW, viewportH, minZ, maxZ, transformation);
                vtx.n = vtx.n.transformed_vector(transposed).normalized();
            }

            float minZ = 1.0f;
            float maxZ = 0.0f;
            for (auto& vtx : vertices)
            {
                minZ = std::min(minZ, vtx.p.z);
                maxZ = std::max(maxZ, vtx.p.z);

                //buffer.set(vtx.p.x, vtx.p.y, 1.0f);
            }

            for (int i = 0; i < (int)indices.size(); i += 3)
            {
                auto i0 = indices[i], i1 = indices[i + 1], i2 = indices[i + 2];

                const auto& v0 = vertices[i0];
                const auto& v1 = vertices[i1];
                const auto& v2 = vertices[i2];

                if (cross(v0.p - v1.p, v0.p - v2.p).z < 0)
                    continue;

                const auto c0 = 1.0f - (v0.p.z - minZ) / (maxZ - minZ);
                const auto c1 = 1.0f - (v1.p.z - minZ) / (maxZ - minZ);
                const auto c2 = 1.0f - (v2.p.z - minZ) / (maxZ - minZ);

                //const auto c0 = std::max(v0.n.dot(vec3(0, -1, 0)) * 0.5f + 0.5f, 0.0f);//v0.n.z * 0.5f + 0.5f;
                //const auto c1 = std::max(v1.n.dot(vec3(0, -1, 0)) * 0.5f + 0.5f, 0.0f);//v1.n.z * 0.5f + 0.5f;
                //const auto c2 = std::max(v2.n.dot(vec3(0, -1, 0)) * 0.5f + 0.5f, 0.0f);//v2.n.z * 0.5f + 0.5f;

                //const auto c0 = v0.n.z * 0.5f + 0.5f;
                //const auto c1 = v1.n.z * 0.5f + 0.5f;
                //const auto c2 = v2.n.z * 0.5f + 0.5f;

                triangle_3d(buffer,
                    v1.p.x, v1.p.y, v1.p.z,
                    v0.p.x, v0.p.y, v0.p.z,
                    v2.p.x, v2.p.y, v2.p.z,
                    c1, c0, c2);
            }
        }

        //for (auto& vtx : vertices)
        //    buffer.set(vtx.p.x, vtx.p.y, 1.0f);

        //triangle_3d(buffer, 5, 5, 1, (float)buffer.width - 5, 15, 1, (float)buffer.width - 5, 5, 1, 0, 1, 1);
        //triangle_3d(buffer, 5, 5, 1, 5, 15, 1, (float)buffer.width - 5, 15, 1, 0, 0, 1);

        //int x = 1, y = 1;
        //for (int i = 0x20; i < 0x80; ++i)
        //{
        //    char_2d(display_buffer, font, x, y, i, 1);
        //    if ((i % 8) == 0)
        //    {
        //        x = 1;
        //        y += font.h + 1;
        //    }
        //    else
        //        x += font.w + 1;
        //}

        //auto& f = get_font_8x8();
        //int range = 0x7F - 0x20;
        //int index = (int)fmodf(time * 10.0f, (float)range) + 0x20;
        //char_2d(buffer, f, 5, 21, index, 1);

        display.update(display_buffer.colors);
    }
}