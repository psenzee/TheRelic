#ifndef _DXMOUSE_H
#define _DXMOUSE_H

#include "inputdevice.h"

class DXMouse : public InputDevice
{
public:

    DXMouse(HWND hwnd, IDirectInput8 *input);
    ~DXMouse();

protected:

	HWND                 m_hwnd;
	IDirectInput8       *m_input;
    IDirectInputDevice8 *m_device;
    DIMOUSESTATE         m_mouseState;
    POINT                m_position;

public:

	bool initialize();
	bool poll();

	POINT getMousePos();
	bool  isButtonDown(int button);

	// Handles the WM_SETCURSOR message
	bool handleSetCursor();
	// Toggles cursor display
	void showCursor(bool show);
	// Sets the position of the cursor
	void setCursorPosition(int x, int y);
	// Gets the position of the cursor
	void getCursorPosition(int &x, int &y);
	// Moves the position of the cursor.
	void moveCursor(int x, int y);
	// Update the cursor position
	// based on mouse movement
	void updateCursorPos();
};

#endif  // _DXMOUSE_H