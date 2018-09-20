#include "ScreenOverlay.hpp"

#include <iostream>

#include <X11/extensions/shape.h>
#include <X11/extensions/Xfixes.h>

// Events for normal windows
#define BASIC_EVENT_MASK (StructureNotifyMask|ExposureMask|PropertyChangeMask|EnterWindowMask|LeaveWindowMask|KeyPressMask|KeyReleaseMask|KeymapStateMask)
#define NOT_PROPAGATE_MASK (KeyPressMask|KeyReleaseMask|ButtonPressMask|ButtonReleaseMask|PointerMotionMask|ButtonMotionMask)

XColor red, white, black;

ScreenOverlay::ScreenOverlay(Display* display, int screen, int width, int height) :
	m_display(display),
	m_screen(screen)
{
	XSetWindowAttributes window_attrs;
	XColor bg_color = createXColorFromRGBA(0, 0, 0, 0);

	Window root = DefaultRootWindow(display);
	Visual* visual = DefaultVisual(display, screen);

	XVisualInfo vis_info;
	XMatchVisualInfo(display, DefaultScreen(display), 32, TrueColor, &vis_info);

	XSetWindowAttributes attrs;
	attrs.background_pixmap = None;
	attrs.background_pixel = bg_color.pixel;
	attrs.border_pixel = 0;
	attrs.win_gravity = NorthWestGravity;
	attrs.bit_gravity = ForgetGravity;
	attrs.save_under = 1;
	attrs.event_mask = BASIC_EVENT_MASK;
	attrs.do_not_propagate_mask = NOT_PROPAGATE_MASK;
	attrs.override_redirect = 1; // OpenGL > 0
	attrs.colormap = XCreateColormap(display, DefaultRootWindow(display), vis_info.visual, AllocNone);

	unsigned long mask = CWColormap | CWBorderPixel | CWBackPixel | CWEventMask | CWWinGravity | CWBitGravity | CWSaveUnder | CWDontPropagate | CWOverrideRedirect;

	m_window = XCreateWindow(display, root, 0, 0, width, height, 0, vis_info.depth, InputOutput, vis_info.visual, mask, &attrs);

	XShapeCombineMask(display, m_window, ShapeInput, 0, 0, None, ShapeSet);

	#define SHAPE_MASK ShapeNotifyMask
	XShapeSelectInput(display, m_window, SHAPE_MASK);

	// Tell the window manager not to draw window borders (frame) or title
	window_attrs.override_redirect = 1;
	XChangeWindowAttributes(display, m_window, CWOverrideRedirect, &window_attrs);
	allowInputPassthrough(m_window);

	// Show the window
	XMapWindow(display, m_window);

	red = createXColorFromRGBA(255, 0, 0, 255);
	white = createXColorFromRGBA(255, 255, 255, 255);
	black = createXColorFromRGBA(0, 0, 0, 255);
}

void ScreenOverlay::drawText(const std::string& text, int x, int y)
{
	// XClearWindow(m_display, m_window);

	GC gc;
	XGCValues gcv;

	gc = XCreateGC(m_display, m_window, 0, 0);
	XSetBackground(m_display, gc, white.pixel);
	XSetForeground(m_display, gc, red.pixel);

	XFontStruct* font;
	const char* font_name = "9x15bold";
	font = XLoadQueryFont(m_display, font_name);
	// If the font could not be loaded, revert to the "fixed" font
	if (!font)
	{
		std::cerr << "Unable to load font " << font_name << ". Using fixed instead." << std::endl;
		font = XLoadQueryFont(m_display, "fixed");
	}
	XSetFont(m_display, gc, font->fid);

	XSetForeground(m_display, gc, black.pixel);
	int text_x = x + font->min_bounds.lbearing;
	int text_y = y - font->max_bounds.ascent;
	int text_width = XTextWidth(font, text.c_str(), text.size());
	int text_height = font->max_bounds.ascent + font->max_bounds.descent;
	XFillRectangle(m_display, m_window, gc, text_x, text_y, text_width, text_height);

	XSetForeground(m_display, gc, red.pixel);
	XDrawString(m_display, m_window, gc, x, y, text.c_str(), text.size());

	XFreeGC(m_display, gc);
}

XColor ScreenOverlay::createXColorFromRGB(short red, short green, short blue)
{
	XColor color;
	color.red = (red * 0xFFFF) / 0xFF;
	color.green = (green * 0xFFFF) / 0xFF;
	color.blue = (blue * 0xFFFF) / 0xFF;
	color.flags = DoRed | DoGreen | DoBlue;

	if (!XAllocColor(m_display, DefaultColormap(m_display, m_screen), &color))
	{
		std::cerr << "createXColorFromRGBA: Cannot create color!" << std::endl;
		exit(-1);
	}
	return color;
}

XColor ScreenOverlay::createXColorFromRGBA(short red, short green, short blue, short alpha)
{
	XColor color = createXColorFromRGB(red, green, blue);
	*(&color.pixel) = ((*(&color.pixel)) & 0x00FFFFFF) | (alpha << 24);
	return color;
}

void ScreenOverlay::allowInputPassthrough(Window w)
{
	XserverRegion region = XFixesCreateRegion(m_display, NULL, 0);
	XFixesSetWindowShapeRegion(m_display, w, ShapeInput, 0, 0, region);
	XFixesDestroyRegion(m_display, region);
}