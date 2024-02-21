#ifndef _UDPCHANNEL_H
#define _UDPCHANNEL_H

#include "IChannel.h"

class UdpChannel : public IChannel
{
public:

    UdpChannel() : mAddress(0) {}

    bool Open(const IAddress *address);
    bool Send(const char *data, int length);
    bool SendReliable(const char *data, int length);
    int  Receive(char *data, int length);
    void Close();

    const IAddress *GetRemoteAddress() { return mAddress; }

private:

    const IAddress *mAddress;
};

#endif // _UDPCHANNEL_H