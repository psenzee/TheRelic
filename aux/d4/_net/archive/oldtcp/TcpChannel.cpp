#include "TcpChannel.h"
#include "IAddress.h"
#include "IpAddress.h"

bool TcpChannel::Open(const IAddress *address)
{
    if (address->GetProtocol() != IAddress::PROTOCOL_TCP)
    {
        // $TODO ERROR HERE
        return false;
    }
    mAddress = static_cast<const IpAddress *>(address);
    // ..

    return false;
}

bool TcpChannel::SendTo(const char *data, int length)
{
    // $TODO
    return false;
}

int TcpChannel::ReceiveFrom(char *data, int length)
{
    // $TODO
    return 0;
}

bool TcpChannel::SendToReliable(const char *data, int length)
{
    // it's TCP, always guaranteed
    return SendTo(data, length);
}

void TcpChannel::Close()
{
    // $TODO
}