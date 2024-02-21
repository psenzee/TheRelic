#ifndef _CONNECTIONMANAGER_H
#define _CONNECTIONMANAGER_H

#include "IConnection.h"
#include <vector>
#include <map>
#include <string>
#include <string.h>

namespace Connection
{

class ConnectionManager
{
    enum { DEFAULT_MAX_CONNECTIONS = 4, MESSAGE_NAME_LENGTH = 64, BUFFER_SIZE = 64000 };

    struct ConnectionInfo
    {
        IConnection *connection;
        char         name[MESSAGE_NAME_LENGTH];
        unsigned     tick;
        unsigned     sentTick;

        inline ConnectionInfo() : connection(0), tick(0), sentTick(0)
        {
            memset(name, 0, sizeof(name));
        }
    };

public:

    enum MessageType { MESSAGE_NAME = 1,  // 62 char name with additional terminating null
                       MESSAGE_TICK,
                       MESSAGE_PAYLOAD };

    ConnectionManager();

    void         SetClientConnector(IClientConnector *connector);
    void         SetServerConnector(IServerConnector *connector);

    bool         Start(const char *name);
    bool         IsServer() const;

    int          GetIndex(const IConnection *connection) const;
    int          GetIndexById(int connectionid) const;

    bool         Close(IConnection *connection);
    
    // for sequential enumeration
    size_t       GetConnectionCount() const;
    IConnection *GetConnectionAt(int n);
    const IConnection *GetConnectionAt(int n) const;

    IConnection *GetConnection(int connectionid) { return GetConnectionAt(GetIndexById(connectionid)); }
    const IConnection *GetConnection(int connectionid) const { return GetConnectionAt(GetIndexById(connectionid)); }
 
    bool         IsLosingConnection(int connectionid);

    void         Process();

    void         SetOnReceive(OnReceiveCallback callback, void *user);    
    void         SetOnConnect(OnConnectCallback callback, void *user);
    void         SetOnDisconnect(OnDisconnectCallback callback, void *user);

    // convenience functions
    bool         Send(int connectionid, const char *data, int length, bool reliable, bool sequenced = true);
    bool         SendAll(const char *data, int length, bool reliable, bool sequenced = true);
    bool         SendString(int connectionid, const char *data, bool reliable = true, bool sequenced = true)
    {
        return Send(connectionid, data, strlen(data) + 1, reliable, sequenced);
    }
    bool         SendAllString(const char *data, bool reliable = true, bool sequenced = true)
    {
        return SendAll(data, strlen(data) + 1, reliable, sequenced);
    }

    const char  *GetName(int connectionid) const;

    void         PrintConnections();

protected:

    ConnectionInfo &GetInfo(IConnection *connection);
    void         Tick();

    bool         SendRaw(int connectionid, const char *data, int length, bool reliable, bool sequenced = true);
    bool         SendAllRaw(const char *data, int length, bool reliable, bool sequenced = true);

    bool         AddConnection(IConnection *connection);
    bool         RemoveConnectionAt(int n);
    bool         CloseConnectionAt(int n);

    void         OnConnect(IConnection *connection);
    void         OnReceive(int connectionid, const void *data, int length);
    void         OnDisconnect(int connectionid, int reason);

    static bool  SendInternal(IConnection *connection, const char *data, int length, bool reliable, bool sequenced);

private:

    std::string                              mName;
    std::vector<IConnection *>               mConnections;
    std::map<const IConnection *, ConnectionInfo>  mConnectionInfo;
    OnReceiveCallback                        mReceiveCallback;
    void                                    *mReceiveUser;
    OnConnectCallback                        mConnectCallback;
    void                                    *mConnectUser;
    OnDisconnectCallback                     mDisconnectCallback;
    void                                    *mDisconnectUser;
    IClientConnector                        *mClientConnector;
    IServerConnector                        *mServerConnector;

    static void OnConnectHook(IConnection *connection, void *user);
    static void OnReceiveHook(int connectionid, const void *data, int length, void *user);
    static void OnDisconnectHook(int connectionid, int reason, void *user);
};

}

#endif // _CONNECTIONMANAGER_H