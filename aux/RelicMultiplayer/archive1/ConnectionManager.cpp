#include "ConnectionManager.h"
#include "IConnection.h"
#include <vector>
#include <algorithm>

#include "Sockets.h"

#include <stdio.h> // for printf

namespace Connection
{
    
void ConnectionManager::RejectConnection_Full(IConnection *connection)
{
    if (connection && connection->IsOpen())
    {
        //connection->Send(/*rejection message*/);
        connection->Close();
    }
}

void ConnectionManager::AcceptConnection(IConnection *connection)
{
    if (connection && connection->IsOpen())
    {
        //connection->Send(/*accept message*/);
    }
}    

int ConnectionManager::GetIndex(const IConnection *connection) const
{
    for (size_t i = 0, sz = mConnections.size(); i < sz; i++)
    {
        IConnection *c = mConnections[i];
        if (c == connection)
            return static_cast<int>(i);
    }
    return -1;
}

int ConnectionManager::GetIndexById(int connectionid) const
{
    for (size_t i = 0, sz = mConnections.size(); i < sz; i++)
    {
        IConnection *connection = mConnections[i];
        if (connection && connection->GetId() == connectionid)
            return static_cast<int>(i);
    }
    return -1;
}

int ConnectionManager::GetIndexByOpaqueState(const void *opaque) const
{
    for (size_t i = 0, sz = mConnections.size(); i < sz; i++)
    {
        const IConnection *connection = mConnections[i];
        if (connection && connection->GetOpaqueState() == opaque)
            return static_cast<int>(i);
    }
    return -1;
}

ConnectionManager::ConnectionManager() : mMaxConnections(DEFAULT_MAX_CONNECTIONS)
{
}

void ConnectionManager::SetMaxConnections(size_t max)
{ 
    mMaxConnections = max;
}

size_t ConnectionManager::GetMaxConnections() const
{ 
    return mMaxConnections;
}

void ConnectionManager::Broadcast(const char *data, int length, bool reliable, bool sequenced)
{
    for (std::vector<IConnection *>::iterator i = mConnections.begin(), e = mConnections.end(); i != e; ++i)
        if (*i && (*i)->IsOpen()) (*i)->Send(data, length, reliable, sequenced);
}

bool ConnectionManager::AddConnection(IConnection *connection)
{ 
    if (IsCapacityFull())
    {
        RejectConnection_Full(connection);
        return false;        
    }
    mConnections.push_back(connection);
    AcceptConnection(connection);        
    return true;
}

bool ConnectionManager::CloseConnectionAt(int n)
{
    IConnection *connection = GetConnectionAt(n);
    if (!connection)
        return false;
    bool result = !connection->IsOpen() ? false : connection->Close();
    RemoveConnectionAt(n);
    return result;
}

bool ConnectionManager::IsCapacityFull() const
{ 
    return GetConnectionCount() >= GetMaxConnections();
}

size_t ConnectionManager::GetConnectionCount() const
{ 
    return mConnections.size();
}

IConnection *ConnectionManager::GetConnectionAt(int n) const
{
    return mConnections[n];
}

bool ConnectionManager::RemoveConnectionAt(int n)
{
    if (n < 0 || n >= static_cast<int>(mConnections.size()))
        return false;        
    IConnection *connection = mConnections[n];
    if (!connection)
        return false;
    mConnections.erase(std::remove(mConnections.begin(), mConnections.end(), connection), mConnections.end());
    return true;
}

IConnection *ConnectionManager::GetConnectionById(int connectionid) const
{
    int index = GetIndexById(connectionid);
    return (index < 0) ? 0 : mConnections[index];
}

void ConnectionManager::Process()
{
    for (std::vector<IConnection *>::iterator i = mConnections.begin(), e = mConnections.end(); i != e; ++i)
        if (*i) (*i)->Process();
}

void ConnectionManager::SetOnReceiveCallback(OnReceiveCallback callback, void *user)
{ 
    mReceiveCallback = callback; mReceiveUser = user;
    for (std::vector<IConnection *>::iterator i = mConnections.begin(), e = mConnections.end(); i != e; ++i)
        if (*i) (*i)->SetOnReceive(callback, user);
}

void ConnectionManager::SetOnDisconnectCallback(OnDisconnectCallback callback, void *user)
{ 
    mDisconnectCallback = callback; mDisconnectUser = user;
    for (std::vector<IConnection *>::iterator i = mConnections.begin(), e = mConnections.end(); i != e; ++i)
        if (*i) (*i)->SetOnDisconnect(callback, user);
}

void ConnectionManager::SetClientConnector(IClientConnector *connector)
{
    mClientConnector = connector;
}

void ConnectionManager::SetServerConnector(IServerConnector *connector)
{
    mServerConnector = connector;
}

}