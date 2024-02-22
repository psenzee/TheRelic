#ifndef _ENETCONNECTION_H
#define _ENETCONNECTION_H

#include "enet/enet.h"
#include "IConnection.h"
#include <map>
#include <string>

namespace Connection
{

class ENetPeerConnection;
class ENetClientConnection;

class ENetServerConnector : public IServerConnector
{
public:

    ENetServerConnector() : mENetAddress(0), mLocal(0), mConnectCallback(0), mConnectCallbackUser(0) {}
    ~ENetServerConnector();

    bool      Open(int port);
    ENetHost *GetHost() { return mLocal; }

    // IServerConnector implementation
    void      Start();    
    void      Process();

    // only to be set by ConnectionManager!
    void      SetOnConnect(OnConnectCallback callback, void *user);

private:

    ENetAddress                                *mENetAddress;
    ENetHost                                   *mLocal;
    OnConnectCallback                           mConnectCallback;
    void                                       *mConnectCallbackUser;
    std::map<ENetPeer *, ENetPeerConnection *>  mConnections;

    ENetPeerConnection *CreatePeerConnection(ENetPeer *peer);
    ENetPeerConnection *GetPeerConnection(ENetPeer *peer);
};

class ENetClientConnector : public IClientConnector
{
public:

    void SetAddress(const char *address, int port);

    // IClientConnector implementation
    bool Connect();
    void Process();

    // only to be set by ConnectionManager!
    void SetOnConnect(OnConnectCallback callback, void *user);
    void SetOnAbortConnect(OnAbortConnectCallback callback, void *user);

    // only to be called by ENetClientConnection
    void ConnectInternal(ENetClientConnection *connection);

private:

    std::string             mAddress;
    int                     mPort;
    OnConnectCallback       mConnectCallback;
    void                   *mConnectCallbackUser;
    OnAbortConnectCallback  mAbortConnectCallback;
    void                   *mAbortConnectCallbackUser;
};

class ENetPeerConnection : public IConnection
{
public:
    
    ENetPeerConnection(ENetServerConnector *serverConnector, ENetPeer *peer);
    ~ENetPeerConnection();

    void        ReceiveInternal(const void *data, int length);
    void        DisconnectInternal();

    // IConnection implementation
    void        SetId(int id) { mId = id; }
    int         GetId() const { return mId; }
    bool        Send(const char *data, int length, bool reliable, bool sequenced = true);
    void        Process();
    bool        Close();

    // only to be set by ConnectionManager!
    void        SetOnReceive(OnReceiveCallback callback, void *user);
    void        SetOnDisconnect(OnDisconnectCallback callback, void *user);

private:

    ENetServerConnector  *mServerConnector;
    ENetPeer             *mPeer;
    OnReceiveCallback     mReceiveCallback;
    void                 *mReceiveUser;
    OnDisconnectCallback  mDisconnectCallback;
    void                 *mDisconnectUser;
    int                   mId;
};

class ENetClientConnection : public IConnection
{
public:
    
    ENetClientConnection(ENetClientConnector *connector);
    ~ENetClientConnection();

    bool        Connect(const char *address, int port);

    // IConnection implementation
    void        SetId(int id) { mId = id; }
    int         GetId() const { return mId; }
    bool        Send(const char *data, int length, bool reliable, bool sequenced = true);
    void        Process();
    bool        Close();   

    // only to be set by ConnectionManager!
    void        SetOnReceive(OnReceiveCallback callback, void *user);
    void        SetOnDisconnect(OnDisconnectCallback callback, void *user);

private:

    ENetClientConnector  *mClientConnector;
    ENetAddress          *mENetAddress;
    ENetHost             *mLocal;
    ENetPeer             *mPeer;
    OnReceiveCallback     mReceiveCallback;
    void                 *mReceiveUser;
    OnDisconnectCallback  mDisconnectCallback;
    void                 *mDisconnectUser;
    int                   mId;
};

}

#endif // _ENETCONNECTION_H