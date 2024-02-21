#include "BluetoothChannel.h"
#include "IAddress.h"
#include "IpAddress.h"

bool BluetoothChannel::Open(const IAddress *address)
{
    if (address->GetProtocol() != IAddress::PROTOCOL_BLUETOOTH)
    {
        // $TODO ERROR HERE
        return false;
    }
    mAddress = static_cast<const IpAddress *>(address);
    // ..


    return false;
}

bool BluetoothChannel::Send(const char *data, int length)
{
    // $TODO
    return false;
}

int BluetoothChannel::Receive(char *data, int length)
{
    // $TODO
    return 0;
}

bool BluetoothChannel::SendReliable(const char *data, int length)
{
    // it's Bluetooth, assume guaranteed
    return Send(data, length);
}

void BluetoothChannel::Close()
{
    // $TODO
}