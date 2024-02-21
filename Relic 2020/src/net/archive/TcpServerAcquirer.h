#ifndef _TCPSERVERACQUIRER_H
#define _TCPSERVERACQUIRER_H

#include "AddressAcquirer.h"
#include "Address.h"
#include "Sockets.h"

class TcpServerAcquirer : public AbstractAddressAcquirer
{
public:
    TcpServerAcquirer();

    void Update();
    bool Listen(int port);
    void Close();

private:

    SOCKET  mSocket;
    int     mLocalPort;
    Address mLocal;
};

#endif // _TCPSERVERACQUIRER_H