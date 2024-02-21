#ifndef _TCPUDPCHANNEL_H
#define _TCPUDPCHANNEL_H

#include "IChannel.h"

class TcpUdpChannel : public IChannel
{
public:

    TcpUdpChannel() : mAddress(0) {}

    bool Open(const IAddress *address);
    bool SendTo(const char *data, int length);
    bool SendToReliable(const char *data, int length);
    int  ReceiveFrom(char *data, int length);
    void Close();

    const IAddress *GetRemoteAddress() { return mAddress; }

private:

    const IAddress *mAddress;
};

#endif // _TCPUDPCHANNEL_H