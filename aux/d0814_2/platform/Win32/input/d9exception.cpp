#include "dxconstants.h"
#include "d9exception.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

#include <string>
#include <exception>
#include <stdio.h>

#include "core/exception.h"

#ifndef HAS_DXERR9
static const char *DXGetErrorString9(HRESULT r)      { return "[CAN'T RETRIEVE ERROR STRING]"; }
static const char *DXGetErrorDescription9(HRESULT r) { return "[CAN'T RETRIEVE ERROR DESCRIPTION]"; }
#endif

void _throw_d9_exception(const char *message, HRESULT result, const char *file, int line, const char *expression)
{
    _throw_exception("DX9 exception : %s\r\nError: %s\r\nDescription: %s\r\n(in file '%s' at line %d; HRESULT = 0x%p)\r\nExpression: %s",
                      message, DXGetErrorString9(result), DXGetErrorDescription9(result), file, line, result, expression);
}