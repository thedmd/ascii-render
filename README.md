# ascii-render
Simple ASCII renderer, because it's fun

![ascii-renderer](https://github.com/thedmd/ascii-render/blob/master/screenshots/ascii-renderer.jpg)

# About
Code was quickly hacked to get desired result. At first it used Windows Console to display output but constant obstacles thrown at me by WinAPI made me strip it off and use PixelToaster. It's great for showing framebuffer on screen.

What is done:
* half-space triangle rendering with grayscale gradient and z-buffer support
* a few simple drawing primitives (circle, lines, ellipse, triangle)
* fonts are easy to define
* text gradient is easy to define (`ascii_font_t{ get_font_8x8(),  " .',\";o%O8@#", 0 }`)
* you can render to regular framebuffer or to ascii framebuffer (screen bellow)
* simple meshes
* simple camera
* z-buffer
* no comments

![ascii-renderer](https://github.com/thedmd/ascii-render/blob/master/screenshots/grayscale-renderer.jpg)

# Building
Use Visual Studio 2015 to build provided solution file. Running on other platforms is possible because whole thing is rather platform agnostic and use PixelToaster to present output. Howere some tweaks may be needed.
