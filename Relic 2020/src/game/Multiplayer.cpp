#include "Multiplayer.h"

#include "net/ConnectionManager.h"
#include "net/ENetUtils.h"
#include "net/NetGame.h"
#include "luautil/LuaInterpreter.h"
#include "luautil/LuaCall.h"
#include "serialize/Pack.h"

namespace Serialize
{
extern int GetThisUserId();
}

Multiplayer::Multiplayer() : 
    mJoinComplete(false), mConnectionManager(0), mPlayerId(-1), mConnectionEvent(NONE),
    mConnectionEventConnectionId(-1), mConnectionEventReason(-1), mOnReceiveCharacterUpdates(0), mOnReceiveCharacterUpdatesUser(0)
{
}

static void ServerError()
{
    printf("Unable to open server!\n");
}

static void ClientError()
{
    printf("Unable to connect to server!\n");
}

static void ClientErrorServerFull()
{
    printf("Unable to connect: Server full!\n");
}

void Multiplayer::AddMessageListener(MessageListener &listener)
{
    mListeners.push_back(listener);
}

bool Multiplayer::StartServer(int port, const char *profileName)
{
    /*
    if (!mConnection) // is already initialized?
    {
        mChannel = GetUdpConnection(port);
        if (!mChannel)
            return false;
        mConnection = new NetGameServer(mChannel);
        SetProfileName(profileName);
        GetServer()->SetReceiveCallback(ServerReceive, this);
    }
    return true;
    */
    return false;
}
/*
template <typename T>
static void Pack(char **to, const T &v)
{
    memcpy(*to, &v, sizeof(T));
    (*to) += sizeof(T);
}

template <typename T>
static void Unpack(const char **from, T &v)
{
    memcpy(&v, *from, sizeof(T));
    (*from) += sizeof(T);
}
*/

bool Multiplayer::SendGameData(const char *data, int length, bool reliable, bool sequenced)
{
    if (!mConnectionManager)
        return false;
    return mConnectionManager->SendAll(data, length, true/*reliable*/, true/*sequenced*/);
}

bool Multiplayer::SendGameData(int id, const char *data, int length, bool reliable, bool sequenced)
{
    if (!mConnectionManager)
        return false;
    bool result = false;
    if (id == -1) result = mConnectionManager->SendAll(data, length, true/*reliable*/, true/*sequenced*/);
    else          result = mConnectionManager->Send(id, data, length, true/*reliable*/, true/*sequenced*/);
    return result;
}

void Multiplayer::SetProfileName(const char *name)
{
    /*
    if (mConnection)
        mConnection->SetName(name);
    */
}

bool Multiplayer::IsActiveConnection() const
{
    return mConnectionManager != 0 && mConnectionManager->GetConnectionCount() > 0;
}

void Multiplayer::Close()
{
    if (mConnectionManager)
        delete mConnectionManager;
    mConnectionManager = 0;
}

bool Multiplayer::StartENetClient(const char *name, const char *address, int port)
{
    Close();
    printf("WiFi client starting..\n");
    mJoinComplete = false;
    mConnectionManager = CreateENetClientConnectionManager(address, port);
    mConnectionManager->SetOnConnect(Multiplayer_OnConnectClient, this);
    mConnectionManager->SetOnDisconnect(Multiplayer_OnDisconnectClient, this);
    mConnectionManager->SetOnReceive(Multiplayer_OnReceiveClient, this);
    return mConnectionManager->Start(name);
}

bool Multiplayer::StartENetServer(const char *name, int port)
{
    Close();
    printf("WiFi server starting..\n");
    mJoinComplete = false;
    mConnectionManager = CreateENetServerConnectionManager(port);
    mConnectionManager->SetOnConnect(Multiplayer_OnConnectServer, this);
    mConnectionManager->SetOnDisconnect(Multiplayer_OnDisconnectServer, this);
    mConnectionManager->SetOnReceive(Multiplayer_OnReceiveServer, this);
    return mConnectionManager->Start(name);
}

bool Multiplayer::StartBluetoothServer(const char *name)
{
    Close();
    printf("Bluetooth server starting..\n");
    mJoinComplete = false;
    mConnectionManager = CreateBluetoothServerConnectionManager();
    mConnectionManager->SetOnConnect(Multiplayer_OnConnectServer, this);
    mConnectionManager->SetOnDisconnect(Multiplayer_OnDisconnectServer, this);
    mConnectionManager->SetOnReceive(Multiplayer_OnReceiveServer, this);
    return mConnectionManager->Start(name);
}

bool Multiplayer::StartBluetoothClient(const char *name)
{
    Close();
    printf("Bluetooth client starting..\n");
    mJoinComplete = false;
    mConnectionManager = CreateBluetoothClientConnectionManager();
    mConnectionManager->SetOnConnect(Multiplayer_OnConnectClient, this);
    mConnectionManager->SetOnDisconnect(Multiplayer_OnDisconnectClient, this);
    mConnectionManager->SetOnReceive(Multiplayer_OnReceiveClient, this);
    return mConnectionManager->Start(name);
}

bool Multiplayer::StartClient(const char *profileName)
{
    /*
    if (!mConnection) // is already initialized?
    {
        mChannel = GetUdpConnection(0);
        if (!mChannel)
            return false;
        mConnection = new NetGameClient(mChannel);
        SetProfileName(profileName);
        GetClient()->SetReceiveCallback(ClientReceive, this);
    }
    */
    return true;
}
/*
NetGameClient *Multiplayer::GetClient()
{
    return (mConnection && mConnection->IsServer()) ? 0 : static_cast<NetGameClient *>(mConnection);
}

NetGameServer *Multiplayer::GetServer()
{
    return (mConnection && mConnection->IsServer()) ? static_cast<NetGameServer *>(mConnection) : 0;
}

const NetGameClient *Multiplayer::GetClient() const
{
    return (mConnection && mConnection->IsServer()) ? 0 : static_cast<const NetGameClient *>(mConnection);
}

const NetGameServer *Multiplayer::GetServer() const
{
    return (mConnection && mConnection->IsServer()) ? static_cast<const NetGameServer *>(mConnection) : 0;
}
*/

bool Multiplayer::Join(const char *address, int port)
{
    return false;
}

bool Multiplayer::IsJoined()
{
    return mJoinComplete;
}

bool Multiplayer::Update()
{
    if (!mConnectionManager)
        return false;
    mConnectionManager->Process();
    switch (mConnectionEvent)
    {
    case CLIENT_CONNECT:
        {
            LuaValue values[] = {  LuaValue::Int(mConnectionEventConnectionId) };
	        LuaCall(LuaInterpreter::GetInstance()->GetState(), "Client_OnConnect", values, 1);  
        }
        break;
    case CLIENT_DISCONNECT:
        {
            LuaValue values[] = {  LuaValue::Int(mConnectionEventConnectionId), LuaValue::Int(mConnectionEventReason) };
	        LuaCall(LuaInterpreter::GetInstance()->GetState(), "Client_OnDisconnect", values, 2);
        }
        break;
    case SERVER_CONNECT:
        {
            LuaValue values[] = {  LuaValue::Int(mConnectionEventConnectionId) };
	        LuaCall(LuaInterpreter::GetInstance()->GetState(), "Server_OnConnect", values, 1);
        }
        break;
    case SERVER_DISCONNECT:
        {
            LuaValue values[] = {  LuaValue::Int(mConnectionEventConnectionId), LuaValue::Int(mConnectionEventReason) };
	        LuaCall(LuaInterpreter::GetInstance()->GetState(), "Server_OnDisconnect", values, 2);
        }
        break;
    }
    mConnectionEvent = NONE;
    return true;
}

void Multiplayer::Multiplayer_OnConnectClient(Connection::IConnection *connection, void *user)
{
    Multiplayer *mp = static_cast<Multiplayer *>(user);
    mp->OnConnectClient(connection);
}

void Multiplayer::Multiplayer_OnReceiveClient(int connectionid, const void *data, int length, void *user)
{
    Multiplayer *mp = static_cast<Multiplayer *>(user);
    mp->OnReceiveClient(connectionid, data, length);
}

void Multiplayer::Multiplayer_OnDisconnectClient(int connectionid, int reason, void *user)
{
    Multiplayer *mp = static_cast<Multiplayer *>(user);
    mp->OnDisconnectClient(connectionid, reason);
}

void Multiplayer::Multiplayer_OnConnectServer(Connection::IConnection *connection, void *user)
{
    Multiplayer *mp = static_cast<Multiplayer *>(user);
    mp->OnConnectServer(connection);
}

void Multiplayer::Multiplayer_OnReceiveServer(int connectionid, const void *data, int length, void *user)
{
    Multiplayer *mp = static_cast<Multiplayer *>(user);
    mp->OnReceiveServer(connectionid, data, length);
}

void Multiplayer::Multiplayer_OnDisconnectServer(int connectionid, int reason, void *user)
{
    Multiplayer *mp = static_cast<Multiplayer *>(user);
    mp->OnDisconnectServer(connectionid, reason);
}

void Multiplayer::OnConnectClient(Connection::IConnection *connection)
{
    mJoinComplete = true;
    mConnectionEvent = CLIENT_CONNECT;
    mConnectionEventConnectionId = connection->GetId();
    mConnectionEventReason = -1;
}

void Multiplayer::OnReceiveClient(int connectionid, const void *data, int length)
{
    if (ReceiveMessage(static_cast<const char *>(data), length))
        return;
    for (std::vector<MessageListener>::iterator i = mListeners.begin(), e = mListeners.end(); i != e; ++i)
        if ((*i).Notify(static_cast<const char *>(data), length))
            return;
}

void Multiplayer::OnDisconnectClient(int connectionid, int reason)
{
    mJoinComplete = false;
    mConnectionEvent = CLIENT_DISCONNECT;
    mConnectionEventConnectionId = connectionid;
    mConnectionEventReason = reason;
}

void Multiplayer::OnConnectServer(Connection::IConnection *connection)
{
    mJoinComplete = true;
    mConnectionEvent = SERVER_CONNECT;
    mConnectionEventConnectionId = connection->GetId();
    mConnectionEventReason = -1;
}

void Multiplayer::OnReceiveServer(int connectionid, const void *data, int length)
{
    // echo first
	for (int i = 0, sz = mConnectionManager->GetConnectionCount(); i < sz; i++)
	{
        Connection::IConnection *connection = mConnectionManager->GetConnectionAt(i);
		// $TODO should we do it like this?
		if (connectionid != connection->GetId()) // don't send it back to the player that sent it
		{
			SendGameData(connection->GetId(), (const char *)data, length, true, true);
		}
	}
    if (ReceiveMessage(static_cast<const char *>(data), length))
        return;
	for (std::vector<MessageListener>::iterator i = mListeners.begin(), e = mListeners.end(); i != e; ++i)
		if ((*i).Notify(static_cast<const char *>(data), length))
			return;

    /*
    for (int i = 0; i < NetGameServer::PLAYER_COUNT; i++)
    {
        if (i == NetGameServer::HOST)
            ClientReceive(self, message, length); // handle it
        else if (i != player) // don't send it back to the player that sent it
            self->GetServer()->SendGameData(i, message, length);
    }
    */
}

void Multiplayer::OnDisconnectServer(int connectionid, int reason)
{
    mJoinComplete = false;
    mConnectionEvent = CLIENT_DISCONNECT;
    mConnectionEventConnectionId = connectionid;
    mConnectionEventReason = reason;
}

bool Multiplayer::SendTypedData(unsigned short type, const char *message, int length, bool reliable, bool sequenced)
{
    return SendTypedData(-1, type, message, length, reliable, sequenced);
}

bool Multiplayer::SendDataMessage(const char *message, int length, bool reliable, bool sequenced)
{
    return SendTypedData(static_cast<unsigned short>(RAW_DATA), message, length, reliable, sequenced);
}

bool Multiplayer::SendLuaMessage(const char *message, int length, bool reliable, bool sequenced)
{
    return SendTypedData(static_cast<unsigned short>(LUA_MESSAGE), message, length, reliable, sequenced);
}

bool Multiplayer::SendTypedData(int id, unsigned short type, const char *message, int length, bool reliable, bool sequenced)
{
    if (!IsActiveConnection())
        return false;
    char buffer[MAX_MESSAGE_SIZE];
    char *pb = buffer;
    PackUInt16(&pb, type);
    memcpy(pb, message, length);
    SendGameData(id, buffer, length + sizeof(unsigned short), reliable, true/*sequenced*/);
    return true;
}

bool Multiplayer::SendDataMessage(int id, const char *message, int length, bool reliable, bool sequenced)
{
    return SendTypedData(id, static_cast<unsigned short>(RAW_DATA), message, length, reliable, sequenced);
}

bool Multiplayer::SendLuaMessage(int id, const char *message, int length, bool reliable, bool sequenced)
{
    return SendTypedData(id, static_cast<unsigned short>(LUA_MESSAGE), message, length, reliable, sequenced);
}

const std::string &Multiplayer::NextMessage(Queue queue) const
{
    static const std::string empty;
    return mMessageQueues[queue].empty() ? empty : mMessageQueues[queue].front();
}

void Multiplayer::RemoveMessage(Queue queue)
{
    if (!mMessageQueues[queue].empty())
        mMessageQueues[queue].pop_front();
}

bool Multiplayer::HasMessage(Queue queue) const
{
    return !mMessageQueues[queue].empty();
}

void Multiplayer::ClearMessages(Queue queue)
{
    mMessageQueues[queue].clear();
}

bool Multiplayer::ReceiveMessage(Queue queue, const char *message, int length)
{
    mMessageQueues[queue].push_back(""); // do it this way to avoid allocating and copying twice
    mMessageQueues[queue].back().assign(message, length);
    return true;
}

void Multiplayer::SetOnReceiveCharacterUpdates(OnReceiveCharacterUpdates callback, void *user)
{
    mOnReceiveCharacterUpdates     = callback;
    mOnReceiveCharacterUpdatesUser = user;
}

bool Multiplayer::ReceiveMessage(const char *message, int length)
{
    if (!IsActiveConnection())
        return false;
    unsigned short type = UnpackUInt16(&message);
    length -= sizeof(type);
    switch (type)
    {
    case CHARACTER_UPDATE:
        if (mOnReceiveCharacterUpdates)
            return mOnReceiveCharacterUpdates(message, length, mOnReceiveCharacterUpdatesUser);
        return false;
    case LUA_MESSAGE:      
        {
            /*
            if (mConnectionManager->IsServer())
            {
                // here we need to echo this out to other clients
                enum { PLAYER_FROM_FIELD = 3, PLAYER_TO_FIELD = 4, MAX_PLAYERS = 4 };
                int id     = Serialize::GetThisUserId(),
                    fromId = message[PLAYER_FROM_FIELD],
                    toId   = message[PLAYER_TO_FIELD];
                for (int i = 0; i < MAX_PLAYERS; i++)
                {
                    if (i == id || i == fromId || i == toId)
                        continue;
           //       SendLuaMessage(const char *message, int length, bool reliable, bool sequenced);
                }
            }
//          printf("RPC: %s\n", message + 5);			
*/
            return ReceiveMessage(LUA_MESSAGE_QUEUE,  message, length);
        }
    case RAW_DATA:         return ReceiveMessage(DATA_MESSAGE_QUEUE, message, length);
    default:
        // unrecognized message
        break;
    }
    return false;
}