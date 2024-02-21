#ifndef _TCPSERVERCHANNEL_H
#define _TCPSERVERCHANNEL_H

#include "IChannel.h"

class TcpServerChannel : public IChannel
{
public:

    TcpServerChannel() : mAddress(0) {}

    bool Open(const IAddress *address);
    bool Send(const char *data, int length);
    bool SendReliable(const char *data, int length);
    int  Receive(char *data, int length);
    void Close();

    const IAddress *GetRemoteAddress() { return mAddress; }

private:

    const IAddress *mAddress;
};

#endif // _TCPSERVERCHANNEL_H