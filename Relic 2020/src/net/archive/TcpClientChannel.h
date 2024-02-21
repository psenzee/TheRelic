#ifndef _TCPCLIENTCHANNEL_H
#define _TCPCLIENTCHANNEL_H

#include "IChannel.h"

class TcpClientChannel : public IChannel
{
public:

    TcpClientChannel() : mAddress(0) {}

    bool Open(const IAddress *address);
    bool Send(const char *data, int length);
    bool SendReliable(const char *data, int length);
    int  Receive(char *data, int length);
    void Close();

    const IAddress *GetRemoteAddress() { return mAddress; }

private:

    const IAddress *mAddress;
};

#endif // _TCPCLIENTCHANNEL_H