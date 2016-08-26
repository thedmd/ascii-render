// Pixel Format Conversion Routines
// Copyright © 2004-2007 Glenn Fiedler
// Part of the PixelToaster Framebuffer Library - http://www.pixeltoaster.com

#ifndef PIXELTOASTER_CONVERSION_H
#define PIXELTOASTER_CONVERSION_H

#include "PixelToaster.h"

#ifndef PIXELTOASTER_NO_CRT
#include <memory.h>
#endif

namespace PixelToaster
{
	// floating point tricks!

    union FloatInteger
    {
        float f;
        int i;
    };

    inline integer32 clamp_positive(integer32 value)
 		{
			return (value - (value & (((int)value)>>31)));
		}

    inline integer32 clamped_fraction_8( float input )
    {
        FloatInteger value;

        value.f = input;
				value.i = clamp_positive( value.i );

        if ( value.i >= 0x3F7FFFFF )
            return 0x07F8000;

        value.f += 1.0f;

        return value.i & 0x07F8000;
    }

    inline integer32 clamped_fraction_6( float input )
    {
        FloatInteger value;

        value.f = input;
 				value.i = clamp_positive( value.i );

        if ( value.i >= 0x3F7FFFFF )
            return 0x07E0000;

        value.f += 1.0f;

        return value.i & 0x07E0000;
    }

    inline integer32 clamped_fraction_5( float input )
    {
        FloatInteger value;

        value.f = input;
				value.i = clamp_positive( value.i );

        if ( value.i >= 0x3F7FFFFF )
            return 0x07C0000;

        value.f += 1.0f;

        return value.i & 0x07C0000;
    }

		inline float uint8ToFloat( integer8 input )
		{
		  FloatInteger value;

			value.i = input | ( 142L << 23 );
			value.f -= 32768.0f;

			return value.f;
		}

	// floating point conversion routines

	inline void convert_XBGRFFFF_to_XRGB8888( const Pixel source[], integer32 destination[], unsigned int count )
    {
        for ( unsigned int i = 0; i < count; ++i )
        {
            const integer32 r = clamped_fraction_8( source[i].r ) << 1;
            const integer32 g = clamped_fraction_8( source[i].g ) >> 7;
            const integer32 b = clamped_fraction_8( source[i].b ) >> 15;

            destination[i] = r | g | b;
        }
    }

    inline void convert_XRGB8888_to_XBGRFFFF( const integer32 source[], Pixel destination[], unsigned int count )
    {
        for ( unsigned int i = 0; i < count; ++i )
        {
					destination[i].r = uint8ToFloat( source[i] >> 16 );
					destination[i].g = uint8ToFloat( source[i] >> 8 );
					destination[i].b = uint8ToFloat( source[i] );
        }
    }

    inline void convert_XBGRFFFF_to_XBGR8888( const Pixel source[], integer32 destination[], unsigned int count )
    {
        for ( unsigned int i = 0; i < count; ++i )
        {
            const integer32 r = clamped_fraction_8( source[i].r ) >> 15;
            const integer32 g = clamped_fraction_8( source[i].g ) >> 7;
            const integer32 b = clamped_fraction_8( source[i].b ) << 1;

            destination[i] = r | g | b;
        }
    }

    inline void convert_XBGR8888_to_XBGRFFFF( const integer32 source[], Pixel destination[], unsigned int count )
    {
        for ( unsigned int i = 0; i < count; ++i )
        {
            destination[i].r = uint8ToFloat( source[i] );
            destination[i].g = uint8ToFloat( source[i] >> 8 );
            destination[i].b = uint8ToFloat( source[i] >> 16 );
        }
    }

    inline void convert_XBGRFFFF_to_RGB888( const Pixel source[], integer8 destination[], unsigned int count )
    {
        for ( unsigned int i = 0; i < count; ++i )
        {
            const integer32 r = clamped_fraction_8( source[i].r ) >> 15;
            const integer32 g = clamped_fraction_8( source[i].g ) >> 15;
            const integer32 b = clamped_fraction_8( source[i].b ) >> 15;

            destination[0] = (integer8) r;
            destination[1] = (integer8) g;
            destination[2] = (integer8) b;

            destination += 3;
        }
    }

    inline void convert_RGB888_to_XBGRFFFF( const integer8 source[], Pixel destination[], unsigned int count )
    {
        for ( unsigned int i = 0; i < count; ++i )
        {
						destination[i].r = uint8ToFloat( source[0] );
						destination[i].g = uint8ToFloat( source[1] );
						destination[i].b = uint8ToFloat( source[2] );

            source += 3;
        }
    }

    inline void convert_XBGRFFFF_to_BGR888( const Pixel source[], integer8 destination[], unsigned int count )
    {
        for ( unsigned int i = 0; i < count; ++i )
        {
            const integer32 r = clamped_fraction_8( source[i].r ) >> 15;
            const integer32 g = clamped_fraction_8( source[i].g ) >> 15;
            const integer32 b = clamped_fraction_8( source[i].b ) >> 15;

            destination[0] = (integer8) b;
            destination[1] = (integer8) g;
            destination[2] = (integer8) r;

            destination += 3;
        }
    }

    inline void convert_BGR888_to_XBGRFFFF( const integer8 source[], Pixel destination[], unsigned int count )
    {
        for ( unsigned int i = 0;  i < count; ++i )
        {
						destination[i].r = uint8ToFloat( source[2] );
						destination[i].g = uint8ToFloat( source[1] );
						destination[i].b = uint8ToFloat( source[0] );

            source += 3;
        }
    }

    inline void convert_XBGRFFFF_to_RGB565( const Pixel source[], integer16 destination[], unsigned int count )
    {
        for ( unsigned int i = 0; i < count; ++i )
        {
            const integer32 r = clamped_fraction_5( source[i].r ) >> 7;
            const integer32 g = clamped_fraction_6( source[i].g ) >> 12;
            const integer32 b = clamped_fraction_5( source[i].b ) >> 18;

            destination[i] = (integer16) ( r | g | b );
        }
    }

    inline void convert_RGB565_to_XBGRFFFF( const integer16 source[], Pixel destination[], unsigned int count )
    {
        for ( unsigned int i = 0; i < count; ++i )
        {
            const integer32 color = (integer32) source[i];

            const integer8 r = ( color & 0x0000F800 ) >> 8;
            const integer8 g = ( color & 0x000007E0 ) >> 3;
            const integer8 b = ( color & 0x0000001F ) << 3;

						destination[i].r = uint8ToFloat( r );
						destination[i].g = uint8ToFloat( g );
						destination[i].b = uint8ToFloat( b );
        }
    }

    inline void convert_XBGRFFFF_to_BGR565( const Pixel source[], integer16 destination[], unsigned int count )
    {
        for ( unsigned int i = 0; i < count; ++i )
        {
            const integer32 r = clamped_fraction_5( source[i].r ) >> 18;
            const integer32 g = clamped_fraction_6( source[i].g ) >> 12;
            const integer32 b = clamped_fraction_5( source[i].b ) >> 7;

            destination[i] = (integer16) ( r | g | b );
        }
    }

    inline void convert_BGR565_to_XBGRFFFF( const integer16 source[], Pixel destination[], unsigned int count )
    {
        for ( unsigned int i = 0; i < count; ++i )
        {
            const integer32 color = (integer32) source[i];

            const integer8 b = ( color & 0x0000F800 ) >> 8;
            const integer8 g = ( color & 0x000007E0 ) >> 3;
            const integer8 r = ( color & 0x0000001F ) << 3;

						destination[i].r = uint8ToFloat( r );
						destination[i].g = uint8ToFloat( g );
						destination[i].b = uint8ToFloat( b );
        }
    }

    inline void convert_XBGRFFFF_to_XRGB1555( const Pixel source[], integer16 destination[], unsigned int count )
    {
        for ( unsigned int i = 0; i < count; ++i )
        {
            const integer32 r = clamped_fraction_5( source[i].r ) >> 8;
            const integer32 g = clamped_fraction_5( source[i].g ) >> 13;
            const integer32 b = clamped_fraction_5( source[i].b ) >> 18;

            destination[i] = (integer16) ( r | g | b );
        }
    }

    inline void convert_XRGB1555_to_XBGRFFFF( const integer16 source[], Pixel destination[], unsigned int count )
    {
        for ( unsigned int i = 0; i < count; ++i )
        {
            const integer32 color = (integer32) source[i];

            const integer8 r = ( color & 0x00007C00 ) >> 7;
            const integer8 g = ( color & 0x000003E0 ) >> 2;
            const integer8 b = ( color & 0x0000001F ) << 3;

						destination[i].r = uint8ToFloat( r );
						destination[i].g = uint8ToFloat( g );
						destination[i].b = uint8ToFloat( b );
        }
    }

    inline void convert_XBGRFFFF_to_XBGR1555( const Pixel source[], integer16 destination[], unsigned int count )
    {
        for ( unsigned int i = 0; i < count; ++i )
        {
            const integer32 r = clamped_fraction_5( source[i].r ) >> 18;
            const integer32 g = clamped_fraction_5( source[i].g ) >> 13;
            const integer32 b = clamped_fraction_5( source[i].b ) >> 8;

            destination[i] = (integer16) ( r | g | b );
        }
    }

    inline void convert_XBGR1555_to_XBGRFFFF( const integer16 source[], Pixel destination[], unsigned int count )
    {
        for ( unsigned int i = 0; i < count; ++i )
        {
            const integer32 color = (integer32) source[i];

            const integer8 b = ( color & 0x00007C00 ) >> 7;
            const integer8 g = ( color & 0x000003E0 ) >> 2;
            const integer8 r = ( color & 0x0000001F ) << 3;

						destination[i].r = uint8ToFloat( r );
						destination[i].g = uint8ToFloat( g );
						destination[i].b = uint8ToFloat( b );
        }
    }

    // integer to integer converters

    inline void convert_XRGB8888_to_XBGR8888( const integer32 source[], integer32 destination[], unsigned int count )
    {
        for ( unsigned int i = 0; i < count; ++i )
        {
            integer32 r = ( source[i] & 0x00FF0000 ) >> 16;
            integer32 g = ( source[i] & 0x0000FF00 );
            integer32 b = ( source[i] & 0x000000FF ) << 16;
            destination[i] = r | g | b;
        }
    }

    inline void convert_XBGR8888_to_XRGB8888( const integer32 source[], integer32 destination[], unsigned int count )
    {
        convert_XRGB8888_to_XBGR8888( source, destination, count );
    }

    inline void convert_XRGB8888_to_RGB888( const integer32 source[], integer8 destination[], unsigned int count )
    {
        for ( unsigned int i = 0; i < count; ++i )
        {
            destination[0] = (integer8) ( ( source[i] & 0x00FF0000 ) >> 16 );
            destination[1] = (integer8) ( ( source[i] & 0x0000FF00 ) >> 8 );
            destination[2] = (integer8) ( ( source[i] & 0x000000FF ) );
            destination += 3;
        }
    }

    inline void convert_RGB888_to_XRGB8888( const integer8 source[], integer32 destination[], unsigned int count )
    {
        for ( unsigned int i = 0; i < count; ++i )
        {
            integer32 r = source[0];
            integer32 g = source[1];
            integer32 b = source[2];

            destination[i] = ( r << 16 ) | ( g << 8 ) | b;

            source += 3;
        }
    }

    inline void convert_XRGB8888_to_BGR888( const integer32 source[], integer8 destination[], unsigned int count )
    {
        for ( unsigned int i = 0; i < count; ++i )
        {
            destination[0] = (integer8) ( (source[i] & 0x000000FF) );
            destination[1] = (integer8) ( (source[i] & 0x0000FF00) >> 8 );
            destination[2] = (integer8) ( (source[i] & 0x00FF0000) >> 16 );
            destination += 3;
        }
    }

    inline void convert_BGR888_to_XRGB8888( const integer8 source[], integer32 destination[], unsigned int count )
    {
        for ( unsigned int i = 0; i < count; ++i )
        {
            integer32 r = source[0];
            integer32 g = source[1];
            integer32 b = source[2];

            destination[i] = ( b << 16 ) | ( g << 8 ) | r;

            source += 3;
        }
    }

    inline void convert_XRGB8888_to_RGB565( const integer32 source[], integer16 destination[], unsigned int count )
    {
        for ( unsigned int i = 0; i < count; ++i )
        {
            integer32 r = ( source[i] & 0x00F80000 ) >> 8;
            integer32 g = ( source[i] & 0x0000FC00 ) >> 5;
            integer32 b = ( source[i] & 0x000000F8 ) >> 3;

            destination[i] = (integer16) ( r | g | b );
        }
    }

    inline void convert_RGB565_to_XRGB8888( const integer16 source[], integer32 destination[], unsigned int count )
    {
        for ( unsigned int i = 0; i < count; ++i )
        {
            integer32 color = (integer32) source[i];

            integer32 r = ( color & 0x0000F800 ) << 8;
            integer32 g = ( color & 0x000007E0 ) << 5;
            integer32 b = ( color & 0x0000001F ) << 3;

            destination[i] = r | g | b;
        }
    }

    inline void convert_XRGB8888_to_BGR565( const integer32 source[], integer16 destination[], unsigned int count )
    {
        for ( unsigned int i = 0; i < count; ++i )
        {
            integer32 r = ( source[i] & 0x00F80000 ) >> 19;
            integer32 g = ( source[i] & 0x0000FC00 ) >> 5;
            integer32 b = ( source[i] & 0x000000F8 ) << 8;

            destination[i] = (integer16) ( r | g | b );
        }
    }

    inline void convert_BGR565_to_XRGB8888( const integer16 source[], integer32 destination[], unsigned int count )
    {
        for ( unsigned int i = 0; i < count; ++i )
        {
            integer32 color = (integer32) source[i];

            integer32 r = ( ( color & 0x0000F800 ) << 8 ) >> 16;
            integer32 g = ( ( color & 0x000007E0 ) << 5 );
            integer32 b = ( ( color & 0x0000001F ) << 3 ) << 16;

            destination[i] = r | g | b;
        }
    }

    inline void convert_XRGB8888_to_XRGB1555( const integer32 source[], integer16 destination[], unsigned int count )
    {
        for ( unsigned int i = 0; i < count; ++i )
        {
            integer32 r = ( source[i] & 0x00F80000 ) >> 9;
            integer32 g = ( source[i] & 0x0000F800 ) >> 6;
            integer32 b = ( source[i] & 0x000000F8 ) >> 3;

            destination[i] = (integer16) ( r | g | b );
        }
    }

    inline void convert_XRGB1555_to_XRGB8888( const integer16 source[], integer32 destination[], unsigned int count )
    {
        for ( unsigned int i = 0; i < count; ++i )
        {
            integer32 color = (integer32) source[i];

            integer32 r = ( color & 0x00007C00 ) << 9;
            integer32 g = ( color & 0x000003E0 ) << 6;
            integer32 b = ( color & 0x0000001F ) << 3;

            destination[i] = r | g | b;
        }
    }

    inline void convert_XRGB8888_to_XBGR1555( const integer32 source[], integer16 destination[], unsigned int count )
    {
        for ( unsigned int i = 0; i < count; ++i )
        {
            integer32 r = ( source[i] & 0x00F80000 ) >> 19;
            integer32 g = ( source[i] & 0x0000F800 ) >> 6;
            integer32 b = ( source[i] & 0x000000F8 ) << 7;

            destination[i] = (integer16) ( r | g | b );
        }
    }

    inline void convert_XBGR1555_to_XRGB8888( const integer16 source[], integer32 destination[], unsigned int count )
    {
        for ( unsigned int i = 0; i < count; ++i )
        {
            integer32 color = (integer32) source[i];

            integer32 r = ( color & 0x00007C00 ) >> 7;
            integer32 g = ( color & 0x000003E0 ) << 6;
            integer32 b = ( color & 0x0000001F ) << 19;

            destination[i] = r | g | b;
        }
    }

    // copy converters

    inline void convert_XRGB8888_to_XRGB8888( const integer32 source[], integer32 destination[], unsigned int count )
    {
		#ifndef PIXELTOASTER_NO_CRT
       	memcpy( destination, source, count * 4 );
		#else
		for ( unsigned int i = 0; i < count; ++i )
			destination[i] = source[i];
		#endif
    }

    inline void convert_XBGRFFFF_to_XBGRFFFF( const Pixel source[], Pixel destination[], unsigned int count )
    {
		#ifndef PIXELTOASTER_NO_CRT
        memcpy( destination, source, count * 16 );
		#else
		for ( unsigned int i = 0; i < count; ++i )
			destination[i] = source[i];
		#endif
    }

	// declare set of converter classes

    class ConverterAdapter : public Converter
    {
        void begin() {};
        void end() {}
    };

	#define PIXELTOASTER_CONVERTER( type, source_type, destination_type )								\
																										\
	class Converter_##type : public ConverterAdapter													\
    {																									\
        void convert( const void * source, void * destination, int pixels )								\
        {																								\
			convert_##type( (const source_type*) source, (destination_type*) destination, pixels );		\
        }																								\
    };																									\

	PIXELTOASTER_CONVERTER( XBGRFFFF_to_XBGRFFFF, Pixel, Pixel);
	PIXELTOASTER_CONVERTER( XBGRFFFF_to_XRGB8888, Pixel, integer32 );
	PIXELTOASTER_CONVERTER( XBGRFFFF_to_XBGR8888, Pixel, integer32 );
	PIXELTOASTER_CONVERTER( XBGRFFFF_to_RGB888, Pixel, integer8 );
	PIXELTOASTER_CONVERTER( XBGRFFFF_to_BGR888, Pixel, integer8 );
	PIXELTOASTER_CONVERTER( XBGRFFFF_to_RGB565, Pixel, integer16 );
	PIXELTOASTER_CONVERTER( XBGRFFFF_to_BGR565, Pixel, integer16 );
	PIXELTOASTER_CONVERTER( XBGRFFFF_to_XRGB1555, Pixel, integer16 );
	PIXELTOASTER_CONVERTER( XBGRFFFF_to_XBGR1555, Pixel, integer16 );

	PIXELTOASTER_CONVERTER( XRGB8888_to_XBGRFFFF, integer32, Pixel );
	PIXELTOASTER_CONVERTER( XRGB8888_to_XRGB8888, integer32, integer32 );
	PIXELTOASTER_CONVERTER( XRGB8888_to_XBGR8888, integer32, integer32 );
	PIXELTOASTER_CONVERTER( XRGB8888_to_RGB888, integer32, integer8 );
	PIXELTOASTER_CONVERTER( XRGB8888_to_BGR888, integer32, integer8 );
	PIXELTOASTER_CONVERTER( XRGB8888_to_RGB565, integer32, integer16 );
	PIXELTOASTER_CONVERTER( XRGB8888_to_BGR565, integer32, integer16 );
	PIXELTOASTER_CONVERTER( XRGB8888_to_XRGB1555, integer32, integer16 );
	PIXELTOASTER_CONVERTER( XRGB8888_to_XBGR1555, integer32, integer16 );

	#undef CONVERTER
}

#endif
