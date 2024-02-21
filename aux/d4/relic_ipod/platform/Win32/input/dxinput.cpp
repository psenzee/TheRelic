#include "dxinput.h"
#include "dxerrormacros.h"

DXInput::DXInput()
{
	m_hwnd = NULL;
	::EnumThreadWindows(::GetCurrentThreadId(), m_enumerator, (LPARAM)((void *)this));

	FAIL_ERR(DirectInput8Create(::GetModuleHandle(NULL),
		DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_input, NULL));

	FAIL_ERR((m_devices[KEYBOARD] = new DXKeyboard(m_hwnd, m_input))->initialize());
	FAIL_ERR((m_devices[MOUSE]    = new DXMouse   (m_hwnd, m_input))->initialize());
	FAIL_ERR((m_devices[JOYSTICK] = new DXJoystick(m_hwnd, m_input))->initialize());
}

DXInput::~DXInput()
{
	// Get rid of the IDirectInput8 object
	if (m_input != NULL)
	{
		m_input->Release();
		m_input = NULL;
	}

    m_instance = NULL;
}

InputDevice *DXInput::device(int index)
{ 
    InputDevice *d = instance()->m_devices[index];
    return d && d->active() ? d : 0;
}

DXInput *DXInput::instance()
{ 
    return m_instance = (m_instance == NULL) ? new DXInput : m_instance;
}

void DXInput::poll()
{ 
    instance()->m_poll();
}

void DXInput::m_poll()
{
	InputDevice *id = NULL;
	for (int i = 0; i < DEVICE_COUNT; i++)
		if ((id = m_devices[i]) != NULL)
			id->poll();
}

BOOL CALLBACK DXInput::m_enumerator(HWND hwnd, LPARAM context)
{
	DXInput *di = (DXInput *)((void *)context);
    di->m_hwnd = hwnd;
	return false;
}

DXInput *DXInput::m_instance = NULL;