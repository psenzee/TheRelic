#ifndef _SOCKETS_H
#define _SOCKETS_H

#ifdef WIN32

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX 1

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>

#define IoctlSocket ioctlsocket
#define CloseSocket closesocket

#else
// .. unix defines

#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

typedef int SOCKET;

#define IoctlSocket ioctl
#define CloseSocket close

#endif

int         InitializeSockets();
void        ShutdownSockets();
const char *GetSocketErrorDescription();

#endif // _SOCKETS_H