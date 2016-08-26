#pragma once

enum font_pack_t
{
    font_pack_row_low,
    font_pack_row_high,
    font_pack_column_low,
    font_pack_column_high
};

struct font_range_t
{
    int start, end;
    int offset;
};

struct font_t
{
    typedef unsigned char byte;

    int                 w;
    int                 h;
    const byte*         data;
    font_pack_t         pack;
    const font_range_t* ranges;
    const int           range_count;

    const byte* find(char c) const;
};

const font_t& get_font_5x7();
const font_t& get_font_8x8();
const font_t& get_font_8x13();