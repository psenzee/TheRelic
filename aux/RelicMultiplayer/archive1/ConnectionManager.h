#ifndef _CONNECTIONMANAGER_H
#define _CONNECTIONMANAGER_H

#include "IConnection.h"
#include <vector>

namespace Connection
{

class ConnectionManager
{
    enum { DEFAULT_MAX_CONNECTIONS = 4 };

public:

    ConnectionManager();

    void         SetClientConnector(IClientConnector *connector);
    void         SetServerConnector(IServerConnector *connector);
    
    void         SetMaxConnections(size_t max);
    size_t       GetMaxConnections() const;

    int          GetIndex(const IConnection *connection) const;
    int          GetIndexById(int connectionid) const;
    int          GetIndexByOpaqueState(const void *opaque) const;

    void         Broadcast(const char *data, int length, bool reliable, bool sequenced = true);
    bool         AddConnection(IConnection *connection);    

    bool         Close(IConnection *connection);
    
    bool         IsCapacityFull() const;
    size_t       GetConnectionCount() const;
    IConnection *GetConnectionAt(int n) const;
    bool         RemoveConnectionAt(int n);
    bool         CloseConnectionAt(int n);

    IConnection *GetConnectionById(int connectionid) const;
   
    void         Process();

    void         SetOnReceiveCallback(OnReceiveCallback callback, void *user);    
    void         SetOnDisconnectCallback(OnDisconnectCallback callback, void *user);

private:

    std::vector<IConnection *>  mConnections;
    size_t                      mMaxConnections;
    OnReceiveCallback           mReceiveCallback;
    void                       *mReceiveUser;
    OnDisconnectCallback        mDisconnectCallback;
    void                       *mDisconnectUser;
    IClientConnector           *mClientConnector;
    IServerConnector           *mServerConnector;
       
    void RejectConnection_Full(IConnection *connection);
    void AcceptConnection(IConnection *connection);
};

}

#endif // _CONNECTIONMANAGER_H