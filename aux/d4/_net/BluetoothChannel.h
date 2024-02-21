#ifndef _BLUETOOTHCHANNEL_H
#define _BLUETOOTHCHANNEL_H

#include "IChannel.h"

class BluetoothChannel : public IChannel
{
public:

    BluetoothChannel() : mAddress(0) {}

    bool Open(const IAddress *address);
    bool Send(const char *data, int length);
    bool SendReliable(const char *data, int length);
    int  Receive(char *data, int length);
    void Close();

    const IAddress *GetRemoteAddress() { return mAddress; }

private:

    const IAddress *mAddress;
};

#endif // _BLUETOOTHCHANNEL_H