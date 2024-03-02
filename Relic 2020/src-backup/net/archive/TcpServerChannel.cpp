#include "TcpServerChannel.h"
#include "IAddress.h"
#include "IpAddress.h"

bool TcpServerChannel::Open(const IAddress *address)
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

bool TcpServerChannel::Send(const char *data, int length)
{
    // $TODO
    return false;
}

int TcpServerChannel::Receive(char *data, int length)
{
    // $TODO
    return 0;
}

bool TcpServerChannel::SendReliable(const char *data, int length)
{
    // it's TCP, always guaranteed
    return Send(data, length);
}

void TcpServerChannel::Close()
{
    // $TODO
}