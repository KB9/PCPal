#ifndef _KEYBOARD_SIMULATOR_H_
#define _KEYBOARD_SIMULATOR_H_

#include <string>

#include <X11/Xlib.h>
#include <X11/keysym.h>

class KeyboardSimulator
{
public:
	KeyboardSimulator();

	void typeText(Display *display, const std::string &text);

private:
	XKeyEvent createKeyEvent(Display *display, Window &window, Window &root_window,
	                         bool press, int keycode, int modifiers);
	void sendKeyStroke(Display *display, char key);

	bool getFocusedWindow(Display *display, Window &focused_window);
};

#endif // _KEYBOARD_SIMULATOR_H_