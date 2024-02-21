#ifndef _IPADDRESS_H
#define _IPADDRESS_H

#include "IAddress.h"
#include "Sockets.h"
#include "Address.h"

class IpAddress : public IAddress
{
public:

    IpAddress(IAddress::Domain domain, IAddress::Protocol protocol, 
              const Address &address) : mDomain(domain), mProtocol(protocol) {}

    Protocol  GetProtocol() const { return mProtocol; }
    Domain    GetDomain()   const { return mDomain; }

    IChannel *Open(bool server);

    const Address &GetAddress() const { return mAddress; }

private:

    IAddress::Domain   mDomain;
    IAddress::Protocol mProtocol;
    Address            mAddress;
};

class TcpAddressListener : public IAddressListener
{
public:

    IAddress::Protocol GetProtocol() const { return IAddress::PROTOCOL_TCP; }
    bool               Receive(IAddress *address);

    // ..
};

class UdpAddressListener : public IAddressListener
{
public:

    IAddress::Protocol GetProtocol() const { return IAddress::PROTOCOL_UDP; }
    bool               Receive(IAddress *address);

    // ..
};

class TcpUdpAddressListener : public IAddressListener
{
public:

    IAddress::Protocol GetProtocol() const { return IAddress::PROTOCOL_TCP_UDP; }
    bool               Receive(IAddress *address);

    // ..
};

#endif // _IPADDRESS_H