#ifndef _SCREEN_OVERLAY_H_
#define _SCREEN_OVERLAY_H_

#include <string>

#include <X11/Xlib.h>

#include <cairo.h>

class ScreenOverlay
{
public:
	ScreenOverlay(Display *display);
	~ScreenOverlay();

	void drawBox(unsigned x, unsigned y, unsigned w, unsigned h);
	void drawText(const std::string &text, unsigned x, unsigned y);

private:
	Window overlay;
	cairo_surface_t *surface = nullptr;
	cairo_t *context = nullptr;

	Window getOverlayWindow(Display *display, Window &root_window);
	void enableInputPassthrough(Display *display, Window &overlay);
};

#endif // _SCREEN_OVERLAY_H_