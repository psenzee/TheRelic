#include "dxkeyboard.h"

DXKeyboard::DXKeyboard(HWND hwnd, IDirectInput8 *input) : InputDevice(), m_hwnd(hwnd), m_device(NULL), m_input(input)
{
	memset(m_buffer, 0, sizeof(m_buffer));
}

DXKeyboard::~DXKeyboard()
{
	if (m_device != NULL)
	{
		m_device->Unacquire();
		m_device->Release();
		m_device = NULL;
	}
}

bool DXKeyboard::initialize()
{
	HRESULT r;

	if (m_input == NULL)
		return false;

	// Release the device if it has already been created
	if (m_device != NULL)
	{
		m_device->Unacquire();
		m_device->Release();
		m_device = NULL;
	}

	// Create the device for the keyboard
	r = m_input->CreateDevice(GUID_SysKeyboard, &m_device, NULL);
	if (FAILED(r))
	{
		//ERR0("Failed to create keyboard device");
		return false;
	}

	// Set the data format for the device
	r = m_device->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(r))
	{
		//ERR0("Unable to set the keyboard data format");
		return false;
	}

	// Set the cooperative level
	r = m_device->SetCooperativeLevel(m_hwnd,
		    DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(r))
	{
//		ERR0("Unable to set keyboard cooperative level");
		return false;
	}

	// Acquire the device
	r = m_device->Acquire();
	if (FAILED(r))
	{
//		ERR0("Unable to acquire the keyboard");
		return false;
	}

	// Set the initialization flag to true
//	setInitialized(true);

	return true;
}

bool DXKeyboard::poll(void)
{
	HRESULT r = 0;

	// Make sure the keyboard has been initialized
//	if(!isInitialized())
		///return false;

	// Get the state of the keyboard into the key buffer
	r = m_device->GetDeviceState(sizeof(m_buffer), &m_buffer); // CHECK &m_buffer here is makes me nervous..
	if (FAILED(r))
	{
		// If the device is not acquired...
		if (r == DIERR_INPUTLOST)
		{
			// ...then reacquire the device
			while (r == DIERR_INPUTLOST)
				r = m_device->Acquire();

			if (SUCCEEDED(r))
			{
				m_device->GetDeviceState(sizeof(m_buffer), &m_buffer); // CHECK &m_buffer here is makes me nervous..
				return true;
			}
			return false;
		}
	}
	return true;
}

bool DXKeyboard::isPolledKeyDown(int key)
{
    return (m_buffer[key] & 0x80) != 0;
}

bool DXKeyboard::isKeyDown(int key)
{
	HRESULT r = 0;

	// Make sure the keyboard has been initialized
//	if(!isInitialized())
//		return false;

	// Get the state of the keyboard into the key buffer
	r = m_device->GetDeviceState(sizeof(m_buffer), &m_buffer); // CHECK &m_buffer here is makes me nervous..
	if (FAILED(r))
	{
		// If the device is not acquired...
		if (r == DIERR_INPUTLOST)
		{
			// ...then reacquire the device
			while (r == DIERR_INPUTLOST)
				r = m_device->Acquire();

			if (SUCCEEDED(r))
				m_device->GetDeviceState(sizeof(m_buffer), &m_buffer); // CHECK &m_buffer here is makes me nervous..
			else
				return false;
		}
		else
			// ...Otherwise it was some other error
			return false;
	}

	// Check if the key was set
	return (m_buffer[key] & 0x80) != 0;
}
