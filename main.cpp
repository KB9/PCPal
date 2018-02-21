#include <iostream>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <X11/extensions/Xcomposite.h>
#include <X11/extensions/Xfixes.h>
#include <X11/extensions/shape.h>

#include <cairo.h>
#include <cairo-xlib.h>

#include <opencv2/highgui.hpp>

class ScreenOverlay
{
public:
	ScreenOverlay(Display *display)
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

	~ScreenOverlay()
	{
		cairo_destroy(context);
		cairo_surface_destroy(surface);
	}

	void drawBox(unsigned x, unsigned y, unsigned w, unsigned h)
	{
		cairo_set_source_rgb(context, 1.0, 0.0, 0.0);
		cairo_rectangle(context, x, y, w, h);
		cairo_stroke(context);
	}

private:
	Window overlay;
	cairo_surface_t *surface = nullptr;
	cairo_t *context = nullptr;

	Window getOverlayWindow(Display *display, Window &root_window)
	{
		return XCompositeGetOverlayWindow(display, root_window);
	}

	void enableInputPassthrough(Display *display, Window &overlay)
	{
		// TODO: Is this even required?
		XserverRegion region = XFixesCreateRegion(display, NULL, 0);
	    XFixesSetWindowShapeRegion(display, overlay, ShapeBounding, 0, 0, 0);
	    XFixesSetWindowShapeRegion(display, overlay, ShapeInput, 0, 0, region);
	    XFixesDestroyRegion(display, region);
	}
};

cv::Mat xImageToCvMat(const XImage *const ximage)
{
	return cv::Mat(ximage->height, ximage->width, CV_8UC4, ximage->data);
}

cv::Mat grabScreen(Display *display)
{
	int screen_number = XDefaultScreen(display);
	Window window = XRootWindow(display, screen_number);

	Screen *screen = XDefaultScreenOfDisplay(display);
	int screen_width = XWidthOfScreen(screen);
	int screen_height = XHeightOfScreen(screen);

	XImage *ximage = XGetImage(display, window, 0, 0, screen_width, screen_height, AllPlanes, ZPixmap);

	cv::Mat mat = xImageToCvMat(ximage);
	XFree(ximage);

	return mat;
}

std::pair<int, int> mousePos(Display *display)
{
	int screen_number = XDefaultScreen(display);
	Window window = XRootWindow(display, screen_number);

	Window root_return, child_return;
	int root_x, root_y;
	int win_x, win_y;
	unsigned mask_return;
	Bool result = XQueryPointer(display, window, &root_return, &child_return,
	                            &root_x, &root_y, &win_x, &win_y, &mask_return);

	if (result == True)
		return {root_x, root_y};
	else
		return {-1, -1};
}

int main(int argc, char *argv[])
{
	Display *display = XOpenDisplay(NULL);

	ScreenOverlay screen_overlay(display);

	cv::imshow("PCPal - Screen Capture", grabScreen(display));
	char key;
	while (key != 'q' && key != 'Q')
	{
		auto pos = mousePos(display);

		screen_overlay.drawBox(pos.first-10, pos.second-10, 20, 20);
		key = cv::waitKey(30);
	}

	XCloseDisplay(display);
	return 0;
}