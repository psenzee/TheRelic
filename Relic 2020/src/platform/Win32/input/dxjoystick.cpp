#include "dxjoystick.h"
#include "dxerrormacros.h"

DXJoystick::DXJoystick(HWND hwnd, IDirectInput8 *input) : 
    InputDevice(), m_hwnd(hwnd), m_device(NULL), m_input(input), m_active(false)
{
}

DXJoystick::~DXJoystick()
{
    if (m_device != NULL)
    {
        m_device->Unacquire();
        m_device->Release();
        m_device = NULL;
    }
}

bool DXJoystick::poll()
{
    HRESULT r = 0;

    // Return if the object has not been initialized
    // Return if the DirectInput object does not exist
    if (m_input == NULL || m_device == NULL)
        initialize();
    if (m_input == NULL || m_device == NULL)
        return false;

    // Get the state of the Joystick
    r = m_device->Poll();
    while (r == DIERR_INPUTLOST || r == DIERR_NOTACQUIRED)
        r = m_device->Acquire();
    if (r == DIERR_NOTACQUIRED)
        return false;
    r = m_device->GetDeviceState(sizeof(DIJOYSTATE), &m_data);
    if (FAILED(r))
    {
        // If the Joystick has moved focus
        if (r == DIERR_INPUTLOST)
        {
            // Reacquire the Joystick
            while (r == DIERR_INPUTLOST)
                r = m_device->Acquire();

            // Try to test the state again
            if (SUCCEEDED(r))
                m_device->GetDeviceState(sizeof(DIJOYSTATE), &m_data);
            else
                return false;
        }
        else
        {
            //_throw_di_exception(r);
            return false;
        }
    }

    m_active = true;

    m_x  = m_data.lX;
    m_y  = m_data.lY;
    m_z  = m_data.lZ;
    m_rx = m_data.lRx;
    m_ry = m_data.lRy;
    m_rz = m_data.lRz;

    return true;
}

BOOL CALLBACK DXJoystick::m_enumerator(const DIDEVICEINSTANCE *deviceInstance, void *context)
{
    HRESULT hr;

    DXJoystick *joystick = (DXJoystick *)context;

    // Obtain an interface to the enumerated joystick.
    hr = joystick->m_input->CreateDevice(deviceInstance->guidInstance,
                                         &(joystick->m_device), NULL);
    if (FAILED(hr))
        return DIENUM_CONTINUE;

    return DIENUM_STOP;
}

bool DXJoystick::active() const
{
    return m_active;
}

// Initializes the joystick
bool DXJoystick::initialize()
{
    HRESULT r = 0;

    // Return if the DirectInput object does not exist
    if (m_input == NULL)
        return m_active = false;

    // Release the Joystick device if it has already been created
    if (m_device != NULL)
    {
        m_device->Unacquire();
        m_device->Release();
        m_device = NULL;
    }

    // Create the Joystick device
    FAIL_ERR(m_input->EnumDevices(DI8DEVCLASS_GAMECTRL, DXJoystick::m_enumerator,
                       this, DIEDFL_ATTACHEDONLY));

    if (m_device == NULL)
        return m_active = false;

    m_propertyInt.setDevice(m_device);
    m_propertyRange.setDevice(m_device);

    try { m_deadZone        = m_propertyInt.get(DIPROP_DEADZONE).dwData;      } catch (...) {}
    try { m_logicalRange    = m_propertyInt.get(DIPROP_LOGICALRANGE).dwData;  } catch (...) {}
    try { m_physicalRange   = m_propertyInt.get(DIPROP_PHYSICALRANGE).dwData; } catch (...) {}
    try { m_saturation      = m_propertyInt.get(DIPROP_SATURATION).dwData;    } catch (...) {}

    try {
    DIPROPRANGE range = m_propertyRange.get(DIPROP_RANGE);
    m_rangeMin        = range.lMin;
    m_rangeMax        = range.lMax;
    }
    catch (...) {}

    // Set the data format for the Joystick
    FAIL_ERR(r = m_device->SetDataFormat(&c_dfDIJoystick));

    // Set the cooperative level for the Joystick
    if (m_hwnd != NULL)
        FAIL_ERR(r = m_device->SetCooperativeLevel(m_hwnd,
            DISCL_NONEXCLUSIVE /*DISCL_EXCLUSIVE */| DISCL_FOREGROUND)); // TODO parameterize EXCLUSIVE/NONEXCLUSIVE mode.

    // Acquire the physical Joystick into the device
    if (FAILED(r = m_device->Acquire()))
        return m_active = false;

    return m_active = true;
}