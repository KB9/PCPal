#ifndef _SCREEN_OVERLAY_H_
#define _SCREEN_OVERLAY_H_

#include <string>

#include <X11/Xlib.h>

class ScreenOverlay
{
public:
	ScreenOverlay(Display* display, int screen, int width, int height);

	void drawBox(int x, int y, int width, int height, bool fill = false);
	void drawText(const std::string& text, int x, int y);

private:
	Display* m_display;
	int m_screen;
	Window m_window;

	XColor createXColorFromRGB(short red, short green, short blue);
	XColor createXColorFromRGBA(short red, short green, short blue, short alpha);

	void allowInputPassthrough(Window w);
};

#endif // _SCREEN_OVERLAY_H_