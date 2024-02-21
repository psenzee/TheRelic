#include "ConnectionManager.h"
#include "IConnection.h"
#include <vector>
#include <algorithm>

#include "Sockets.h"
#include "time/Timer.h"

#include <stdio.h> // for printf

namespace Connection
{

int ConnectionIdAssigner::sId = 0;

bool ConnectionManager::Start(const char *name)
{
    mName = name;
    if (mClientConnector)
    {
        return mClientConnector->Connect();
    }
    else if (mServerConnector)
    {
        mServerConnector->Start();		
        return true;
    }
    return false;
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

ConnectionManager::ConnectionManager() : mReceiveCallback(0), mReceiveUser(0), mDisconnectCallback(0), mDisconnectUser(0), mClientConnector(0), mServerConnector(0)
{
}

bool ConnectionManager::AddConnection(IConnection *connection)
{ 
    mConnections.push_back(connection);
    return true;
}

bool ConnectionManager::CloseConnectionAt(int n)
{
    IConnection *connection = GetConnectionAt(n);
    if (!connection)
        return false;
    RemoveConnectionAt(n);    
//  delete connection; // commenting this out for now..
    return true;
}

size_t ConnectionManager::GetConnectionCount() const
{ 
    return mConnections.size();
}

IConnection *ConnectionManager::GetConnectionAt(int n)
{
    return mConnections[n];
}

const IConnection *ConnectionManager::GetConnectionAt(int n) const 
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
    std::map<const IConnection *, ConnectionInfo>::iterator i = mConnectionInfo.find(connection);
    if (i != mConnectionInfo.end())
        mConnectionInfo.erase(i);
    return true;
}

bool ConnectionManager::IsLosingConnection(int connectionid)
{
    int ms = GetCurrentTimeMs();
    IConnection *connection = GetConnection(connectionid);
    ConnectionInfo &info = mConnectionInfo[connection];
    return (ms - info.tick > 1000);
}

void ConnectionManager::Process()
{
    int ms = GetCurrentTimeMs();
    if (mClientConnector)
        mClientConnector->Process();
    if (mServerConnector)
        mServerConnector->Process();
    std::vector<IConnection *> remove;
    for (std::vector<IConnection *>::iterator i = mConnections.begin(), e = mConnections.end(); i != e; ++i)
    {
        if (*i)
        {
            ConnectionInfo &info = mConnectionInfo[*i];
            if (ms > info.sentTick + 2000)
            {
                info.sentTick = ms;
                char tick[] = { MESSAGE_TICK, 0 };
                (*i)->Send(tick, 2, false, false);
//              printf("%c", 'a' + (*i)->GetId());
            }
            if (info.tick == 0)
                info.tick = ms; // initialize
            /*
            if (ms - info.tick > 1000)
            {
                printf("Haven't heard from %d in a second!\n", (*i)->GetId());
            }
            if (ms - info.tick > 10000)
            {
                remove.push_back(*i);
            }
            */
            (*i)->Process();
        }
    }
    for (std::vector<IConnection *>::iterator i = remove.begin(), e = remove.end(); i != e; ++i)
    {
        RemoveConnectionAt(GetIndex(*i));
        //(*i)->Close();
        delete *i;
    }

    //ms = GetCurrentTimeMs() - ms;
    //printf("ConnectionManager process time: %d\n", ms);
}

void ConnectionManager::SetOnConnect(OnConnectCallback callback, void *user)
{ 
    mConnectCallback = callback; mConnectUser = user;
}

void ConnectionManager::SetOnReceive(OnReceiveCallback callback, void *user)
{ 
    mReceiveCallback = callback; mReceiveUser = user;
}

void ConnectionManager::SetOnDisconnect(OnDisconnectCallback callback, void *user)
{ 
    mDisconnectCallback = callback; mDisconnectUser = user;
}

void ConnectionManager::SetClientConnector(IClientConnector *connector)
{
    if (connector)
    {
        if (mClientConnector) // if there was a previous one..
            mClientConnector->SetOnConnect(0, 0);
        connector->SetOnConnect(OnConnectHook, this);
    }
    mClientConnector = connector;
}

void ConnectionManager::SetServerConnector(IServerConnector *connector)
{
    if (connector)
    {
        if (mServerConnector) // if there was a previous one..
            mServerConnector->SetOnConnect(0, 0);
        connector->SetOnConnect(OnConnectHook, this);
    }
    mServerConnector = connector;
}

void ConnectionManager::OnConnectHook(IConnection *connection, void *user)
{
    ConnectionManager *self = reinterpret_cast<ConnectionManager *>(user);
    self->OnConnect(connection);
}

void ConnectionManager::OnReceiveHook(int connectionid, const void *data, int length, void *user)
{
    ConnectionManager *self = reinterpret_cast<ConnectionManager *>(user);
    self->OnReceive(connectionid, data, length);
}

void ConnectionManager::OnDisconnectHook(int connectionid, int reason, void *user)
{
    ConnectionManager *self = reinterpret_cast<ConnectionManager *>(user);
    self->OnDisconnect(connectionid, reason);
}

void ConnectionManager::OnConnect(IConnection *connection)
{
    AddConnection(connection);
    connection->SetOnDisconnect(OnDisconnectHook, this);
    connection->SetOnReceive(OnReceiveHook, this);
    char message[MESSAGE_NAME_LENGTH + MESSAGE_NAME_LENGTH];
    memset(message, 0, sizeof(message));
    message[0] = MESSAGE_NAME;
    strncpy(message + 1, mName.c_str(), MESSAGE_NAME_LENGTH - 2);
    connection->Send(message, MESSAGE_NAME_LENGTH, true);
    if (mConnectCallback)
        mConnectCallback(connection, mConnectUser);
}

ConnectionManager::ConnectionInfo &ConnectionManager::GetInfo(IConnection *connection)
{
    return mConnectionInfo[connection];
}

void ConnectionManager::Tick()
{
    char tick[] = { MESSAGE_TICK, 0 };
    SendAllRaw(tick, 2, false, false);
}

void ConnectionManager::OnReceive(int connectionid, const void *data, int length)
{
    const char *chdata = static_cast<const char *>(data);
    IConnection *connection = GetConnection(connectionid);
    switch (*chdata)
    {
    case MESSAGE_CHANGE_ID:
        {
            int32_t newid = 0;
            memcpy(&newid, chdata + 1, sizeof(int32_t));
            ChangeIdInternal(connection->GetId(), newid);
        }
        break;
    case MESSAGE_NAME:
        {
            ConnectionInfo &info = GetInfo(connection);
            info.connection = connection;
            memset(info.name, 0, sizeof(info.name));
            strncpy(info.name, chdata + 1, MESSAGE_NAME_LENGTH - 2);
        }        
        break;
    case MESSAGE_TICK:
        {
            ConnectionInfo &info = GetInfo(connection);
            info.connection = connection;
            info.tick = GetCurrentTimeMs();
            printf("%d", connection->GetId());
        }        
        break;
    case MESSAGE_PAYLOAD:
        if (mReceiveCallback)
            mReceiveCallback(connectionid, (chdata + 1), length - 1, mReceiveUser);
        break;
    }
}

int ConnectionManager::GetIdByIndex(int at) const
{ 
    const IConnection *c = GetConnectionAt(at);
    return !c ? -1 : c->GetId();
}

bool ConnectionManager::ChangeId(int oldid, int newid)
{
    int index = GetIndexById(oldid);
    if (index == -1) return false;
    IConnection *connection = GetConnectionAt(index);
    if (!connection) return false;
    char message[1 + sizeof(int32_t)] = { MESSAGE_CHANGE_ID, 0 };
    memcpy(message + 1, &newid, sizeof(int32_t));
    if (!connection->Send(message, sizeof(message), true, true))
        return false;
    ChangeIdInternal(oldid, newid);
}

bool ConnectionManager::ChangeIdInternal(int oldid, int newid)
{
    if (oldid == -1) return false;
    int index = GetIndexById(oldid);
    if (index == -1) return false;
    IConnection *c = GetConnectionAt(index);
    if (!c) return false;
    c->SetId(newid);
}

bool ConnectionManager::CloseById(int connectionid)
{
    if (connectionid == -1) return false;
    int index = GetIndexById(connectionid);
    if (index == -1) return false;
    IConnection *c = GetConnectionAt(index);
    if (!c) return false;
    return c->Close();
}

void ConnectionManager::OnDisconnect(int connectionid, int reason)
{
    CloseConnectionAt(connectionid);
    if (mDisconnectCallback)
        mDisconnectCallback(connectionid, reason, mReceiveUser);
}

bool ConnectionManager::SendInternal(IConnection *connection, const char *data, int length, bool reliable, bool sequenced)
{
    char staticbuffer[BUFFER_SIZE + 2];
    char *buffer = staticbuffer;
    bool dynamic = length + 1 > BUFFER_SIZE;
    if (dynamic)
        buffer = new char [length + 1];
    *buffer = MESSAGE_PAYLOAD;
    memcpy(buffer + 1, data, length);
    bool result = connection->Send(buffer, length + 1, reliable, sequenced);
    if (dynamic)
        delete [] buffer;
    return result;
}

// convenience functions
bool ConnectionManager::Send(int connectionid, const char *data, int length, bool reliable, bool sequenced)
{
    IConnection *connection = GetConnectionAt(GetIndexById(connectionid));
    if (!connection)
        return false;
    return SendInternal(connection, data, length, reliable, sequenced);
}

bool ConnectionManager::SendAll(const char *data, int length, bool reliable, bool sequenced)
{
    bool success = true;
    for (int i = 0, sz = static_cast<int>(GetConnectionCount()); i < sz; i++)
    {
        IConnection *connection = GetConnectionAt(i);
        if (connection)
            success &= SendInternal(connection, data, length, reliable, sequenced);
    }
    return success;
}

bool ConnectionManager::SendRaw(int connectionid, const char *data, int length, bool reliable, bool sequenced)
{
    IConnection *connection = GetConnectionAt(GetIndexById(connectionid));
    if (!connection)
        return false;
    return SendInternal(connection, data, length, reliable, sequenced);
}

bool ConnectionManager::SendAllRaw(const char *data, int length, bool reliable, bool sequenced)
{
    bool success = true;
    for (int i = 0, sz = static_cast<int>(GetConnectionCount()); i < sz; i++)
    {
        IConnection *connection = GetConnectionAt(i);
        if (connection)
            success &= connection->Send(data, length, reliable, sequenced);
    }
    return success;
}

bool ConnectionManager::IsServer() const
{
    return mServerConnector != 0;
}

void ConnectionManager::PrintConnections()
{
    if (!mConnectionInfo.empty())
    {
    printf("Connections\n-----------\n");
    for (std::map<const IConnection *, ConnectionInfo>::const_iterator i = mConnectionInfo.begin(), e = mConnectionInfo.end(); i != e; ++i)
        printf("#%02d Addr %p Name '%s'\n", (*i).first->GetId(), (*i).first, (*i).second.name);
    printf("-----------\n\n");
    }
}

const char *ConnectionManager::GetName(int connectionid) const
{
    const IConnection *connection = GetConnection(connectionid);
    if (!connection)
        return 0;
    std::map<const IConnection *, ConnectionInfo>::const_iterator i = mConnectionInfo.find(connection), e = mConnectionInfo.end();
    if (i == e)
        return 0;
    return (*i).second.name;    
}

}
