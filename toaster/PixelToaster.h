/*
 	PixelToaster Framebuffer Library.

	Copyright © 2004-2007 Glenn Fiedler

	This software is provided 'as-is', without any express or implied
	warranty. In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.

	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.

	3. This notice may not be removed or altered from any source distribution.

	Glenn Fiedler
	gaffer@gaffer.org
*/

#ifndef PIXELTOASTER_H
#define PIXELTOASTER_H

// current API version ( API is not allowed to change in point releases )

#define PIXELTOASTER_VERSION 1.5

// disable annoying visual c++ warnings

#ifdef _MSC_VER
#pragma warning( disable : 4100 )		// warning C4100: unreferenced formal parameter
#pragma warning( disable : 4201 )		// warning C4201: nonstandard extension used : nameless struct/union
#pragma warning( disable : 4996 )		// warning C4201: stupid visual c++ deprecated stuff
#endif

// platforms

#define PIXELTOASTER_NULL 0
#define PIXELTOASTER_APPLE 1
#define PIXELTOASTER_UNIX 2
#define PIXELTOASTER_WINDOWS 3

#if defined(PLATFORM_WINDOWS)
	#define PIXELTOASTER_PLATFORM PIXELTOASTER_WINDOWS
#elif defined(PLATFORM_APPLE)
	#define PIXELTOASTER_PLATFORM PIXELTOASTER_APPLE
#elif defined(PLATFORM_UNIX)
	#define PIXELTOASTER_PLATFORM PIXELTOASTER_UNIX
#elif defined(PLATFORM_NULL)
	#define PIXELTOASTER_PLATFORM PIXELTOASTER_NULL
#endif

#ifndef PIXELTOASTER_PLATFORM
	#if defined(_WIN32)
		#define PIXELTOASTER_PLATFORM PIXELTOASTER_WINDOWS
	#elif defined(__APPLE__)
		#define PIXELTOASTER_PLATFORM PIXELTOASTER_APPLE
	#elif defined(linux) || defined(__linux) || defined(__linux__) || defined(__CYGWIN__)
		#define PIXELTOASTER_PLATFORM PIXELTOASTER_UNIX
	#else
		#define PIXELTOASTER_PLATFORM PIXELTOASTER_NULL
	#endif
#endif

// 32 or 64 bits?

#if defined(__LP64__) || defined(__64BIT__) || defined(_LP64) || (__WORDSIZE == 64)
	#define PIXELTOASTER_64BIT
#endif

// endianness

#if defined(__LITTLE_ENDIAN__)
	#define PIXELTOASTER_LITTLE_ENDIAN
#endif

#if defined(__BIG_ENDIAN__)
	#define PIXELTOASTER_BIG_ENDIAN
#endif

#if !defined(PIXELTOASTER_LITTLE_ENDIAN) && !defined (PIXELTOASTER_BIG_ENDIAN)
	#if defined(PLATFORM_WINDOWS)
		#define PIXELTOASTER_LITTLE_ENDIAN
	#elif defined(PLATFORM_APPLE)
		#define PIXELTOASTER_BIG_ENDIAN
	#else
		#define PIXELTOASTER_LITTLE_ENDIAN
	#endif
#endif

#if defined(PIXELTOASTER_LITTLE_ENDIAN) && defined(PIXELTOASTER_BIG_ENDIAN)
	#error cannot define both big and little endian!
#endif

#if !defined(PIXELTOASTER_LITTLE_ENDIAN) && !defined(PIXELTOASTER_BIG_ENDIAN)
	#error endianness not defined
#endif

// dynamic linking

#if defined(_WIN32) && defined(PIXELTOASTER_DYNAMIC)
    #ifdef PIXELTOASTER_DLL
		#define PIXELTOASTER_API __declspec(dllexport)
    #else
		#define PIXELTOASTER_API __declspec(dllimport)
    #endif
#else
    #define PIXELTOASTER_API
#endif

// executable size optimization

#ifdef PIXELTOASTER_TINY
#define PIXELTOASTER_NO_STL
#define PIXELTOASTER_NO_CRT
#endif

#ifndef PIXELTOASTER_NO_STL
#include <vector>
#endif

#ifndef NULL
#define NULL 0
#endif

namespace PixelToaster
{
	#ifndef PIXELTOASTER_NO_STL
	using namespace std;
	#endif

    typedef unsigned int integer32;				///< unsigned 32 bit integer
    typedef unsigned short integer16;			///< unsigned 16 bit integer
    typedef unsigned char integer8;				///< unsigned 8 bit integer

	/** \brief Represents a pixel in floating point color mode.

		Each pixel is made up of three components: red, green and blue.

		In floating point color each component is represented by a floating point value.
		0.0 is minimum intensity and 1.0 is maximum intensity.

		Some examples:

			- black (0.0, 0.0, 0.0)
			- white (1.0, 1.0, 1.0)
			- grey (0.5, 0.5, 0.5)
			- red (1.0, 0.0, 0.0)
			- purple (1.0, 0.0, 1.0)
			- green (0.0, 1.0, 0.0)

		Any component less than zero is treated as black, and any value greater than 1.0
		is treated as full intensity. You do not need to clamp the component values manually!

		You can acheive special effects by exploiting the so called "high dynamic range" available in
		floating point color. See http://www.debevec.org for information on this technique.

		Also, please note that each floating point pixel contains an alpha value
		which you are free to use however you like. For example, it could be used as a z-buffer
		for a software renderer, or simply as 4 bytes of storage per pixel for any purpose.

		If you dont have any use for the alpha value, its safe to just ignore it.

		\note You can refer to this class simply as "Pixel" if you like because floating point color
		      is the default in Pixel Toaster. See the floating point example source code for details.
	**/

    class FloatingPointPixel
    {
    public:

		/// The default constructor sets the pixel to black.

		FloatingPointPixel()
		{
			r = 0.0f;
			g = 0.0f;
			b = 0.0f;
			a = 0.0f;
		}

		/// This convenience constructor lets you specify color and alpha values at creation

		FloatingPointPixel( float r, float g, float b, float a = 0.0f )
		{
			this->r = r;
			this->g = g;
			this->b = b;
			this->a = a;
		}

        float r;        ///< red component
        float g;        ///< green component
        float b;        ///< blue component
        float a;        ///< alpha component (unused)
    };

	/// since floating point color is the default, we setup a typedef to allow users to shortcut it just to "Pixel"

	typedef FloatingPointPixel Pixel;

	/** \brief Represents a pixel in truecolor mode.

		Each pixel consists of three 8 bit color values packed into a 32 bit integer.

		The color components are stored in the integer in rgb order. The high 8 bits are referred to as 'alpha' but are not used.

		Here are some examples:

			- black = 0x00000000
			- white = 0x00FFFFFF
			- grey = 0x007F7F7F
			- red = 0x00FF0000
			- purple = 0x00FF00FF
			- green = 0x0000FF00

		Obviously it can get quite tedious manipulating the integer directly, so this union gives you
		the option of directly accessing the integer, or manipulating each of the 8 bit component values
		directly:

		%TrueColorPixel pixel;<br>
		pixel.r = 0;<br>
		pixel.g = 255;<br>
		pixel.b = 0;<br>

		And voila, you have a green pixel. Beware though that unlike floating point color,
		truecolor does not clamp the components for you. You need to ensure that your values
		stay within the [0,255] range or they will wrap around and the pixel color will not
		be what you intended.

		\note The top 8 bits of the integer are unused and are referred to as 'alpha'.
		      You can use these bits for 8 bits of per-pixel storage, or ignore them completely,
		      they have absolutely no effect on the pixel color.

		\todo Finish documentation
	**/

	union TrueColorPixel
	{
		/// The default constructor sets the pixel to black.

		TrueColorPixel()
		{
			integer = 0;
		}

		/// This convenience constructor lets you specify color and alpha values at creation

		TrueColorPixel( integer8 r, integer8 g, integer8 b, integer8 a = 0 )
		{
			this->r = r;
			this->g = g;
			this->b = b;
			this->a = a;
		}

		/// Construct from an integer

		explicit TrueColorPixel( integer32 i )
		{
			integer = i;
		}

		integer32 integer;			///< you can work directly with the truecolor integer if you like, but be careful about endianness!

		struct
		{
			#ifdef PIXELTOASTER_LITTLE_ENDIAN

				integer8 b;			///< blue component
				integer8 g;			///< green component
				integer8 r;			///< red component
				integer8 a;			///< alpha component

			#else

				integer8 a;			///< alpha component
				integer8 r;			///< red component
				integer8 g;			///< green component
				integer8 b;			///< blue component

			#endif
		};
	};

	/** \brief Lets you chose between floating point and truecolor when you open a display.

		Floating point color represents each pixel as four floating point values, while
		truecolor packs the color information into a 32bit integer. See FloatingPointPixel and TrueColorPixel for more information.

		Please note that this class is just a simple wrapper around an enumeration.
		This is done so that you can refer to modes as Mode::TrueColor and Mode::FloatingPoint in your code.

		You can assign and compare instances of this class as if they were the enumeration itself.

		Here is some example code to demonstrate:

		\code

Mode mode = Mode::FloatingPoint;

if ( mode != Mode::TrueColor )
{
	mode = Mode::TrueColor;
}

switch ( mode )
{
	case Mode::FloatingPoint: print( "floating point mode" ); break;
	case Mode::TrueColor: print( "truecolor mode" ); break;
}

Mode a = Mode::FloatingPoint;
Mode b = Mode::TrueColor;

assert( a != b );
assert( ! (a == b) );

Display display( "mode example", 320, 240, Output::Windowed, mode );

if ( display.open() )
{
	assert( mode == display.mode() );
}

		\endcode

		\see Display::open and Display::mode
     **/

    class Mode
    {
    public:

        /// The internal enumeration wrapped by the Mode class.
		/// You should never need to use the enumeration type directly, only the Mode::TrueColor and Mode::FloatingPoint values.

        enum Enumeration
        {
			TrueColor,					///< pixels are represented as packed 32 bit integers. See TrueColorPixel for details.
            FloatingPoint				///< pixels are represented by four floating point values for. See FloatingPointPixel for details.
        };

        /// The mode default constuctor sets the enumeration value to FloatingPoint.
		/// Floating point color to considered to be the default throughout the Pixel Toaster API.

        Mode()
        {
            enumeration = FloatingPoint;
        }

		/// This constructor enables automatic conversion from the enumeration type to a mode object.
		/// For example: Mode mode = Mode::FloatingPoint;
        /// @param enumeration the enumeration value.

        Mode( Enumeration enumeration )
        {
            this->enumeration = enumeration;
        }

        /// Cast from mode object to enumeration.
        /// Allows you to treat this class as if it was the enumeration itself.
        /// This enables the ==, != operators, and the use of mode objects in a switch statement.

        operator Enumeration() const
        {
            return enumeration;
        }

    private:

        Enumeration enumeration;
    };

	// this is an internal class representing the set of supported pixel formats.
	// because conversion occurs automatically when you update the display the details of the underlying display format are hidden.
	// if we decide to expose the converter class as a publically supported class, then this class must also become public.

    class Format
    {
    public:

		/// The internal enumeration wrapped by the Format class.
		/// You should never need to use the enumeration type directly, just use the enumeration values such as Format::RGB565, Format::RGB888 etc.

        enum Enumeration
        {
            Unknown,        ///< unknown pixel format.
            XRGB8888,       ///< 32 bit truecolor. this is the native pixel format in Mode::TrueColor.
            XBGR8888,       ///< 32 bit truecolor in BGR order.
            RGB888,         ///< 24 bit truecolor.
            BGR888,         ///< 24 bit truecolor in BGR order.
            RGB565,         ///< 16 bit hicolor.
            BGR565,         ///< 16 bit hicolor in BGR order.
            XRGB1555,       ///< 15 bit hicolor.
            XBGR1555,       ///< 15 bit hicolor in BGR order.
			XBGRFFFF,		///< 128bit floating point color. this is the native pixel format in Mode::FloatingPoint.
        };

		/// The default constructor sets the enumeration value to Unknown.

        Format()
        {
            enumeration = Unknown;
        }

		/// This constructor enables automatic conversion from the enumeration type to a format object.
		/// For example: Format format = Format::RGB565;
        /// @param enumeration the enumeration value.

		Format( Enumeration enumeration )
        {
            this->enumeration = enumeration;
        }

        /// Cast from format object to enumeration.
        /// Allows you to treat this class as if it was the enumeration itself.
        /// This enables the ==, != operators, and the use of format objects in a switch statement.

        operator Enumeration() const
        {
            return enumeration;
        }

    private:

        Enumeration enumeration;
    };

    /** \brief Lets you chose between fullscreen and windowed when opening a display.

		Display output can be either "windowed" or "fullscreen". Windowed output opens a display window on the desktop
		and draws your pixels inside this window. Fullscreen output switches to the best display mode mode and fills the
		entire screen with your pixels.

		An additional "default" output type is used to indicate that you want to open a display and use the 'best'
		output mode for the current platform. This is a rather nebulous concept. Currently the default output mode just
		maps to windowed mode, but this may change in the future. The bottom line is that if you absolutely require
		fullscreen or windowed output, you should specify this, or the display will just do whatever it thinks is best.

		Here is some example code to demonstrate output use:

		\code

Output output = Output::Fullscreen;

if ( output != Output::Windowed )
{
	output = Output::Windowed;
}

switch ( output )
{
	case Output::Default: print( "default output" ); break;
	case Output::Windowed: print( "windowed output" ); break;
	case Output::Fullscreen: print( "fullscreen output" ); break;
}

Output a = Mode::Windowed
Output b = Mode::Fullscreen;

assert( a != b );
assert( ! (a == b) );

Display display( "output example", 320, 240, output );

if ( display.open() )
{
	assert( output == display.output() );
}

display.close();

display.open( "default output example", 320, 240, Output::Default );

if ( display.open() )
{
	assert( display.output != Output::Default );

	switch ( output )
	{
		case Output::Windowed: print( "default output is windowed" ); break;
		case Output::Fullscreen: print( "default output is fullscreen" ); break;
	}
}

		\endcode

		\see Display::open and Display::output.
	 **/

    class Output
    {
    public:

        /// %Output enumeration.

        enum Enumeration
        {
            Default,            ///< default output. let the display choose between windowed and fullscreen. windowed output is preferred if available.
            Windowed,           ///< windowed output. output pixels to a window.
            Fullscreen          ///< fullscreen output. switch to a fullscreen display mode.
        };

        /// The default constructor sets the enumeration value to Default.

        Output()
        {
            enumeration = Default;
        }

        /// This constructor enables automatic conversion from the enumeration type to an output object.
		/// @param enumeration the enumeration value.

        Output( Enumeration enumeration )
        {
            this->enumeration = enumeration;
        }

        /// Cast from output object to enumeration.
        /// Allows you to treat this class as if it was the enumeration itself.
        /// This enables the ==, != operators, and the use of output objects in a switch statement.

        operator Enumeration() const
        {
            return enumeration;
        }

    private:

        Enumeration enumeration;
    };

    /** \brief Describes the current mouse position and the state of the left, right and middle mouse buttons.

		This class is used by the Listener interface for each of the event callbacks for mouse input.

		\see Listener::onMouseButtonDown, Listener::onMouseButtonUp and Listener::onMouseMove.

		\note The mouse position is stored as floating point values because the coordinates are relative to the size
		      of the display, not the size of the window. So if you open a display with dimensions 100x100 then the user
			  resizes the window to 200x100, then you will get x mouse coordinates 100.5, 101.0, 101.5, 102.0, 102.5 etc...
	 **/

    class Mouse
    {
    public:

        /// Indicates which mouse buttons are currently being pressed.

        class Buttons
        {
        public:
            bool left;        ///< true if left button is pressed.
            bool middle;      ///< true if middle button is pressed.
            bool right;       ///< true if right button is pressed.
        };

        Buttons buttons;      ///< mouse button state. indicates which mouse buttons are currently pressed.
        float x;              ///< current mouse cursor x position. standard range is from 0 to display width - 1, from left to right. values outside this range occur when the user drags the mouse outside the display window.
        float y;              ///< current mouse cursor y position. standard range is from 0 to display height - 1, from top to bottom. values outside this range occur when the user drags the mouse outside the display window.
    };

    /** \brief Identifies a single key on the keyboard.

		For all intents and purposes you may treat an instance of this class as if
        it was the key code enumeration value itself.

	    Here is some example code to demonstrate:

		\code

Key key = Key::Escape;

if ( key != Key::Enter )
{
	key = Key::Space;
}

switch ( key )
{
	case Key::Enter: print( "enter key" ); break;
	case Key::Space: print( "space key" ); break;
	case Key::Escape: print( "escape key" ); break;
}

Key a = Key::A;
Key b = Key::B;

assert( a != b );
assert( ! (a == b) );

		\endcode

		\see Listener::onKeyDown, Listener::onKeyPressed and Listener::onKeyUp
	 **/

    class Key
    {
    public:

        /// Key code enumeration.
		/// You should never need to use this enumeration type directly, just use the enumeration values Key::Enter, Key::Left, Key::Escape etc...

        enum Code
        {
            Enter          = '\n',      ///< enter key
            BackSpace      = '\b',      ///< backspace key
            Tab            = '\t',      ///< tab key
            Cancel         = 0x03,      ///< cancel key
            Clear          = 0x0C,      ///< clear key
            Shift          = 0x10,      ///< shift key
            Control        = 0x11,      ///< control key
            Alt            = 0x12,      ///< alt key
            Pause          = 0x13,      ///< pause key
            CapsLock       = 0x14,      ///< capslock key
            Escape         = 0x1B,      ///< escape key
            Space          = 0x20,      ///< space key
            PageUp         = 0x21,      ///< page up key
            PageDown       = 0x22,      ///< page down key
            End            = 0x23,      ///< end key
            Home           = 0x24,      ///< home key
            Left           = 0x25,      ///< left key
            Up             = 0x26,      ///< up arrow key
            Right          = 0x27,      ///< right arrow key
            Down           = 0x28,      ///< down arrow key
            Comma          = 0x2C,      ///< comma key ','
            Period         = 0x2E,      ///< period key '.'
            Slash          = 0x2F,      ///< slash key '/'
            Zero           = 0x30,      ///< zero key
            One            = 0x31,      ///< one key
            Two            = 0x32,      ///< two key
            Three          = 0x33,      ///< three key
            Four           = 0x34,      ///< four key
            Five           = 0x35,      ///< five key
            Six            = 0x36,      ///< six key
            Seven          = 0x37,      ///< seven key
            Eight          = 0x38,      ///< eight key
            Nine           = 0x39,      ///< nine key
            SemiColon      = 0x3B,      ///< semicolon key ';'
            Equals         = 0x3D,      ///< equals key '='
            A              = 0x41,      ///< a key
            B              = 0x42,      ///< b key
            C              = 0x43,      ///< c key
            D              = 0x44,      ///< d key
            E              = 0x45,      ///< e key
            F              = 0x46,      ///< f key
            G              = 0x47,      ///< g key
            H              = 0x48,      ///< h key
            I              = 0x49,      ///< i key
            J              = 0x4A,      ///< j key
            K              = 0x4B,      ///< k key
            L              = 0x4C,      ///< l key
            M              = 0x4D,      ///< m key
            N              = 0x4E,      ///< n key
            O              = 0x4F,      ///< o key
            P              = 0x50,      ///< p key
            Q              = 0x51,      ///< q key
            R              = 0x52,      ///< r key
            S              = 0x53,      ///< s key
            T              = 0x54,      ///< t key
            U              = 0x55,      ///< u key
            V              = 0x56,      ///< v key
            W              = 0x57,      ///< w key
            X              = 0x58,      ///< x key
            Y              = 0x59,      ///< y key
            Z              = 0x5A,      ///< z key
            OpenBracket    = 0x5B,      ///< open bracket key '['
            BackSlash      = 0x5C,      ///< back slash key '\'
            CloseBracket   = 0x5D,      ///< close bracket key ']'
            NumPad0        = 0x60,      ///< numpad 0 key
            NumPad1        = 0x61,      ///< numpad 1 key
            NumPad2        = 0x62,      ///< numpad 2 key
            NumPad3        = 0x63,      ///< numpad 3 key
            NumPad4        = 0x64,      ///< numpad 4 key
            NumPad5        = 0x65,      ///< numpad 5 key
            NumPad6        = 0x66,      ///< numpad 6 key
            NumPad7        = 0x67,      ///< numpad 7 key
            NumPad8        = 0x68,      ///< numpad 8 key
            NumPad9        = 0x69,      ///< numpad 9 key
            Multiply       = 0x6A,      ///< multiply key '*'
            Add            = 0x6B,      ///< add key '+'
            Separator      = 0x6C,      ///< separator key '-'
            Subtract       = 0x6D,      ///< subtract key '-'
            Decimal        = 0x6E,      ///< decimal key '.'
            Divide         = 0x6F,      ///< divide key '/'
            F1             = 0x70,      ///< F1 key
            F2             = 0x71,      ///< F2 key
            F3             = 0x72,      ///< F3 key
            F4             = 0x73,      ///< F4 key
            F5             = 0x74,      ///< F5 key
            F6             = 0x75,      ///< F6 key
            F7             = 0x76,      ///< F7 key
            F8             = 0x77,      ///< F8 key
            F9             = 0x78,      ///< F9 key
            F10            = 0x79,      ///< F10 key
            F11            = 0x7A,      ///< F11 key
            F12            = 0x7B,      ///< F12 key
            Delete         = 0x7F,      ///< delete key
            NumLock        = 0x90,      ///< numlock key
            ScrollLock     = 0x91,      ///< scroll lock key
            PrintScreen    = 0x9A,      ///< print screen key
            Insert         = 0x9B,      ///< insert key
            Help           = 0x9C,      ///< help key
            Meta           = 0x9D,      ///< meta key
            BackQuote      = 0xC0,      ///< backquote key
            Quote          = 0xDE,      ///< quote key
            Final          = 0x18,      ///< final key
            Convert        = 0x1C,      ///< convert key
            NonConvert     = 0x1D,      ///< non convert key
            Accept         = 0x1E,      ///< accept key
            ModeChange     = 0x1F,      ///< mode change key
            Kana           = 0x15,      ///< kana key
            Kanji          = 0x19,      ///< kanji key
            Undefined      = 0x0        ///< undefined key
        };

        /// The default constructor sets the key code to Undefined.

        Key()
        {
            code = Undefined;
        }

        /// Automatically converts from key code enumeration values to key objects.
        /// @param code the key code.

        Key( Code code )
        {
            this->code = code;
        }

        /// Cast from key object to code.
        /// Allows you to treat this class as if it was the code itself.
        /// This enables the ==, != operators, and the use of key objects in a switch statement.

        operator Code() const
        {
            return code;
        }

    private:

        Code code;
    };

	// Rectangular range of pixels: [xBegin, xEnd) x [yBegin, yEnd)
	//
	struct Rectangle
	{
		int xBegin;	///< first column in range
		int xEnd;	///< one past last column in range
		int yBegin;	///< first row in range
		int yEnd;	///< one past last row in range
		Rectangle(): xBegin(0), xEnd(0), yBegin(0), yEnd(0) {}
		Rectangle(int xb, int xe, int yb, int ye): xBegin(xb), xEnd(xe), yBegin(yb), yEnd(ye) {}
	};

	// internal factory methods

	PIXELTOASTER_API class DisplayInterface * createDisplay();
	PIXELTOASTER_API class TimerInterface * createTimer();
	PIXELTOASTER_API class Converter * requestConverter( Format source, Format destination );

	// internal display interface

    class DisplayInterface
    {
    public:

        virtual ~DisplayInterface() {};

        virtual bool open( const char title[], int width, int height, Output output = Output::Default, Mode mode = Mode::FloatingPoint ) = 0;
        virtual void close() = 0;

		virtual bool open() const = 0;

        virtual bool update( const FloatingPointPixel pixels[], const Rectangle* dirtyBox = 0 ) = 0;
        virtual bool update( const TrueColorPixel pixels[], const Rectangle* dirtyBox = 0 ) = 0;

        virtual void zoom( int factor ) = 0;

        virtual const char * title() const = 0;
		virtual void title( const char title[] ) = 0;
        virtual int width() const = 0;
        virtual int height() const = 0;
        virtual Mode mode() const = 0;
        virtual Output output() const = 0;

	    virtual void listener( class Listener * listener ) = 0;
		virtual class Listener * listener() const = 0;

	    virtual void wrapper( DisplayInterface * wrapper ) = 0;
		virtual DisplayInterface * wrapper() = 0;
	};

    /** \brief Provides the mechanism for getting your pixels up on the screen.

		Every Pixel Toaster application needs a display. Lets analyze a simple application to understand exactly what is going on:

		\code

Display display( "Example", 320, 240 );

vector<Pixel> pixels( 320 * 240 );

while ( display.open() )
{
	display.update( pixels );
}

		\endcode

		In a nutshell, the simple application does the following:

			- Create and open a display
			- Create an array of pixels to work with
			- Loop and update the pixels to the display

		All you need to do is implement your rendering code and do it each frame before you update the display.
		Each time display update is called the array of pixels are copied to the display, so you can see them!

		Now lets actually draw something. Here is the same code again, but this time each update we set a random pixel to the color blue:

		\code

const int width = 320;
const int height = 240;

Display display( "Example", width, height );

vector<Pixel> pixels( width, height );

while ( display.open() )
{
	const int x = rand() % width;
	const int y = rand() % height;

	pixels[x+y*width].b = 1.0f;

	display.update( pixels );
}
		\endcode
	 **/

	class Display : public DisplayInterface
    {
    public:

        /// Creates the display object but does not open the display.
        /// You need to call Display::open first before you can copy pixels to the display with Display::update.

        Display()
        {
            internal = createDisplay();
			internal->wrapper( this );
        }

		/// Create and open display in one step.
		/// This is equivalent to creating a display using the default constructor then calling Display::open.
		/// \see Display::open

		Display( const char title[], int width, int height, Output output = Output::Default, Mode mode = Mode::FloatingPoint )
		{
            internal = createDisplay();
			internal->wrapper( this );
			open( title, width, height, output, mode );
		}

        /// Destructor.
        /// Closes the display if it is still open.

        ~Display()
        {
            if ( internal )
            {
                delete internal;
                internal = 0;
            }
        }

        /// Open display.
        /// Opens the display by creating an output window or entering fullscreen mode.
        /// @param title the title of the display window or fullscreen application.
        /// @param width the width of the display in pixels.
        /// @param height the height of the display in pixels.
        /// @param output the output type of the display. you can choose between windowed output and fullscreen output, or you can leave it up to the display by passing in default.
        /// @param mode the mode of operation for the display. you can choose between true color mode and floating point color mode.
        /// @returns true if the display open was successful.

        bool open( const char title[], int width, int height, Output output = Output::Default, Mode mode = Mode::FloatingPoint )
        {
            if ( internal )
                return internal->open( title, width, height, output, mode );
            else
                return false;
        }

        /// Close display.
        /// Closes the display window or leaves fullscreen mode.
        /// This method is safe to call even if the display not open.

        void close()
        {
            if (internal)
                internal->close();
        }

		/// Check if display is open.

		bool open() const
		{
			if ( internal )
				return internal->open();
			else
				return false;
		}

        /// Update display with floating point pixels.
        /// The input pixels must be a linear array of size width x height, where width and height are the
        /// dimensions of the display as per the most recent successful call to Display::open.
        /// You can calculate the offset for a pixel in the linear array as follows: int offset = width*y + x;
        /// This is the correct update method to call when the display was opened in Mode::FloatingPoint,
        /// however it is safe to call this method even when operating in Mode::TrueColor if you wish.
		///
		/// The dirty box acts as a @b hint to the update function that only pixels inside that box have been changed
		/// since last update.  The update function does not need to respect this hint and may do a full update
		/// instead.  In particular, it will do this if it can avoid a buffer copy when matching formats are found.
		/// That's why you still need to provide a full buffer of pixels, and the ones outside dirtyBox should
		/// be the same pixels as the previous call.
		///
        /// @param pixels the pixels to copy to the screen.
 		/// @param dirtyBox range of pixels that have been changed since last call.
        /// @returns true if the update was successful.

        bool update( const class FloatingPointPixel pixels[], const Rectangle * dirtyBox = 0 )
        {
            if ( internal )
                return internal->update( pixels, dirtyBox );
            else
                return false;
        }

        /// Update display with truecolor pixels.
        /// The input pixels must be a linear array of size width x height, where width and height are the
        /// dimensions of the display as per the most recent successful call to Display::open.
        /// You can calculate the offset for a pixel in the linear array as follows: int offset = width*y + x;
        /// This is the natural update method to call when the display was opened in Mode::TrueColor,
        /// however it is safe to call this method even when operating in Mode::FloatingPoint if you wish.
 		///
		/// The dirty box acts as a @b hint to the update function that only pixels inside that box have been changed
		/// since last update.  The update function does not need to respect this hint and may do a full update
		/// instead.  In particular, it will do this if it can avoid a buffer copy when matching formats are found.
		/// That's why you still need to provide a full buffer of pixels, and the ones outside dirtyBox should
		/// be the same pixels as the previous call.
		///
        /// @param pixels the pixels to copy to the screen.
		/// @param dirtyBox range of pixels that have been changed since last call.
        /// @returns true if the update was successful.

        bool update( const TrueColorPixel pixels[], const Rectangle * dirtyBox = 0 )
        {
            if (internal)
                return internal->update( pixels, dirtyBox );
            else
                return false;
        }

#ifndef PIXELTOASTER_NO_STL

        /// Update display with standard vector of floating point pixels.
		/// This is just a helper method to make it a bit cleaner to pass a vector of pixels into the update.
        /// @param pixels the pixels to copy to the screen.
        /// @returns true if the update was successful.

		bool update( const vector<FloatingPointPixel> & pixels, const Rectangle * dirtyBox = 0 )
        {
			return update( &pixels[0], dirtyBox );
        }

        /// Update display with standard vector of truecolor pixels.
		/// This is just a helper method to make it a bit cleaner to pass a vector of pixels into the update.
        /// @param pixels the pixels to copy to the screen.
        /// @returns true if the update was successful.

		bool update( const vector<TrueColorPixel> & pixels, const Rectangle * dirtyBox = 0 )
        {
			return update( &pixels[0], dirtyBox );
        }

#endif

        void zoom( int factor )
        {
            if (internal)
                internal->zoom( factor );
        }

        /// Get display title

        const char * title() const
        {
            if (internal)
                return internal->title();
            else
                return "";
        }

		/// Set display title

		void title( const char title[] )
		{
			if (internal)
				internal->title(title);
		}

        /// Get display width

        int width() const
        {
            if ( internal )
                return internal->width();
            else
                return 0;
        }

        /// Get display height

        int height() const
        {
            if ( internal )
                return internal->height();
            else
                return 0;
        }

        /// Get display mode

        Mode mode() const
        {
            if ( internal )
                return internal->mode();
            else
                return Mode::FloatingPoint;
        }

        /// Get display output

        Output output() const
        {
            if ( internal )
                return internal->output();
            else
                return Output::Default;
        }

        /// Register a listener object.
		/// Implement the Listener interface and pass in an pointer to an instance of your object to recieve display events such as keyboard and mouse input.
		///	@param listener the listener object. pass in 0 if you want to remove the current listener.

        void listener( class Listener * listener )
        {
            if (internal)
                internal->listener( listener );
        }

		/// Get current listener object.
		/// @returns the currrent listener object pointer. null if there is no listener.

		class Listener * listener() const
		{
			if ( internal )
				return internal->listener();
			else
				return 0;
		}

		void wrapper( class DisplayInterface * wrapper )
		{
			// wrapper is always this
		}

		class DisplayInterface * wrapper()
		{
			return this;
		}

    private:

		DisplayInterface * internal;
    };

	// internal timer interface

    class TimerInterface
    {
    public:
        virtual ~TimerInterface() {};
        virtual void reset() = 0;
        virtual double time() = 0;
        virtual double delta() = 0;
        virtual double resolution() = 0;
        virtual void wait( double seconds ) = 0;
    };

    /** \brief A high resolution timer.

		The most common use for a timer is to keep track of the current time, and the delta time
		each frame you render to the display. This lets you make sure your program is framerate independent,
		in other words, it runs at the same speed no matter the framerate.

		Here is a simple example:

		\code

		Display display( "timer example", 320, 240 );

		Timer timer;

		while ( display.open() )
		{
			const double time = timer.time();
			const double delta = timer.delta();

			// advance animation forward by delta seconds...
		}

		\endcode

		\note The timer uses doubles instead of floats to ensure maximum precision is available.
	 **/

    class Timer
    {
    public:

        /// The default constructor sets the current time to zero and starts the timer.

        Timer()
        {
            internal = createTimer();
        }

        ~Timer()
        {
            delete internal;
			internal = NULL;
        }

        /// Resets current time to zero.

        void reset()
        {
            if ( internal )
                internal->reset();
        }

        /// Get current time.
        /// @returns current time in seconds.

        double time()
        {
            if ( internal )
                return internal->time();
            else
                return 0.0;
        }

        /// Get time elapsed since last call.
		/// \note if this is the first call to Timer::delta since you created the timer, or called reset, then delta time is equivalent to time elapsed since the timer was started.
        /// @returns delta time in seconds.

        double delta()
        {
            if ( internal )
                return internal->delta();
            else
                return 0.0;
        }

        /// Get timer resolution.
        /// The timer resolution is the smallest change in time that can be detected.
        /// @returns timer resolution in seconds.

        double resolution()
        {
            if ( internal )
                return internal->resolution();
            else
                return 0.0;
        }

        /// Wait for a period of time.
        /// @param seconds the number of seconds to wait before returning from this method.

        void wait( double seconds )
        {
            if ( internal )
                internal->wait( seconds );
        }

    private:

        TimerInterface * internal;
    };

    /** \brief Implement the listener interface and register with a display to receive events.

		This interface defines all the callback functions for display events.
		It gives you keyboard and mouse input, plus useful system events for window activation
		and close window requests.

		Use of the listener interface is optional. If you do not register a display listener,
		your program will respond to input in using the following default behavior:

			- Pressing the escape key closes the display.
			- If the user presses the platform close window key combinaton (Alt-F4 on windows, Apple-W on Mac) or clicks on the close icon in the title bar then the display is closed.
			- All other events are ignored.

		This avoids writing code to handle basic events when all you want is to open a display and draw pixels.

		But when you do want to listen to input, just implement this interface in your own class,
		and register your listener with the display using Display::listener.

		Here is a quick example to demonstrate:

		\code

class Application : public Listener
{
public:

	Application()
	{
		quit = false;
	}

    int run()
    {
		// open display

        const int width = 320;
        const int height = 240;

        if ( !display.open( "Listener Example", width, height ) )
			return 1;

		// register listener

        display.listener( this );

		// keep updating the display until we are ready to quit

        vector<Pixel> pixels( width * height );

        while ( !quit )
        {
			// ...

            display.update( pixels );
        }
    }

protected:

    void onKeyDown( DisplayInterface & display, Key key )
    {
        if ( key==Key::Escape )
            quit = true;

		return false;		// disable default key handlers
    }

    void onKeyPressed( DisplayInterface & display, Key key )
    {
		// ...
    }

    void onKeyUp( DisplayInterface & display, Key key )
    {
		// ...
    }

    void onMouseButtonDown( DisplayInterface & display, Mouse mouse )
    {
		// ...
    }

    void onMouseButtonUp( DisplayInterface & display, Mouse mouse )
    {
		// ...
    }

    void onMouseMove( DisplayInterface & display, Mouse mouse )
    {
		// ...
    }

    void onActivate( DisplayInterface & display, bool active )
    {
		// ...
    }

    void onClose( DisplayInterface & display )
    {
        return quit = true;			// returning true indicates that we want the display close to proceed
    }

private:

	Display display;
	bool quit;
};


int main()
{
    Application application;
    application.run();
}

		\endcode

		There can only be a single listener	registered per display object. Once you register a listener,
		all the default behavior goes away, and you are responsible for handling close requests and
		keyboard and mouse events. For example, if you register a listener but neglect to handle onClose,
		then your user will press Alt-F4 or click on the close button, and nothing will happen!

		\warning Never call Display::open, Display::close or Display::update inside any of these event callbacks!

		\see Display::listener
	 **/

    class Listener
    {
    public:

		virtual ~Listener() {};

		/// Called by display to ask if you want default key handlers to be applied,
		/// eg. Escape quits without you needing to do anything. default is true.
		/// override and return false if you dont want default key handlers.

		virtual bool defaultKeyHandlers() const { return true; }

        /// On key down.
        /// Called once only when a key is pressed and held.
		/// @param display the display sending the event
        /// @param key the key event data.

		virtual void onKeyDown( DisplayInterface & display, Key key ) { }

        /// On key pressed.
        /// Called multiple times while a key is pressed and held including the initial event.
		/// @param display the display sending the event
        /// @param key the key event data.

		virtual void onKeyPressed( DisplayInterface & display, Key key ) {}

        /// On key up.
        /// Called when a key is released.
		/// @param display the display sending the event
        /// @param key the key event data.

		virtual void onKeyUp( DisplayInterface & display, Key key ) {}

        /// On mouse button down.
        /// Called once only when a mouse button is initially pressed.
		/// @param display the display sending the event
        /// @param mouse the mouse event data.

		virtual void onMouseButtonDown( DisplayInterface & display, Mouse mouse ) {}

        /// On mouse button up.
        /// Called when a mouse button is released.
		/// @param display the display sending the event
        /// @param mouse the mouse event data.

		virtual void onMouseButtonUp( DisplayInterface & display, Mouse mouse ) {}

        /// On mouse move.
        /// Called when the mouse is moved.
		/// @param display the display sending the event
        /// @param mouse the mouse event data.

		virtual void onMouseMove( DisplayInterface & display, Mouse mouse ) {}

        /// On activate.
        /// Called when the display window is activated or deactivated.
		/// @param display the display sending the event
        /// @param active true if the window is being activated, false if it is being deactivated.

		virtual void onActivate( DisplayInterface & display, bool active ) {}

		/// On open.
		/// Called when a display is opened successfully.
		/// @param display the display sending the event

		virtual void onOpen( DisplayInterface & display ) {}

        /// On close.
        /// Called when the window has been requested to close by the user.
        /// You are responsible for responding to this event by quitting
        /// your application, otherwise the window will stay open and the
        /// application will keep running.
		/// If you return true, the display will close, if you return false it
		/// will remain open and the close will be ignored. true is default.
		/// @param display the display sending the event

		virtual bool onClose( DisplayInterface & display ) { return true; }
    };

	// internal converter interface

    class Converter
    {
    public:
		virtual ~Converter() {};
        virtual void begin() = 0;
        virtual void convert( const void * source, void * destination, int pixels ) = 0;
        virtual void end() = 0;
    };
}

#endif
