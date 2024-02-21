#include "dxerrors.h"

const char *DXErrors::getUnknownMessage(HRESULT hr)
{
    const char *c = getMessage(hr);
    return c != NULL ? c :
            "Unknown application error. Enable\n"
            "debug output for detailed information.";
}

const char *DXErrors::getMessage(HRESULT hr)
{
    switch (hr)
    {
    case D3DAPPERR_NODIRECT3D:
        return "Could not initialize Direct3D. You may\n"
                "want to check that the latest version of\n"
                "DirectX is correctly installed on your\n"
                "system.  Also make sure that this program\n"
                "was compiled with header files that match\n"
                "the installed DirectX DLLs.";

    case D3DAPPERR_NOCOMPATIBLEDEVICES:
        return "Could not find any compatible Direct3D\n"
                "devices.";

    case D3DAPPERR_NOWINDOWABLEDEVICES:
        return "This application cannot run in a desktop\n"
                "window with the current display settings.\n"
                "Please change your desktop settings to a\n"
                "16- or 32-bit display mode and re-run this\n"
                "application.";

    case D3DAPPERR_NOHARDWAREDEVICE:
        return "No hardware-accelerated Direct3D devices\n"
                "were found.";

    case D3DAPPERR_HALNOTCOMPATIBLE:
        return "This application requires functionality that is\n"
                "not available on your Direct3D hardware\n"
                "accelerator.";

    case D3DAPPERR_NOWINDOWEDHAL:
        return "Your Direct3D hardware accelerator cannot\n"
                "render into a window.\n";


    case D3DAPPERR_NODESKTOPHAL:
        return "Your Direct3D hardware accelerator cannot\n"
                "render into a window with the current\n"
                "desktop display settings.\n";

    case D3DAPPERR_NOHALTHISMODE:
        return "This sample requires functionality that is\n"
                "not available on your Direct3D hardware\n"
                "accelerator with the current desktop display\n"
                "settings.\n";

    case D3DAPPERR_MEDIANOTFOUND:
        return "Could not load required media.";

    case D3DAPPERR_RESIZEFAILED:
        return "Could not reset the Direct3D device.";

    case D3DAPPERR_NONZEROREFCOUNT:
        return "A D3D object has a non-zero reference\n"
                "count (meaning things were not properly\n"
                "cleaned up).";

    case D3DAPPERR_NULLREFDEVICE:
        return "Warning: Nothing will be rendered.\n"
                "The reference rendering device was selected, but your\n"
                "computer only has a reduced-functionality reference device\n"
                "installed.  Install the DirectX SDK to get the full\n"
                "reference device.\n";

    case E_OUTOFMEMORY:
        return "Not enough memory.";

    case D3DERR_OUTOFVIDEOMEMORY:
        return "Not enough video memory.";
        break;
    }
    return NULL;
}

#ifndef HAS_DXERR9
static const char *DXGetErrorString9(HRESULT r)      { return "[CAN'T RETRIEVE ERROR STRING]"; }
static const char *DXGetErrorDescription9(HRESULT r) { return "[CAN'T RETRIEVE ERROR DESCRIPTION]"; }
#endif

HRESULT DXErrors::failed(HRESULT r, const char *rstr, const char *file, int line)
{
    if (FAILED(r))
    {
        std::string s("DXError: '");
        s += std::string(DXGetErrorString9(r)) + "'\r\nOFFENDING LINE: '" +
            std::string(rstr) + "'\r\n\r\n" + std::string(getUnknownMessage(r));
        ::OutputDebugStr(s.c_str());
        throw DXException(file, line, r, s.c_str(), rstr);
    }
    return r;
}