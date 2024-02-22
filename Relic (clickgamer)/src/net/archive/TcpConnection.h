#ifndef _TCPCONNECTION_H
#define _TCPCONNECTION_H

#include "Sockets.h"
#include "Address.h"

class TcpConnection
{
public:

    TcpConnection(int localPort = 0);
    ~TcpConnection();
    
    bool Open(const Address &address);
    bool Send(const char *data, int length);
    int  Receive(char *data, int length);
    void Close();

    // NOTE: This gets the LOCAL address
    const Address &GetAddress() { return mLocal; }

private:

    SOCKET         mSocket;
    Address        mLocal;
    int            mLocalPort;
};

#endif // _TCPCONNECTION_H