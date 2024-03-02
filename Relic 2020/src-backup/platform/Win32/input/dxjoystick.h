#ifndef _DXJOYSTICK_H
#define _DXJOYSTICK_H

#include "diexception.h"
#include "dxconstants.h"
#include "inputdevice.h"

template <class T>
class DIProperty
{
    IDirectInputDevice8 *m_device;
    T                    m_property;

public:

    DIProperty() : m_device(NULL) {}
    
    void setDevice(IDirectInputDevice8 *device)
    {
        m_device = device;
    }
    const T &get(const REFGUID id)
    {
        memset((void *)&m_property, 0, sizeof(T));
        m_property.diph.dwSize       = sizeof(T);
        m_property.diph.dwHeaderSize = sizeof(DIPROPHEADER);
        m_property.diph.dwObj        = 0;
        m_property.diph.dwHow        = DIPH_DEVICE;

        HRESULT r = m_device->GetProperty(id, &m_property.diph);

        if (FAILED(r)) _throw_di_exception(r);

        return m_property;
    }
};

class DXJoystick : public InputDevice
{
public:
    DXJoystick(HWND hwnd, IDirectInput8 *input);
    ~DXJoystick();

protected:

    DIJOYSTATE              m_data;
    HWND                    m_hwnd;
    IDirectInput8          *m_input;
    IDirectInputDevice8    *m_device;

    bool                    m_active;

    int                     m_logicalRange,
                            m_physicalRange,
                            m_deadZone,
                            m_saturation,
                            m_rangeMin,
                            m_rangeMax;

    DIProperty<DIPROPDWORD> m_propertyInt;
    DIProperty<DIPROPRANGE> m_propertyRange;

    // The cursor position
    int m_x,  m_y,  m_z,
        m_rx, m_ry, m_rz;

    static BOOL CALLBACK m_enumerator(const DIDEVICEINSTANCE *deviceInstance, void *context);

public:

    bool        initialize();
    bool        poll();
    bool        active() const;

    int         getDeadZone(void)            { return m_deadZone;/*m_propertyInt.get(DIPROP_DEADZONE).dwData;*/ }
    int         getLogicalRange(void)        { return m_logicalRange;/*m_propertyInt.get(DIPROP_LOGICALRANGE).dwData;*/ }
    int         getPhysicalRange(void)       { return m_physicalRange;/*m_propertyInt.get(DIPROP_PHYSICALRANGE).dwData;*/ }
    int         getSaturation(void)          { return m_saturation;/*m_propertyInt.get(DIPROP_SATURATION).dwData;*/ }
    int         getRangeMax(void)            { return m_rangeMax; }
    int         getRangeMin(void)            { return m_rangeMin; }

    inline int  getX(void)               { return m_x; }
    inline int  getY(void)               { return m_y; }
    inline int  getZ(void)               { return m_z; }

    inline int  getRX(void)              { return m_rx; }
    inline int  getRY(void)              { return m_ry; }
    inline int  getRZ(void)              { return m_rz; }

    inline bool isButtonDown(int button) { return (m_data.rgbButtons[button] & 0x80) != 0; }
};

#endif  // _DXJOYSTICK_H