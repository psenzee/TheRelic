#ifndef _SOCKETS_H
#define _SOCKETS_H

#ifdef WIN32

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX 1

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>

int         InitializeSockets();
void        ShutdownSockets();
const char *GetSocketErrorDescription();

#else
// .. unix defines
#endif

#endif // _SOCKETS_H