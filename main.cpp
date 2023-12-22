#define _USE_MATH_DEFINES
#include "toaster/PixelToaster.h"
#include "drawing.h"
#include "math.h"
#include "mesh.h"
#include "imgui/imgui.h"

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

    void fill_rect_2d(int x0, int y0, int x1, int y1, float color) override final
    {
        auto pixel = color_to_pixel(color);

        if (x0 > x1) std::swap(x0, x1);
        if (y0 > y1) std::swap(y0, y1);

        auto out_row = colors.data() + x0 + y0 * width;
        for (int y = y0; y < y1; ++y, out_row += width)
        {
            auto out = out_row;
            for (int x = x0; x < x1; ++x, ++out)
                *out = pixel;
        }
    }

    virtual void char_2d(const font_t& font, int x, int y, char c, float color) override final
    {
        auto data = font.find(c);
        if (!data)
            return;

        typedef bool (*unpack_font_proc)(const uint8_t* data, int x, int y);

        unpack_font_proc unpack_font = nullptr;
        switch (font.pack)
        {
            case font_pack_row_low:     unpack_font = [](const uint8_t* data, int x, int y) { return (data[x] & (1 <<      y))  != 0; }; break;
            case font_pack_row_high:    unpack_font = [](const uint8_t* data, int x, int y) { return (data[x] & (1 << (7 - y))) != 0; }; break;
            case font_pack_column_low:  unpack_font = [](const uint8_t* data, int x, int y) { return (data[y] & (1 <<      x))  != 0; }; break;
            case font_pack_column_high: unpack_font = [](const uint8_t* data, int x, int y) { return (data[y] & (1 << (7 - x))) != 0; }; break;
            default: return;
        }

        auto color_pixel = color_to_pixel(color);
        auto bg_pixel    = color_to_pixel(0);

        auto out_row = colors.data() + x + y * width;
        for (int y = 0; y < font.h; ++y, out_row += width)
        {
            auto out = out_row;
            for (int x = 0; x < font.w; ++x, ++out)
                *out = unpack_font(data, x, y) ? color_pixel : bg_pixel;
        }
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

        colors[x + width * y] = pixel;
    }

    virtual void blend_color(int x, int y, float c, float a) override final
    {
        auto back  = colors[x + width * y];
        auto pixel = color_to_pixel(c);

        auto ia = (int)(a * 255);

        pixel.r = (int)back.r + ((int)pixel.r - (int)back.r) * ia / 255;
        pixel.g = (int)back.g + ((int)pixel.g - (int)back.g) * ia / 255;
        pixel.b = (int)back.b + ((int)pixel.b - (int)back.b) * ia / 255;

        colors[x + y * width] = pixel;
    }

    virtual void commit_impl() override final
    {
    }

    virtual void present_impl() override final
    {
        display.update(colors);
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
    framebuffer_t&     buffer;
    const font_t&      font;
    std::vector<float> color;
    int                font_width;
    int                font_height;
    std::vector<char>  palette;

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

    void dither(bool useZbuffer)
    {
        const auto palette_size = (float)palette.size();

        const auto width = this->width;
        const auto height = this->height;

        const auto k1Per15 = 1.0f / (palette_size - 1);
        const auto k1Per16 = 1.0f / 16.0f;
        const auto k3Per16 = 3.0f / 16.0f;
        const auto k5Per16 = 5.0f / 16.0f;
        const auto k7Per16 = 7.0f / 16.0f;

        int x = 0, y = 0;
        auto depth = this->depth.data();
        for (auto pixel = color.data(), pixelEnd = color.data() + color.size(); pixel < pixelEnd; ++pixel, ++x, ++depth)
        {
            if (x == width)
            {
                x = 0;
                ++y;
            }

            auto c = *pixel;
            auto c2 = std::min(1.0f, std::max(0.0f, floorf(c * palette_size) * k1Per15));
            auto ce = c - c2;

            *pixel = c2;

            auto n1 = pixel + 1;
            auto n2 = pixel + width - 1;
            auto n3 = pixel + width;
            auto n4 = pixel + width + 1;

            auto d1 = depth + 1;
            auto d2 = depth + width - 1;
            auto d3 = depth + width;
            auto d4 = depth + width + 1;

            if (x < width - 1)
                *n1 += (ce * k7Per16) * (useZbuffer ? std::clamp(1.0f - fabsf(*depth - *d1), 0.0f, 1.0f) : 1.0f);

            if (y < height - 1)
            {
                *n3 += ce * k5Per16 * (useZbuffer ? std::clamp(1.0f - fabsf(*depth - *d3), 0.0f, 1.0f) : 1.0f);

                if (x > 0)
                    *n2 += ce * k3Per16 * (useZbuffer ? std::clamp(1.0f - fabsf(*depth - *d2), 0.0f, 1.0f) : 1.0f);

                if (x < height - 1)
                    *n4 += ce * k1Per16 * (useZbuffer ? std::clamp(1.0f - fabsf(*depth - *d4), 0.0f, 1.0f) : 1.0f);
            }
        }
    }

protected:
    virtual void clear_color(float c) override final
    {
        color.assign(width * height, c);
        buffer.clear(c);
    }

    virtual void set_color(int x, int y, float c) override final
    {
        color[x + y * width] = c;
    }

    virtual void blend_color(int x, int y, float c, float a) override final
    {
        color[x + y * width] += (c - color[x + y * width]) * a;
    }

    virtual void commit_impl() override final
    {
        int x = 0, y = 0;
        for (auto pixel = color.data(), pixelEnd = color.data() + color.size(); pixel < pixelEnd; ++pixel, ++x)
        {
            if (x == width)
            {
                x = 0;
                ++y;
            }

            auto color = *pixel;

            if (color == 0.0f)
                continue;

            auto c = color_to_char(color);

            buffer.char_2d(font, x * font_width, y * font_height, c, 1.0f);

            // buffer.fill_rect_2d(x * font_width, y * font_height, x * font_width + font_width - 1, y * font_height + font_height - 1, color);
        }
    }

    virtual void present_impl() override final
    {
        buffer.present();
    }

private:
    char color_to_char(float c) const
    {
        c = std::min(1.0f, std::max(0.0f, c));

        auto index = std::min<int>(static_cast<int>(palette.size() * c), static_cast<int>(palette.size() - 1));

        return palette[index];
    }
};

static framebuffer_t* imgui_render_target = nullptr;
static void render_draw_lists(ImDrawData* draw_data)
{
    for (int n = 0; n < draw_data->CmdListsCount; n++)
    {
        const ImDrawList* const cmd_list = draw_data->CmdLists[n];

        auto& vertices = cmd_list->VtxBuffer;
        auto& indices  = cmd_list->IdxBuffer;

        int idx_offset = 0;
        for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.size(); cmd_i++)
        {
            const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
            if (pcmd->UserCallback)
            {
                pcmd->UserCallback(cmd_list, pcmd);
            }
            else if (pcmd->ElemCount > 0)
            {
                auto* const indexStart = indices.Data + idx_offset;

                for (unsigned int i = 0; i < pcmd->ElemCount; i += 3)
                {
                    const auto i0 = indexStart[i + 0];
                    const auto i1 = indexStart[i + 1];
                    const auto i2 = indexStart[i + 2];

                    const auto v0 = vertices[i0];
                    const auto v1 = vertices[i1];
                    const auto v2 = vertices[i2];

                    const auto vc0 = ImColor(v0.col);
                    const auto vc1 = ImColor(v1.col);
                    const auto vc2 = ImColor(v2.col);

                    const auto c0 = (vc0.Value.x + vc0.Value.y + vc0.Value.z) / 3;
                    const auto c1 = (vc1.Value.x + vc1.Value.y + vc1.Value.z) / 3;
                    const auto c2 = (vc2.Value.x + vc2.Value.y + vc2.Value.z) / 3;

                    if (pcmd->TextureId)
                    {
                        auto& image = *reinterpret_cast<image_t*>(pcmd->TextureId);

                        generic_triangle_2d(*imgui_render_target, image,
                            v0.pos.x, v0.pos.y,
                            v2.pos.x, v2.pos.y,
                            v1.pos.x, v1.pos.y,
                            v0.uv.x, v0.uv.y,
                            v2.uv.x, v2.uv.y,
                            v1.uv.x, v1.uv.y,
                            c0, c2, c1,
                            vc0.Value.w, vc2.Value.w, vc1.Value.w);
                    }
                    else
                    {
                        generic_triangle_2d(*imgui_render_target,
                            static_cast<int>(v0.pos.x), static_cast<int>(v0.pos.y),
                            static_cast<int>(v2.pos.x), static_cast<int>(v2.pos.y),
                            static_cast<int>(v1.pos.x), static_cast<int>(v1.pos.y),
                            c0);
                    }
                }

                //fill_rect_2d(*imgui_render_target, (int)min.x, (int)min.y, (int)max.x, (int)max.y, 1.0f);

                //const D3D10_RECT r = { (LONG)pcmd->ClipRect.x, (LONG)pcmd->ClipRect.y, (LONG)pcmd->ClipRect.z, (LONG)pcmd->ClipRect.w };
                //ctx->PSSetShaderResources(0, 1, (ID3D10ShaderResourceView**)&pcmd->TextureId);
                //ctx->RSSetScissorRects(1, &r);
                //ctx->DrawIndexed(pcmd->ElemCount, idx_offset, vtx_offset);
            }
            idx_offset += pcmd->ElemCount;
        }
    }
}

struct transformed_vertex_t
{
    vec4  p{};
    vec3  n{};
    float c{};

    friend transformed_vertex_t lerp(const transformed_vertex_t& a, const transformed_vertex_t& b, float t)
    {
        return
        {
            a.p + t * (b.p - a.p),
            a.n + t * (b.n - a.n),
            a.c + t * (b.c - a.c),
        };
    }
};

struct transformed_triangle_t
{
    transformed_vertex_t a{}, b{}, c{};
};

struct triangle_clip_result_t
{
    transformed_triangle_t triangles[64];
    int                    triangle_count = 0;
};

static void clip_triangle(const transformed_triangle_t& triangle, triangle_clip_result_t& result)
{
    triangle_clip_result_t buffers[2];
    triangle_clip_result_t* input  = &buffers[0];
    triangle_clip_result_t* output = &buffers[1];

    input->triangles[0]   = triangle;
    input->triangle_count = 1;

    struct clip_rule_t
    {
        using test_t  = bool  (*)(const transformed_vertex_t& v) noexcept;
        using limit_t = float (*)(const transformed_vertex_t& v) noexcept;

        test_t  test;
        limit_t value;
        limit_t limit;
    };

    static constexpr clip_rule_t clip_rules[]
    {
        { [](const auto& v) noexcept { return  v.p.x <= v.p.w; }, [](const auto& v) noexcept { return v.p.x; }, [](const auto& v) noexcept { return                       v.p.w; } },
        { [](const auto& v) noexcept { return  v.p.y <= v.p.w; }, [](const auto& v) noexcept { return v.p.y; }, [](const auto& v) noexcept { return                       v.p.w; } },
        { [](const auto& v) noexcept { return  v.p.z <= v.p.w; }, [](const auto& v) noexcept { return v.p.z; }, [](const auto& v) noexcept { return                       v.p.w; } },
        { [](const auto& v) noexcept { return -v.p.w  < v.p.x; }, [](const auto& v) noexcept { return v.p.x; }, [](const auto& v) noexcept { return -v.p.w + FLT_EPSILON * 2.0f; } },
        { [](const auto& v) noexcept { return -v.p.w  < v.p.y; }, [](const auto& v) noexcept { return v.p.y; }, [](const auto& v) noexcept { return -v.p.w + FLT_EPSILON * 2.0f; } },
        { [](const auto& v) noexcept { return   0.0f  < v.p.z; }, [](const auto& v) noexcept { return v.p.z; }, [](const auto& v) noexcept { return          FLT_EPSILON * 2.0f; } },
    };

    for (const auto& rule : clip_rules)
    {
        const bool is_last_rule = &rule == &clip_rules[sizeof(clip_rules) / sizeof(*clip_rules) - 1];

        if (is_last_rule)
            output = &result;

        output->triangle_count = 0;

        for (int i = 0; i < input->triangle_count; ++i)
        {
            const auto& triangle = input->triangles[i];

            const auto test0 = rule.test(triangle.a);
            const auto test1 = rule.test(triangle.b);
            const auto test2 = rule.test(triangle.c);

            const auto count = static_cast<int>(test0) + static_cast<int>(test1) + static_cast<int>(test2);

            if (count == 0)
                continue;

            if (count == 3)
            {
                output->triangles[output->triangle_count++] = triangle;
            }
            else
            {
                auto A = &triangle.a, B = &triangle.b, C = &triangle.c;
                     if (count == 2 ? !test1 : test1) { A = &triangle.b; B = &triangle.c; C = &triangle.a; }
                else if (count == 2 ? !test2 : test2) { A = &triangle.c; B = &triangle.a; C = &triangle.b; }

                const auto a_value = rule.value(*A);
                const auto b_value = rule.value(*B);
                const auto c_value = rule.value(*C);

                const auto a_limit = rule.limit(*A);
                const auto b_limit = rule.limit(*B);
                const auto c_limit = rule.limit(*C);

                auto t_b = (b_limit - b_value) / (b_limit - a_limit - b_value + a_value);
                auto t_c = (c_limit - c_value) / (c_limit - a_limit - c_value + a_value);

                auto vb = lerp(*B, *A, t_b);
                auto vc = lerp(*C, *A, t_c);

                if (count == 2)
                {
                    output->triangles[output->triangle_count++] = { *B, *C, vb };
                    output->triangles[output->triangle_count++] = { *C, vc, vb };
                }
                else
                {
                    output->triangles[output->triangle_count++] = { *A, vb, vc };
                }
            }
        }

        std::swap(input, output);
    }
}

struct imgui_listener final: public PixelToaster::Listener
{
    virtual void onMouseButtonDown(PixelToaster::DisplayInterface& display, PixelToaster::Mouse mouse) override final
    {
        auto& io = ImGui::GetIO();
        io.MousePos.x = mouse.x * imgui_render_target->width  / display.width();
        io.MousePos.y = mouse.y * imgui_render_target->height / display.height();
        if (mouse.buttons.left)   io.MouseDown[0] = true;
        if (mouse.buttons.right)  io.MouseDown[1] = true;
        if (mouse.buttons.middle) io.MouseDown[2] = true;
    }

    virtual void onMouseButtonUp(PixelToaster::DisplayInterface& display, PixelToaster::Mouse mouse) override final
    {
        auto& io = ImGui::GetIO();
        io.MousePos.x = mouse.x * imgui_render_target->width / display.width();
        io.MousePos.y = mouse.y * imgui_render_target->height / display.height();
        if (io.MouseDown[0] && !mouse.buttons.left)   io.MouseDown[0] = false;
        if (io.MouseDown[1] && !mouse.buttons.right)  io.MouseDown[1] = false;
        if (io.MouseDown[2] && !mouse.buttons.middle) io.MouseDown[2] = false;
    }

    virtual void onMouseMove(PixelToaster::DisplayInterface& display, PixelToaster::Mouse mouse) override final
    {
        auto& io = ImGui::GetIO();
        io.MousePos.x = mouse.x * imgui_render_target->width / display.width();
        io.MousePos.y = mouse.y * imgui_render_target->height / display.height();
    }
};

int wmain()
{
    namespace pt = PixelToaster;

    float displayScale = 1.0f;

    pt::Display display("ASCII Renderer", static_cast<int>(1440 * displayScale), static_cast<int>(800 * displayScale));
    imgui_listener listener;
    display.listener(&listener);

    display.zoom(2);

    pt::Timer timer;

    toaster_framebuffer_t display_buffer(display);

    const auto ascii_font_5x7  = ascii_font_t{ get_font_5x7(),  " .',\";o%O8@#", 1 };
    const auto ascii_font_8x8  = ascii_font_t{ get_font_8x8(),  " .',\";o%O8@#", 0 };
    const auto ascii_font_8x13 = ascii_font_t{ get_font_8x13(), " .',;\"o#@%O8", 0 };

    auto& font         = get_font_8x8();
    auto  ascii_buffer = ascii_framebuffer_t(display_buffer, ascii_font_8x8);

    std::vector<transformed_vertex_t> vertices;

    struct object_t
    {
        mesh_t  mesh;
        matrix4 transformation;
    };

    object_t torus  = { make_torus(10, 5, 24, 16), matrix4::identity };
    object_t box    = { make_box(15, 15, 15),      matrix4::identity };
    object_t teapot = { make_teapot(5, 4),         matrix4::identity };
    object_t line   = { make_line(-19.0f, 0.0f, 0.0f, 19.0f, 0.0f, 0.0f), matrix4::identity };
    object_t normal = { make_normals(teapot.mesh, 0.350f), matrix4::identity };

    object_t* objects[] =
    {
        &torus,
        &box,
        &teapot,
        &line,
        &normal,
    };
    int object_count = sizeof(objects) / sizeof(*objects);

    image_t font_atlas = {};
    {
        auto& io = ImGui::GetIO();
        io.RenderDrawListsFn = render_draw_lists;

        unsigned char* pixels;
        int width, height;
        io.Fonts->GetTexDataAsAlpha8(&pixels, &width, &height);

        font_atlas.data   = pixels;
        font_atlas.width  = width;
        font_atlas.height = height;
        font_atlas.pitch  = width;

        io.Fonts->TexID = &font_atlas;
    }
    imgui_render_target = &display_buffer;

    bool pause                = false;
    bool use_ascii_buffer     = false;
    bool dither_ascii_buffer  = false;
    bool dither_with_z_buffer = false;
    bool solid                = true;
    bool lines                = true;
    bool wireframe            = false;
    bool wireframe_2d         = false;
    float angle               = 0.0f;
    float scale               = 1.0f;
    int  current_font         = 1;

    timer.reset();
    float time = 0.0f;
    while (display.open())
    {
        auto deltaTime = static_cast<float>(timer.delta());

        if (!pause)
            time += deltaTime;

        auto& io = ImGui::GetIO();
        io.DisplaySize.x = static_cast<float>(imgui_render_target->width);
        io.DisplaySize.y = static_cast<float>(imgui_render_target->height);
        io.DeltaTime     = deltaTime;

        ImGui::NewFrame();

        auto& buffer = *(use_ascii_buffer ? (framebuffer_t*)&ascii_buffer : (framebuffer_t*)&display_buffer);

        const float window_w      = (float)display_buffer.width;
        const float window_h      = (float)display_buffer.height;
        const float window_aspect = window_w / window_h;

        const viewport_t viewport =
        {
            0.0f,//buffer.width,
            -buffer.height / 2.0f,
            (float)buffer.width / 2,
            (float)buffer.height / 2,
            0.0f,
            1.0f,
        };

        const auto view       = matrix4::lookAtLH(vec3(0, -50, 0), vec3(0, 0, 0), vec3(0, 0, 1));
        const auto projection = matrix4::perspectiveFovLH((float)M_PI / 8.0f, window_aspect, 1.0f, 500.0f);

        buffer.clear(0, 1.0f);

        torus.transformation =
            matrix4::scale(scale, scale, scale) *
            //matrix4::rotationYawPitchRoll(time - 1, time * 0.1f, 0) *
            matrix4::rotationYawPitchRoll(time, time * 0.4f, time * -0.25f) *
            matrix4::identity;
        torus.transformation[12] = -20;// * sinf(time * 1.25f);
        torus.transformation = torus.transformation *
            matrix4::rotationYawPitchRoll(0.0f, 0.0f, angle);

        box.transformation =
            matrix4::scale(scale, scale, scale) *
            matrix4::rotationYawPitchRoll(time + 1, -time * 0.2f, time * -0.35f) *
            matrix4::identity;
        box.transformation[12]   =  20;// * sinf(time * 1.25f);
        box.transformation = box.transformation *
            matrix4::rotationYawPitchRoll(0.0f, 0.0f, angle);

        teapot.transformation =
            matrix4::scale(scale, scale, scale) *
            matrix4::translation(0, 0, -4) *
            matrix4::rotationYawPitchRoll(time - 1, time * 0.1f, time * 0.45f) *
            matrix4::translation(0, 0, 0) *
            matrix4::rotationYawPitchRoll(0.0f, 0.0f, angle);

        //teapot.transformation =
        //    matrix4::scale(scale, scale, scale) *
        //    matrix4::translation(0, 0, -6) *
        //    matrix4::rotationYawPitchRoll(time - 1, time * 0.1f, time * 0.45f) *
        //    matrix4::rotationYawPitchRoll(0, 0, angle + time * 0.45f) *
        //    //matrix4::rotationYawPitchRoll(0.0f, 0.0f, angle + time * 0.45f) *
        //    matrix4::translation(0, -16, 0);

        line.transformation =
            //matrix4::translation(0, 0, 0) *
            //matrix4::rotationYawPitchRoll(time - 1, time * 0.1f, time * 0.45f) *
            //matrix4::rotationYawPitchRoll(0.0f, 0.0f, time * 0.45f) *
            //matrix4::translation(0, 0, 0) *
            torus.transformation;

        normal.transformation =
            //matrix4::translation(0, 0, 0) *
            //matrix4::rotationYawPitchRoll(time - 1, time * 0.1f, time * 0.45f) *
            //matrix4::rotationYawPitchRoll(0.0f, 0.0f, time * 0.45f) *
            //matrix4::translation(0, 0, 0) *
            teapot.transformation;

        const auto camera_transformation = view * projection;

        const auto clip_transformation = matrix4::clip(
            viewport.clipX, viewport.clipY, viewport.clipWidth, viewport.clipHeight, viewport.minZ, viewport.maxZ);

        const auto viewport_scale =
            matrix4::translation(1.0f, -1.0f, 0.0f) *
            matrix4::scale(0.5f * buffer.width, -0.5f * buffer.height, 1.0f)
            ;


        for (int i = 0; i < object_count; ++i)
        {
            auto& object = *objects[i];

            const auto& indices  = object.mesh.indices;
            vertices.reserve(object.mesh.vertices.size());
            vertices.resize(0);

            const auto transformation = object.transformation * camera_transformation * clip_transformation;
            const auto transposed     = (object.transformation * view).transposed();

            for (auto& vertex : object.mesh.vertices)
            {
                transformed_vertex_t v;
                v.p = vec4(vertex.p, 1.0f).transformed(transformation);
                v.n = vertex.n.transformed_vector(object.transformation).normalized();
                v.c = vertex.c;

                vertices.push_back(v);
            }

            float minZ = 1.0f;
            float maxZ = 0.0f;
# if 0
            for (auto& vtx : vertices)
            {
                minZ = std::min(minZ, vtx.p.z);
                maxZ = std::max(maxZ, vtx.p.z);

                //buffer.set(vtx.p.x, vtx.p.y, 1.0f);
            }
# else
            minZ = 0.96f;
            maxZ = 0.99f;
# endif

            if (solid && object.mesh.primitive_type == primitive_type_t::triangle_list)
            {
                for (int i = 0; i < (int)indices.size() / 3; ++i)
                {
                    auto i0 = indices[i * 3 + 0], i1 = indices[i * 3 + 1], i2 = indices[i * 3 + 2];

                    const auto clip = true;

                    const transformed_triangle_t triangle = { vertices[i0], vertices[i1], vertices[i2] };
                    triangle_clip_result_t clipped_triangles;
                    if (clip)
                        clip_triangle(triangle, clipped_triangles);

                    const auto& triangles      = clip ? &clipped_triangles.triangles[0]  : &triangle;
                    const auto& triangle_count = clip ? clipped_triangles.triangle_count : 1;

                    for (int triangle_index = 0; triangle_index < triangle_count; ++triangle_index)
                    {
                        const auto& triangle = triangles[triangle_index];

                        const auto& v0 = triangle.a;
                        const auto& v1 = triangle.b;
                        const auto& v2 = triangle.c;

                        //if (cross(v0.p.xy() - v1.p.xy(), v0.p.xy() - v2.p.xy()) < 0)
                        //    continue;

                        //const auto t0 = (-v0.p.w < v0.p.x) && (v0.p.x <= v0.p.w) && (-v0.p.w < v0.p.y) && (v0.p.y <= v0.p.w) && (0.0f < v0.p.z) && (v0.p.z <= v0.p.w);
                        //const auto t1 = (-v1.p.w < v1.p.x) && (v1.p.x <= v1.p.w) && (-v1.p.w < v1.p.y) && (v1.p.y <= v1.p.w) && (0.0f < v1.p.z) && (v1.p.z <= v1.p.w);
                        //const auto t2 = (-v2.p.w < v2.p.x) && (v2.p.x <= v2.p.w) && (-v2.p.w < v2.p.y) && (v2.p.y <= v2.p.w) && (0.0f < v2.p.z) && (v2.p.z <= v2.p.w);

                        //if (!t0 || !t1 || !t2)
                        //    continue;

                        const auto p0 = v0.p.transformed(viewport_scale);
                        const auto p1 = v1.p.transformed(viewport_scale);
                        const auto p2 = v2.p.transformed(viewport_scale);

                        const auto o0 = vec3(p0.x / p0.w, p0.y / p0.w, p0.z / p0.w);
                        const auto o1 = vec3(p1.x / p1.w, p1.y / p1.w, p1.z / p1.w);
                        const auto o2 = vec3(p2.x / p2.w, p2.y / p2.w, p2.z / p2.w);

                        if (cross(o0 - o1, o0 - o2).z < 0)
                            continue;

                        //const auto c0 = 1.0f - (o0.z - minZ) / (maxZ - minZ);
                        //const auto c1 = 1.0f - (o1.z - minZ) / (maxZ - minZ);
                        //const auto c2 = 1.0f - (o2.z - minZ) / (maxZ - minZ);

                        const auto c0 = std::max(v0.n.dot(vec3(5, 0, 10).normalized()) * 0.5f + 0.5f, 0.0f);
                        const auto c1 = std::max(v1.n.dot(vec3(5, 0, 10).normalized()) * 0.5f + 0.5f, 0.0f);
                        const auto c2 = std::max(v2.n.dot(vec3(5, 0, 10).normalized()) * 0.5f + 0.5f, 0.0f);

                        generic_triangle_3d(buffer,
                            o1.x, o1.y, o1.z,
                            o0.x, o0.y, o0.z,
                            o2.x, o2.y, o2.z,
                            c1, c0, c2);
                    }
                }
            }

            if (object.mesh.primitive_type == primitive_type_t::triangle_list)
            {
                for (int i = 0; i < (int)indices.size() / 3; ++i)
                {
                    auto i0 = indices[i * 3 + 0], i1 = indices[i * 3 + 1], i2 = indices[i * 3 + 2];

                    const auto& v0 = vertices[i0];
                    const auto& v1 = vertices[i1];
                    const auto& v2 = vertices[i2];

                    const auto p0 = v0.p.transformed(viewport_scale);
                    const auto p1 = v1.p.transformed(viewport_scale);
                    const auto p2 = v2.p.transformed(viewport_scale);

                    const auto o0 = vec3(p0.x / p0.w, p0.y / p0.w, p0.z / p0.w);
                    const auto o1 = vec3(p1.x / p1.w, p1.y / p1.w, p1.z / p1.w);
                    const auto o2 = vec3(p2.x / p2.w, p2.y / p2.w, p2.z / p2.w);

                    if (cross(o0 - o1, o0 - o2).z < 0)
                        continue;

                    //const auto c0 = 1.0f - (v0.p.z - minZ) / (maxZ - minZ);
                    //const auto c1 = 1.0f - (v1.p.z - minZ) / (maxZ - minZ);
                    //const auto c2 = 1.0f - (v2.p.z - minZ) / (maxZ - minZ);

                    const auto c0 = std::max(v0.n.dot(vec3(5, 0, 10).normalized()) * 0.5f + 0.5f, 0.0f);
                    const auto c1 = std::max(v1.n.dot(vec3(5, 0, 10).normalized()) * 0.5f + 0.5f, 0.0f);
                    const auto c2 = std::max(v2.n.dot(vec3(5, 0, 10).normalized()) * 0.5f + 0.5f, 0.0f);

                    if (wireframe)
                    {
                        generic_line_3d(buffer,
                            o1.x, o1.y, o1.z,
                            o0.x, o0.y, o0.z,
                            c1, c0);

                        generic_line_3d(buffer,
                            o1.x, o1.y, o1.z,
                            o2.x, o2.y, o2.z,
                            c1, c2);

                        generic_line_3d(buffer,
                            o0.x, o0.y, o0.z,
                            o2.x, o2.y, o2.z,
                            c0, c2);
                    }

                    if (wireframe_2d)
                    {
                        generic_line_2d(buffer,
                            static_cast<int>(o1.x), static_cast<int>(o1.y),
                            static_cast<int>(o0.x), static_cast<int>(o0.y),
                            (c1 + c0) * 0.5f);

                        generic_line_2d(buffer,
                            static_cast<int>(o1.x), static_cast<int>(o1.y),
                            static_cast<int>(o2.x), static_cast<int>(o2.y),
                            (c1 + c2) * 0.5f);

                        generic_line_2d(buffer,
                            static_cast<int>(o0.x), static_cast<int>(o0.y),
                            static_cast<int>(o2.x), static_cast<int>(o2.y),
                            (c0 + c2) * 0.5f);
                    }
                }
            }

            if (object.mesh.primitive_type == primitive_type_t::line_list)
            {
                auto invertedTransformation = (object.transformation * camera_transformation).inverted();

                for (int i = 0; i < (int)indices.size() / 2; ++i)
                {
                    auto i0 = indices[i * 2 + 0], i1 = indices[i * 2 + 1];

                    const auto& v0 = vertices[i0];
                    const auto& v1 = vertices[i1];

                    const auto p0 = v0.p.transformed(viewport_scale);
                    const auto p1 = v1.p.transformed(viewport_scale);

                    const auto o0 = vec3(p0.x / p0.w, p0.y / p0.w, p0.z / p0.w);
                    const auto o1 = vec3(p1.x / p1.w, p1.y / p1.w, p1.z / p1.w);

                    const auto c0 = 1.0f;// - (v0.p.z - minZ) / (maxZ - minZ);
                    const auto c1 = 1.0f;// - (v1.p.z - minZ) / (maxZ - minZ);

                    if (lines || wireframe)
                    {
                        generic_line_3d(buffer,
                            o1.x, o1.y, o1.z,
                            o0.x, o0.y, o0.z,
                            c1, c0);
                    }

                    if (wireframe_2d)
                    {
                        generic_line_2d(buffer,
                            static_cast<int>(o1.x), static_cast<int>(o1.y),
                            static_cast<int>(o0.x), static_cast<int>(o0.y),
                            (c1 + c0) * 0.5f);
                    }
                }
            }
        }

        //for (auto& vtx : vertices)
        //    buffer.set(vtx.p.x, vtx.p.y, 1.0f);

        //generic_triangle_3d(buffer, 5, 5, 1,           (float)buffer.width - 5, 15, 1, (float)buffer.width - 5, 5, 1, 0, 1, 1);
        //generic_triangle_3d(buffer, 5, 5, 1, 5, 15, 1, (float)buffer.width - 5, 15, 1, 0, 0, 1);

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

        if (use_ascii_buffer && dither_ascii_buffer)
            ascii_buffer.dither(dither_with_z_buffer);

        buffer.commit();

        ImGui::SetNextWindowPos(ImVec2(0, 0));
        if (ImGui::Begin("Example: Fixed Overlay", nullptr, ImVec2(0, 0), 0.0f, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::Text("Mouse Position: (%.1f,%.1f)", ImGui::GetIO().MousePos.x, ImGui::GetIO().MousePos.y);
            ImGui::Text("Buffer: (%.0f,%.0f)", (float)buffer.width, (float)buffer.height);

            if (ImGui::Combo("Font", &current_font, "5x7\08x8\08x13\0\0"))
            {
                ascii_buffer.~ascii_framebuffer_t();
                switch (current_font)
                {
                    default:
                    case 0: new (&ascii_buffer) ascii_framebuffer_t(display_buffer, ascii_font_5x7);  break;
                    case 1: new (&ascii_buffer) ascii_framebuffer_t(display_buffer, ascii_font_8x8);  break;
                    case 2: new (&ascii_buffer) ascii_framebuffer_t(display_buffer, ascii_font_8x13); break;
                }
            }

            ImGui::Spacing();

            ImGui::Checkbox("Render to ASCII buffer", &use_ascii_buffer);
            ImGui::Checkbox("Dither ASCII buffer", &dither_ascii_buffer);
            ImGui::Checkbox("Dither with Z-buffer", &dither_with_z_buffer);
            ImGui::Spacing();
            ImGui::Checkbox("Solid", &solid);
            ImGui::Checkbox("Lines", &lines);
            ImGui::Checkbox("Wireframe", &wireframe);
            ImGui::Checkbox("Wireframe (2D)", &wireframe_2d);
            ImGui::Spacing();
            ImGui::SliderAngle("Angle", &angle, -180.0f, 180.0f);
            ImGui::DragFloat("Scale", &scale, 0.01f, 0.1f, 4.0f);
            ImGui::Spacing();
            ImGui::Checkbox("Pause", &pause);
        }
        ImGui::End();

        ImGui::Render();

        buffer.present();
    }
}