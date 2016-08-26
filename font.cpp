#include "font.h"

#define lengthof(x) (sizeof(x) / sizeof(*(x)))

const font_t::byte* font_t::find(char c) const
{
    for (int i = 0; i < range_count; ++i)
    {
        auto& range = ranges[i];

        if (c >= range.start && c < range.end)
        {
            auto stride = ((pack == font_pack_row_low) || (pack == font_pack_row_high)) ? w : h;

            return data + (range.offset + (c - range.start)) * stride;
        }
    }

    return nullptr;
}
