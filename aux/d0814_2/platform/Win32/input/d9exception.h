#ifndef _D9EXCEPTION_H
#define _D9EXCEPTION_H

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

#define VERIFYD9(expression, message) { HRESULT hr = (expression); if (FAILED(hr)) _throw_d9_exception(message, hr, __FILE__, __LINE__, #expression); }

void _throw_d9_exception(const char *message, HRESULT result, const char *file, int line, const char *expression = "..");

#endif  // _D9EXCEPTION_H