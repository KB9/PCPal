#include "ScreenOverlay.hpp"

#include <X11/extensions/Xcomposite.h>
#include <X11/extensions/shape.h>

#include <cairo-xlib.h>

ScreenOverlay::ScreenOverlay(Display *display)
{
	int screen_number = XDefaultScreen(display);
	Window root_window = XRootWindow(display, screen_number);

	XCompositeRedirectSubwindows(display, root_window, CompositeRedirectAutomatic);
	XSelectInput(display, root_window, SubstructureNotifyMask);

	overlay = getOverlayWindow(display, root_window);
	enableInputPassthrough(display, overlay);
	XSelectInput(display, overlay, ExposureMask);

	int width = DisplayWidth(display, screen_number);
	int height = DisplayHeight(display, screen_number);

	surface = cairo_xlib_surface_create(display, overlay,
	                                    DefaultVisual(display, screen_number),
	                                    width, height);
	context = cairo_create(surface);
}

ScreenOverlay::~ScreenOverlay()
{
	cairo_destroy(context);
	cairo_surface_destroy(surface);
}

void ScreenOverlay::drawBox(unsigned x, unsigned y, unsigned w, unsigned h)
{
	cairo_set_source_rgb(context, 1.0, 0.0, 0.0);
	cairo_rectangle(context, x, y, w, h);
	cairo_stroke(context);
}

Window ScreenOverlay::getOverlayWindow(Display *display, Window &root_window)
{
	return XCompositeGetOverlayWindow(display, root_window);
}

void ScreenOverlay::enableInputPassthrough(Display *display, Window &overlay)
{
	// TODO: Is this even required?
	XserverRegion region = XFixesCreateRegion(display, NULL, 0);
	XFixesSetWindowShapeRegion(display, overlay, ShapeBounding, 0, 0, 0);
	XFixesSetWindowShapeRegion(display, overlay, ShapeInput, 0, 0, region);
	XFixesDestroyRegion(display, region);
}