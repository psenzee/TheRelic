#ifndef _UDPCHANNEL_H
#define _UDPCHANNEL_H

#include "Sockets.h"
#include "Address.h"

class UdpChannel
{
public:

    UdpChannel(int localPort = 0);
    ~UdpChannel();    
    
    bool Open();
    bool SendTo(const Address &address, const char *data, int length);
    int  ReceiveFrom(Address &address, char *data, int length);
    void Close();

    const Address &GetAddress() { return mLocal; }

private:

    SOCKET         mSocket;
    Address        mLocal;
    int            mLocalPort;
};

#endif // _UDPCHANNEL_H