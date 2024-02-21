#ifndef _BLUETOOTHCONNECTION_H
#define _BLUETOOTHCONNECTION_H

//#include "IAddress.h"

class BluetoothConnection
{
public:
    
    typedef void (*ReceiveCallback)(const void *data, int length, void *user);
    typedef void (*DisconnectCallback)(int type, void *user);

    BluetoothConnection(/*const IAddress *address*/);
    ~BluetoothConnection();
    
    bool Open();
    bool Send(const char *data, int length, bool reliable, bool sequenced = true);
    void Process();
    void Close();
    
    void SetReceive(ReceiveCallback callback, void *user) { mReceiveCallback = callback; mReceiveUser = user; }
    void SetDisconnect(DisconnectCallback callback, void *user) { mDisconnectCallback = callback; mDisconnectUser = user; }

private:

 //   const IAddress     *mAddress;
    // ..
    ReceiveCallback     mReceiveCallback;
    void               *mReceiveUser;
    DisconnectCallback  mDisconnectCallback;
    void               *mDisconnectUser;
};

#endif // _BLUETOOTHCONNECTION_H