#ifndef _DXERRORS_H
#define _DXERRORS_H

#include "dxconstants.h"
#include "dxexception.h"
//#include "error.h"

#include <d3d8.h>

#define D3DAPPERR_NODIRECT3D          0x82000001
#define D3DAPPERR_NOWINDOW            0x82000002
#define D3DAPPERR_NOCOMPATIBLEDEVICES 0x82000003
#define D3DAPPERR_NOWINDOWABLEDEVICES 0x82000004
#define D3DAPPERR_NOHARDWAREDEVICE    0x82000005
#define D3DAPPERR_HALNOTCOMPATIBLE    0x82000006
#define D3DAPPERR_NOWINDOWEDHAL       0x82000007
#define D3DAPPERR_NODESKTOPHAL        0x82000008
#define D3DAPPERR_NOHALTHISMODE       0x82000009
#define D3DAPPERR_NONZEROREFCOUNT     0x8200000a
#define D3DAPPERR_MEDIANOTFOUND       0x8200000b
#define D3DAPPERR_RESIZEFAILED        0x8200000c
#define D3DAPPERR_NULLREFDEVICE       0x8200000d

class DXErrors
{
public:

    static bool THROW_EXCEPTION;

    static const char *getUnknownMessage(HRESULT hr);
    static const char *getMessage(HRESULT hr);
    static HRESULT failed(HRESULT r, const char *rstr, const char *file, int line);
};

#endif  // _DXERRORS_H