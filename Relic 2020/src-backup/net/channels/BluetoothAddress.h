#ifndef _BLUETOOTHADDRESS_H
#define _BLUETOOTHADDRESS_H

#include "IAddress.h"

class BluetoothAddress : public IAddress
{
public:

    BluetoothAddress() {}

    Protocol  GetProtocol() const { return PROTOCOL_BLUETOOTH; }
    Domain    GetDomain()   const { return DOMAIN_LOCAL; }

    IChannel *Open(bool server);

private:

    // ..
};

class BluetoothAddressListener : public IAddressListener
{
public:

    IAddress::Protocol GetProtocol() const { return IAddress::PROTOCOL_BLUETOOTH; }
    bool               Receive(IAddress *address);

    // ..
};

#endif // _BLUETOOTHADDRESS_H