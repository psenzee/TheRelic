#include "IpAddress.h"
#include "UdpChannel.h"
//#include "TcpServerChannel.h"
//#include "TcpClientChannel.h"

IChannel *IpAddress::Open(bool server)
{
    IChannel *channel = 0;
    switch (mProtocol)
    {
    case IAddress::PROTOCOL_UDP:       channel = new UdpChannel;      break;
//  case IAddress::PROTOCOL_TCP_UDP:   channel = new TcpUdpChannel;    break;
//  case IAddress::PROTOCOL_TCP:       channel = server ? (IChannel *)new TcpServerChannel : (IChannel *)new TcpClientChannel; break;
    default:
        // $TODO ERROR!
        return 0;
    }
    if (!channel->Open(this))
    {
        // $TODO ERROR!
        delete channel;
        return 0;
    }
    return channel;
}