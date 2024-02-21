#ifndef _ENETCONNECTION_H
#define _ENETCONNECTION_H

#include "enet/enet.h"
#include "IConnection.h"

namespace Connection
{

class ENetServerConnector : public IServerConnector
{


};

class ENetConnection : public IConnection
{
public:
    
    ENetConnection();
    ~ENetConnection();

    bool        OpenServer(int port);
    bool        OpenClient();
    bool        Connect(const char *address, int port);
    void        SetId(int id) { mId = id; }

    bool        IsOpen() const { return false; /* tmp */}
    int         GetId() const { return mId; }
    void        SetOpaqueState(void *opaque) { mOpaque = opaque; }
    void       *GetOpaqueState() { return mOpaque; }
    const void *GetOpaqueState() const { return mOpaque; }

    bool        Send(const char *data, int length, bool reliable, bool sequenced = true);
    void        Process();
    bool        Close();
    
    void        SetOnReceive(OnReceiveCallback callback, void *user) { mReceiveCallback = callback; mReceiveUser = user; }
    void        SetOnDisconnect(OnDisconnectCallback callback, void *user) { mDisconnectCallback = callback; mDisconnectUser = user; }

private:

    ENetAddress          *mENetAddress;
    ENetHost             *mLocal;
    ENetPeer             *mPeer;
    OnReceiveCallback     mReceiveCallback;
    void                 *mReceiveUser;
    OnDisconnectCallback  mDisconnectCallback;
    void                 *mDisconnectUser;
    int                   mId;
    void                 *mOpaque;
};

}

#endif // _ENETCONNECTION_H