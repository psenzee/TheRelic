#include "BluetoothAddress.h"
#include "BluetoothChannel.h"

IChannel *BluetoothAddress::Open(bool server)
{
    IChannel *channel = new BluetoothChannel;
    if (!channel->Open(this))
    {
        // $TODO ERROR!
        delete channel;
        return 0;
    }
    return channel;
}