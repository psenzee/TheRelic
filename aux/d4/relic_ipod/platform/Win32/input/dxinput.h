#ifndef _DXINPUT_H
#define _DXINPUT_H

#include "inputdevice.h"
#include "dxkeyboard.h"
#include "dxmouse.h"
#include "dxjoystick.h"

class DXInput
{
public:

    enum { KEYBOARD, MOUSE, JOYSTICK, DEVICE_COUNT = 3 };

    ~DXInput();

    static InputDevice *device(int index);
    static DXInput     *instance();
    static void         poll();

private:

    void m_poll();

	static BOOL CALLBACK m_enumerator(HWND hwnd, LPARAM context);

    DXInput();

    HWND            m_hwnd;
	IDirectInput8  *m_input;
    InputDevice    *m_devices[DEVICE_COUNT];

    static DXInput *m_instance;
};

#endif  // _DXINPUT_H