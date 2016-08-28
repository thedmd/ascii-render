#include "drawing.h"
#include <algorithm>

template <typename T> static inline T min(T a, T b, T c) { return std::min(a, std::min(b, c)); }
template <typename T> static inline T max(T a, T b, T c) { return std::max(a, std::max(b, c)); }

void generic_fill_rect_2d(framebuffer_t& buffer, int x0, int y0, int x1, int y1, float color)
{
    if (x0 > x1) std::swap(x0, x1);
    if (y0 > y1) std::swap(y0, y1);

    for (int y = y0; y < y1; ++y)
    {
        for (int x = x0; x < x1; ++x)
        {
            buffer.set(x, y, color);
        }
    }
}

// http://www.codecodex.com/wiki/Bresenham's_line_algorithm
void generic_circle_2d(framebuffer_t& buffer, int cx, int cy, int radius, float color)
{
    int f     = 1 - radius;
    int ddF_x = 0;
    int ddF_y = -2 * radius;
    int x     = 0;
    int y     = radius;

    buffer.set(cx, cy + radius, color);
    buffer.set(cx, cy - radius, color);
    buffer.set(cx + radius, cy, color);
    buffer.set(cx - radius, cy, color);

    while (x < y)
    {
        if (f >= 0)
        {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x + 1;
        buffer.set(cx + x, cy + y, color);
        buffer.set(cx - x, cy + y, color);
        buffer.set(cx + x, cy - y, color);
        buffer.set(cx - x, cy - y, color);
        buffer.set(cx + y, cy + x, color);
        buffer.set(cx - y, cy + x, color);
        buffer.set(cx + y, cy - x, color);
        buffer.set(cx - y, cy - x, color);
    }
}

// https://sites.google.com/site/ruslancray/lab/projects/bresenhamscircleellipsedrawingalgorithm/bresenham-s-circle-ellipse-drawing-algorithm
void generic_ellipse_2d(framebuffer_t& buffer, int cx, int cy, int rx, int ry, float color)
{
    const auto a2  = rx * rx;
    const auto b2  = ry * ry;
    const auto fa2 = 4 * a2;
    const auto fb2 = 4 * b2;

    for (int x = 0, y = ry, sigma = 2 * b2 + a2 * (1 - 2 * ry); b2 * x <= a2 * y; x++)
    {
        buffer.set(cx + x, cy + y, color);
        buffer.set(cx - x, cy + y, color);
        buffer.set(cx + x, cy - y, color);
        buffer.set(cx - x, cy - y, color);
        if (sigma >= 0)
        {
            sigma += fa2 * (1 - y);
            y--;
        }
        sigma += b2 * ((4 * x) + 6);
    }

    for (int x = rx, y = 0, sigma = 2 * a2 + b2 * (1 - 2 * rx); a2 * y <= b2 * x; y++)
    {
        buffer.set(cx + x, cy + y, color);
        buffer.set(cx - x, cy + y, color);
        buffer.set(cx + x, cy - y, color);
        buffer.set(cx - x, cy - y, color);
        if (sigma >= 0)
        {
            sigma += fb2 * (1 - x);
            x--;
        }
        sigma += a2 * ((4 * y) + 6);
    }
}

// http://www.roguebasin.com/index.php?title=Bresenham%27s_Line_Algorithm#C.2B.2B
void generic_line_2d(framebuffer_t& buffer, int x0, int y0, int x1, int y1, float color)
{
    const auto flip = abs(y1 - y0) > abs(x1 - x0);
    if (flip)
    {
        std::swap(x0, y0);
        std::swap(x1, y1);
    }

    if (x0 > x1)
    {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    const auto dx   = x1 - x0;
    const auto dy   = abs(y1 - y0);
    const auto step = y0 < y1 ? 1 : -1;

    auto error = dx / 2;
    auto y = y0;

    for (int x = x0; x < x1; ++x)
    {
        if (flip)
            buffer.set(y, x, color);
        else
            buffer.set(x, y, color);

        error = error - dy;
        if (error < 0)
        {
            y     += step;
            error += dx;
        }
    }
}

void generic_hline_2d(framebuffer_t& buffer, int x1, int y, int x2, float c)
{
    if (x1 > x2)
        std::swap(x1, x2);

    for (int x = x1; x < x2; ++x)
        buffer.set(x, y, c);
}

void generic_vline_2d(framebuffer_t& buffer, int x, int y1, int y2, float c)
{
    if (y1 > y2)
        std::swap(y1, y2);

    for (int y = y1; y < y2; ++y)
        buffer.set(x, y, c);
}

// http://forum.devmaster.net/t/advanced-rasterization/6145
void generic_triangle_2d(framebuffer_t& buffer, int x0, int y0, int x1, int y1, int x2, int y2, float color)
{
    // 28.4 fixed-point coordinates
    const int Y0 = y0 << 4;
    const int Y1 = y1 << 4;
    const int Y2 = y2 << 4;

    const int X0 = x0 << 4;
    const int X1 = x1 << 4;
    const int X2 = x2 << 4;

    // Deltas
    const int DX01 = X0 - X1;
    const int DX12 = X1 - X2;
    const int DX20 = X2 - X0;

    const int DY01 = Y0 - Y1;
    const int DY12 = Y1 - Y2;
    const int DY20 = Y2 - Y0;

    // Fixed-point deltas
    const int FDX01 = DX01 << 4;
    const int FDX12 = DX12 << 4;
    const int FDX20 = DX20 << 4;

    const int FDY01 = DY01 << 4;
    const int FDY12 = DY12 << 4;
    const int FDY20 = DY20 << 4;

    // Bounding rectangle
    int minx = (min(X0, X1, X2) + 0x0F) >> 4;
    int maxx = (max(X0, X1, X2) + 0x0F) >> 4;
    int miny = (min(Y0, Y1, Y2) + 0x0F) >> 4;
    int maxy = (max(Y0, Y1, Y2) + 0x0F) >> 4;

    // Half-edge constants
    int C0 = DY01 * X0 - DX01 * Y0;
    int C1 = DY12 * X1 - DX12 * Y1;
    int C2 = DY20 * X2 - DX20 * Y2;

    // Correct for fill convention
    if (DY01 < 0 || (DY01 == 0 && DX01 > 0)) C0++;
    if (DY12 < 0 || (DY12 == 0 && DX12 > 0)) C1++;
    if (DY20 < 0 || (DY20 == 0 && DX20 > 0)) C2++;

    int CY0 = C0 + DX01 * (miny << 4) - DY01 * (minx << 4);
    int CY1 = C1 + DX12 * (miny << 4) - DY12 * (minx << 4);
    int CY2 = C2 + DX20 * (miny << 4) - DY20 * (minx << 4);

    for (int y = miny; y < maxy; y++)
    {
        int CX0 = CY0;
        int CX1 = CY1;
        int CX2 = CY2;

        for (int x = minx; x < maxx; x++)
        {
            if (CX0 > 0 && CX1 > 0 && CX2 > 0)
            {
                buffer.set(x, y, color);
            }

            CX0 -= FDY01;
            CX1 -= FDY12;
            CX2 -= FDY20;
        }

        CY0 += FDX01;
        CY1 += FDX12;
        CY2 += FDX20;
    }
}

void generic_triangle_2d(framebuffer_t& buffer, const image_t& texture, float x0, float y0, float x1, float y1, float x2, float y2, float u0, float v0, float u1, float v1, float u2, float v2, float c0, float c1, float c2, float a0, float a1, float a2)
{
    // 24.8 fixed-point
    const int precission = 4;
    const int mask       = (1 << precission) - 1;

    // Fixed-point coordinates
    const int Y0 = (int)(y0 * static_cast<float>(1 << precission));
    const int Y1 = (int)(y1 * static_cast<float>(1 << precission));
    const int Y2 = (int)(y2 * static_cast<float>(1 << precission));

    const int X0 = (int)(x0 * static_cast<float>(1 << precission));
    const int X1 = (int)(x1 * static_cast<float>(1 << precission));
    const int X2 = (int)(x2 * static_cast<float>(1 << precission));

    // Deltas
    const int DX01 = X0 - X1;
    const int DX12 = X1 - X2;
    const int DX20 = X2 - X0;

    const int DY01 = Y0 - Y1;
    const int DY12 = Y1 - Y2;
    const int DY20 = Y2 - Y0;

    // Fixed-point deltas
    const int FDX01 = DX01 << precission;
    const int FDX12 = DX12 << precission;
    const int FDX20 = DX20 << precission;

    const int FDY01 = DY01 << precission;
    const int FDY12 = DY12 << precission;
    const int FDY20 = DY20 << precission;

    // Bounding rectangle
    int minx = (min(X0, X1, X2) + mask) >> precission;
    int maxx = (max(X0, X1, X2) + mask) >> precission;
    int miny = (min(Y0, Y1, Y2) + mask) >> precission;
    int maxy = (max(Y0, Y1, Y2) + mask) >> precission;

    // Half-edge constants
    int C0 = DY01 * X0 - DX01 * Y0;
    int C1 = DY12 * X1 - DX12 * Y1;
    int C2 = DY20 * X2 - DX20 * Y2;

    // Correct for fill convention
    if (DY01 < 0 || (DY01 == 0 && DX01 > 0)) C0++;
    if (DY12 < 0 || (DY12 == 0 && DX12 > 0)) C1++;
    if (DY20 < 0 || (DY20 == 0 && DX20 > 0)) C2++;

    int CY0 = C0 + DX01 * (miny << precission) - DY01 * (minx << precission);
    int CY1 = C1 + DX12 * (miny << precission) - DY12 * (minx << precission);
    int CY2 = C2 + DX20 * (miny << precission) - DY20 * (minx << precission);

    auto iC = 1.0f / (CY0 + CY1 + CY2);

    for (int y = miny; y < maxy; y++)
    {
        if (y >= 0 && y < buffer.height)
        {
            int CX0 = CY0;
            int CX1 = CY1;
            int CX2 = CY2;

            for (int x = minx; x < maxx; x++)
            {
                if (x >= 0 && x < buffer.width)
                {
                    if (CX0 > 0 && CX1 > 0 && CX2 > 0)
                    {
                        const auto c = (c2 * CX0 + c0 * CX1 + c1 * CX2) * iC;
                        const auto a = (a2 * CX0 + a0 * CX1 + a1 * CX2) * iC;
                        const auto u = (u2 * CX0 + u0 * CX1 + u1 * CX2) * iC;
                        const auto v = (v2 * CX0 + v0 * CX1 + v1 * CX2) * iC;

                        const auto tx = std::max(0, std::min(static_cast<int>(u * (texture.width)), texture.width - 1));
                        const auto ty = std::max(0, std::min(static_cast<int>(v * (texture.height)), texture.height - 1));

                        const auto texture_color = texture.data[tx + ty * texture.pitch] * (1.0f / 255.0f);

                        buffer.blend(x, y, c, texture_color * a);
                    }
                }
                else if (x >= buffer.width)
                    break;

                CX0 -= FDY01;
                CX1 -= FDY12;
                CX2 -= FDY20;
            }
        }
        else if (y >= buffer.height)
            return;

        CY0 += FDX01;
        CY1 += FDX12;
        CY2 += FDX20;
    }
}

void generic_char_2d(framebuffer_t& buffer, const font_t& font, int x, int y, char c, float color)
{
    auto data = font.find(c);
    if (!data)
        return;

    if (font.pack == font_pack_row_low)
    {
        for (int cx = 0; cx < font.w; ++cx)
        {
            auto rows = data[cx];

            for (int cy = 0; cy < font.h; ++cy)
            {
                buffer.set(x + cx, y + cy, rows & (1 << cy) ? color : 0);
            }
        }
    }
    else if (font.pack == font_pack_row_high)
    {
        for (int cx = 0; cx < font.w; ++cx)
        {
            auto rows = data[cx];

            for (int cy = 0; cy < font.h; ++cy)
            {
                buffer.set(x + cx, y + cy, rows & (1 << (7 - cy)) ? color : 0);
            }
        }
    }
    else if (font.pack == font_pack_column_low)
    {
        for (int cy = 0; cy < font.h; ++cy)
        {
            auto column = data[cy];

            for (int cx = 0; cx < font.w; ++cx)
            {
                buffer.set(x + cx, y + cy, column & (1 << cx) ? color : 0);
            }
        }
    }
    else if (font.pack == font_pack_column_high)
    {
        for (int cy = 0; cy < font.h; ++cy)
        {
            auto column = data[cy];

            for (int cx = 0; cx < font.w; ++cx)
            {
                buffer.set(x + cx, y + cy, column & (1 << (7 - cx)) ? color : 0);
            }
        }
    }
}

// http://forum.devmaster.net/t/advanced-rasterization/6145
void generic_triangle_3d(framebuffer_t& buffer,
    float x0, float y0, float z0,
    float x1, float y1, float z1,
    float x2, float y2, float z2,
    float c0, float c1, float c2)
{
    // 28.4 fixed-point coordinates
    const int Y0 = (int)(y0 * 16.0f);
    const int Y1 = (int)(y1 * 16.0f);
    const int Y2 = (int)(y2 * 16.0f);

    const int X0 = (int)(x0 * 16.0f);
    const int X1 = (int)(x1 * 16.0f);
    const int X2 = (int)(x2 * 16.0f);

    // Deltas
    const int DX01 = X0 - X1;
    const int DX12 = X1 - X2;
    const int DX20 = X2 - X0;

    const int DY01 = Y0 - Y1;
    const int DY12 = Y1 - Y2;
    const int DY20 = Y2 - Y0;

    // Fixed-point deltas
    const int FDX01 = DX01 << 4;
    const int FDX12 = DX12 << 4;
    const int FDX20 = DX20 << 4;

    const int FDY01 = DY01 << 4;
    const int FDY12 = DY12 << 4;
    const int FDY20 = DY20 << 4;

    // Bounding rectangle
    int minx = (min(X0, X1, X2) + 0x0F) >> 4;
    int maxx = (max(X0, X1, X2) + 0x0F) >> 4;
    int miny = (min(Y0, Y1, Y2) + 0x0F) >> 4;
    int maxy = (max(Y0, Y1, Y2) + 0x0F) >> 4;

    if (minx >= buffer.width  || maxx < 0) return;
    if (miny >= buffer.height || maxy < 0) return;

    // Half-edge constants
    int C0 = DY01 * X0 - DX01 * Y0;
    int C1 = DY12 * X1 - DX12 * Y1;
    int C2 = DY20 * X2 - DX20 * Y2;

    // Correct for fill convention
    if (DY01 < 0 || (DY01 == 0 && DX01 > 0)) C0++;
    if (DY12 < 0 || (DY12 == 0 && DX12 > 0)) C1++;
    if (DY20 < 0 || (DY20 == 0 && DX20 > 0)) C2++;

    int CY0 = C0 + DX01 * (miny << 4) - DY01 * (minx << 4);
    int CY1 = C1 + DX12 * (miny << 4) - DY12 * (minx << 4);
    int CY2 = C2 + DX20 * (miny << 4) - DY20 * (minx << 4);

    if (miny < 0)
    {
        auto diff = -miny;
        CY0 += FDX01 * diff;
        CY1 += FDX12 * diff;
        CY2 += FDX20 * diff;
        miny = 0;
    }

    if (maxy >= buffer.height)
        maxy = buffer.height - 1;

    const auto offset_x = (minx < 0) ? -minx : 0;
    if (offset_x)
        minx = 0;

    const auto C  = CY0 + CY1 + CY2;
    if (C == 0)
        return;

    const auto iC  = 1.0f / C;
    const auto iZ0 = 1.0f / z0;
    const auto iZ1 = 1.0f / z1;
    const auto iZ2 = 1.0f / z2;

    for (int y = miny; y < maxy; y++)
    {
        int CX0 = CY0 - FDY01 * offset_x;
        int CX1 = CY1 - FDY12 * offset_x;
        int CX2 = CY2 - FDY20 * offset_x;

        for (int x = minx; x < maxx; x++)
        {
            {
                if (CX0 > 0 && CX1 > 0 && CX2 > 0)
                {
                    auto c =         (c2  * CX0 + c0  * CX1 + c1  * CX2) * iC;
                    auto z = 1.0f / ((iZ2 * CX0 + iZ0 * CX1 + iZ1 * CX2) * iC);

                    if (z >= 0.0f && z <= 1.0f)
                        buffer.set(x, y, z, c);
                }
            }

            CX0 -= FDY01;
            CX1 -= FDY12;
            CX2 -= FDY20;
        }

        CY0 += FDX01;
        CY1 += FDX12;
        CY2 += FDX20;
    }
}
