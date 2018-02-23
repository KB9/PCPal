#include <iostream>

#include <X11/Xlib.h>

#include <opencv2/highgui.hpp>

#include "ScreenOverlay.hpp"
#include "KeyboardSimulator.hpp"

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
	struct timespec ts = {0, 5000000};

	Display *display = XOpenDisplay(NULL);

	ScreenOverlay screen_overlay(display);
	KeyboardSimulator keyboard;

	XEvent event;
	bool is_running = true;
	while (is_running)
	{
		auto pos = mousePos(display);
		screen_overlay.drawText("(" + std::to_string(pos.first) + "," + std::to_string(pos.second) + ")", pos.first+5, pos.second-5);
		nanosleep(&ts, NULL);
	}

	XCloseDisplay(display);
	return 0;
}