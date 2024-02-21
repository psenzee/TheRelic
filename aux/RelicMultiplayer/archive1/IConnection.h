#ifndef _ICONNECTION_H
#define _ICONNECTION_H

namespace Connection
{

class IConnection;

typedef void (*OnReceiveCallback)(int connectionid, const void *data, int length, void *user);
typedef void (*OnDisconnectCallback)(int connectionid, int reason, void *user);
typedef void (*OnConnectCallback)(IConnection *connection, void *user);
typedef void (*OnAbortConnectCallback)(IConnection *connection, int reason, void *user);

namespace ConnectionStatus
{
enum ConnectionStatus
{
    NOT_STARTED = 0,
    IN_PROGRESS,
    CONNECTED,
    REFUSED,
    ABORTING_BY_USER,
    ABORTED_BY_USER,
    ABORTED_ERROR,
};
}

class IConnection
{
public:

    virtual ~IConnection() {}   
    
    virtual int         GetId() const = 0;
    virtual void        SetOpaqueState(void *opaque) = 0;
    virtual void       *GetOpaqueState() = 0;
    virtual const void *GetOpaqueState() const = 0;

    virtual bool        IsOpen() const = 0;
    virtual bool        Send(const char *data, int length, bool reliable, bool sequenced = true) = 0;
    virtual void        Process() = 0;
    virtual bool        Close() = 0;
    
    // for use by connection manager
    virtual void        SetOnReceive(OnReceiveCallback callback, void *user) = 0;
    virtual void        SetOnDisconnect(OnDisconnectCallback callback, void *user) = 0;
};


// bluetooth or enet
class IClientConnector
{
public:

    virtual ~IClientConnector() {}

    virtual bool                               Connect() = 0;
    virtual void                               Process() = 0;
    virtual void                               Abort() = 0;
    virtual ConnectionStatus::ConnectionStatus GetStatus() const = 0;

    virtual void                               SetOnConnect(OnConnectCallback callback, void *user) = 0;
    virtual void                               SetOnAbortConnect(OnAbortConnectCallback callback, void *user) = 0;
};


// bluetooth or enet
class IServerConnector
{
public:

    virtual ~IServerConnector() {}

    virtual void Process() = 0;

    virtual void SetOnConnect(OnConnectCallback callback, void *user) = 0;
    virtual void SetOnAbortConnect(OnAbortConnectCallback callback, void *user) = 0;
};

}

#endif // _ICONNECTION_H