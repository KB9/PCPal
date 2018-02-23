#include "KeyboardSimulator.hpp"

KeyboardSimulator::KeyboardSimulator()
{

}

void KeyboardSimulator::typeText(Display *display, const std::string &text)
{
	for (char c : text)
		sendKeyStroke(display, c);
}

XKeyEvent KeyboardSimulator::createKeyEvent(Display *display, Window &win,
                                            Window &winRoot, bool press,
                                            int keycode, int modifiers)
{
	XKeyEvent event;

	event.display     = display;
	event.window      = win;
	event.root        = winRoot;
	event.subwindow   = None;
	event.time        = CurrentTime;
	event.x           = 1;
	event.y           = 1;
	event.x_root      = 1;
	event.y_root      = 1;
	event.same_screen = True;
	event.keycode     = XKeysymToKeycode(display, keycode);
	event.state       = modifiers;

	if(press)
		event.type = KeyPress;
	else
		event.type = KeyRelease;

	return event;
}

void KeyboardSimulator::sendKeyStroke(Display *display, char key)
{
	int screen_number = XDefaultScreen(display);
	Window root_window = XRootWindow(display, screen_number);

	Window focused_window;
	bool ok = getFocusedWindow(display, focused_window);
	if (ok)
	{
		int keycode = XStringToKeysym(&key);

		// Send a key press event to the focused window
		XKeyEvent event = createKeyEvent(display, focused_window, root_window, true, keycode, 0);
		XSendEvent(event.display, event.window, True, KeyPressMask, (XEvent *)&event);

		// Send a key release event to the focused window
		event = createKeyEvent(display, focused_window, root_window, false, keycode, 0);
		XSendEvent(event.display, event.window, True, KeyPressMask, (XEvent *)&event);
	}
}

bool KeyboardSimulator::getFocusedWindow(Display *display, Window &focused_window)
{
	int revert_to;
	XGetInputFocus(display, &focused_window, &revert_to);
	return focused_window != None;
}