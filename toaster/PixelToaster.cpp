// PixelToaster Implementation
// Copyright © 2004-2007 Glenn Fiedler
// Part of the PixelToaster Framebuffer Library - http://www.pixeltoaster.com

#ifndef PIXELTOASTER_NO_CRT
#include <cassert>
#else
#define assert(condition)
#endif

#include "PixelToaster.h"
#include "PixelToasterCommon.h"
#include "PixelToasterConversion.h"


#if PIXELTOASTER_PLATFORM == PIXELTOASTER_UNIX
	#include "PixelToasterUnix.h"
	#define TimerClass UnixTimer
	#define DisplayClass UnixDisplay
#endif

#if PIXELTOASTER_PLATFORM == PIXELTOASTER_APPLE
	#include "PixelToasterApple.h"
	#define TimerClass AppleTimer
	#define DisplayClass AppleDisplay
#endif

#if PIXELTOASTER_PLATFORM == PIXELTOASTER_WINDOWS
	#include "PixelToasterWindows.h"
	#define TimerClass WindowsTimer
	#define DisplayClass WindowsDisplay
#endif

#if PIXELTOASTER_PLATFORM == PIXELTOASTER_NULL
	#define TimerClass PortableTimer
	#define DisplayClass DisplayAdapter
#endif


#if !defined( DisplayClass ) || !defined( TimerClass )
	#error unknown pixeltoaster platform!
#endif


PixelToaster::DisplayInterface * PixelToaster::createDisplay()
{
#ifdef DisplayClass
    return new DisplayClass();
#else
	return NULL;
#endif
}

PixelToaster::TimerInterface * PixelToaster::createTimer()
{
#ifdef TimerClass
    return new TimerClass();
#else
	return NULL;
#endif
}


PixelToaster::Converter_XBGRFFFF_to_XBGRFFFF 	converter_XBGRFFFF_to_XBGRFFFF;
PixelToaster::Converter_XBGRFFFF_to_XRGB8888 	converter_XBGRFFFF_to_XRGB8888;
PixelToaster::Converter_XBGRFFFF_to_XBGR8888 	converter_XBGRFFFF_to_XBGR8888;
PixelToaster::Converter_XBGRFFFF_to_RGB888 		converter_XBGRFFFF_to_RGB888;
PixelToaster::Converter_XBGRFFFF_to_BGR888 		converter_XBGRFFFF_to_BGR888;
PixelToaster::Converter_XBGRFFFF_to_RGB565 		converter_XBGRFFFF_to_RGB565;
PixelToaster::Converter_XBGRFFFF_to_BGR565 		converter_XBGRFFFF_to_BGR565;
PixelToaster::Converter_XBGRFFFF_to_XRGB1555 	converter_XBGRFFFF_to_XRGB1555;
PixelToaster::Converter_XBGRFFFF_to_XBGR1555 	converter_XBGRFFFF_to_XBGR1555;

PixelToaster::Converter_XRGB8888_to_XBGRFFFF 	converter_XRGB8888_to_XBGRFFFF;
PixelToaster::Converter_XRGB8888_to_XRGB8888 	converter_XRGB8888_to_XRGB8888;
PixelToaster::Converter_XRGB8888_to_XBGR8888 	converter_XRGB8888_to_XBGR8888;
PixelToaster::Converter_XRGB8888_to_RGB888 		converter_XRGB8888_to_RGB888;
PixelToaster::Converter_XRGB8888_to_BGR888 		converter_XRGB8888_to_BGR888;
PixelToaster::Converter_XRGB8888_to_RGB565 		converter_XRGB8888_to_RGB565;
PixelToaster::Converter_XRGB8888_to_BGR565 		converter_XRGB8888_to_BGR565;
PixelToaster::Converter_XRGB8888_to_XRGB1555 	converter_XRGB8888_to_XRGB1555;
PixelToaster::Converter_XRGB8888_to_XBGR1555 	converter_XRGB8888_to_XBGR1555;


PixelToaster::Converter * PixelToaster::requestConverter( PixelToaster::Format source, PixelToaster::Format destination )
{
    if ( source == Format::XBGRFFFF )
    {
        switch ( destination )
        {
            case Format::XBGRFFFF: 		return &converter_XBGRFFFF_to_XBGRFFFF;
            case Format::XRGB8888: 		return &converter_XBGRFFFF_to_XRGB8888;
            case Format::XBGR8888: 		return &converter_XBGRFFFF_to_XBGR8888;
            case Format::RGB888: 		return &converter_XBGRFFFF_to_RGB888;
            case Format::BGR888: 		return &converter_XBGRFFFF_to_BGR888;
            case Format::RGB565: 		return &converter_XBGRFFFF_to_RGB565;
            case Format::BGR565: 		return &converter_XBGRFFFF_to_BGR565;
            case Format::XRGB1555: 		return &converter_XBGRFFFF_to_XRGB1555;
            case Format::XBGR1555: 		return &converter_XBGRFFFF_to_XBGR1555;

			default:
				return NULL;
        }
    }
    else if ( source==Format::XRGB8888 )
    {
        switch ( destination )
        {
            case Format::XBGRFFFF: 		return &converter_XRGB8888_to_XBGRFFFF;
            case Format::XRGB8888: 		return &converter_XRGB8888_to_XRGB8888;
            case Format::XBGR8888: 		return &converter_XRGB8888_to_XBGR8888;
            case Format::RGB888: 		return &converter_XRGB8888_to_RGB888;
            case Format::BGR888: 		return &converter_XRGB8888_to_BGR888;
            case Format::RGB565: 		return &converter_XRGB8888_to_RGB565;
            case Format::BGR565: 		return &converter_XRGB8888_to_BGR565;
            case Format::XRGB1555: 		return &converter_XRGB8888_to_XRGB1555;
            case Format::XBGR1555: 		return &converter_XRGB8888_to_XBGR1555;

			default:
				return NULL;
        }
    }

	return NULL;
}
