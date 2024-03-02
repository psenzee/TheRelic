#ifndef _UDPCONNECTION_H
#define _UDPCONNECTION_H

#include "Sockets.h"
#include "Address.h"

class UdpConnection
{
public:

    UdpConnection(int localPort = 0);
    ~UdpConnection();    
    
    bool Open();
    bool SendTo(const Address &address, const char *data, int length);
    int  ReceiveFrom(Address &address, char *data, int length);
    void Close();

    // NOTE: This gets the LOCAL address
    const Address &GetAddress() { return mLocal; }

private:

    SOCKET         mSocket;
    Address        mLocal;
    int            mLocalPort;
};

#endif // _UDPCONNECTION_H