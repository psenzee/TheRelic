#include "Multiplayer.h"

#include "net/UdpChannel.h"
#include "net/NetGameClient.h"
#include "net/NetGameServer.h"

Multiplayer::Multiplayer() : mConnection(0), mChannel(0), mJoinError(NetError::ERR_NONE), mJoinSuccess(false), mJoinComplete(false), mPlayerId(-1)
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

static UdpChannel *GetUdpChannel(int port) // port zero means client
{
    UdpChannel *channel = new UdpChannel(port);
    if (!channel->Open())
    {
        if (port != 0) ServerError();
        else           ClientError();
        delete channel;
        channel = 0;
    }
    return channel;
}

void Multiplayer::AddMessageListener(MessageListener &listener)
{
    mListeners.push_back(listener);
}

bool Multiplayer::StartServer(int port, const char *profileName)
{
    if (!mConnection) // is already initialized?
    {
        mChannel = GetUdpChannel(port);
        if (!mChannel)
            return false;
        mConnection = new NetGameServer(mChannel);
        SetProfileName(profileName);
        GetServer()->SetReceiveCallback(ServerReceive, this);
    }
    return true;
}

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

void Multiplayer::SendGameData(const char *data, int length)
{
    if      (GetServer())
        GetServer()->BroadcastGameData(data, length);
    else if (GetClient())
        GetClient()->SendGameData(data, length);
}

void Multiplayer::SetProfileName(const char *name)
{
    if (mConnection)
        mConnection->SetName(name);
}

bool Multiplayer::IsActiveConnection() const
{
    return mConnection ? ((GetClient() && GetClient()->IsJoined()) || GetServer()) : false;
}

bool Multiplayer::StartClient(const char *profileName)
{
    if (!mConnection) // is already initialized?
    {
        mChannel = GetUdpChannel(0);
        if (!mChannel)
            return false;
        mConnection = new NetGameClient(mChannel);
        SetProfileName(profileName);
        GetClient()->SetReceiveCallback(ClientReceive, this);
    }
    return true;
}

NetGameClient *Multiplayer::GetClient()
{
    return mConnection->IsServer() ? 0 : static_cast<NetGameClient *>(mConnection);
}

NetGameServer *Multiplayer::GetServer()
{
    return mConnection->IsServer() ? static_cast<NetGameServer *>(mConnection) : 0;
}

const NetGameClient *Multiplayer::GetClient() const
{
    return mConnection->IsServer() ? 0 : static_cast<const NetGameClient *>(mConnection);
}

const NetGameServer *Multiplayer::GetServer() const
{
    return mConnection->IsServer() ? static_cast<const NetGameServer *>(mConnection) : 0;
}

bool Multiplayer::Join(const char *address, int port)
{
    NetGameClient *client = GetClient();
    if (!client)
        return false;
    Address host;
    if (!host.Resolve(address, port))
    {
        ClientError();
        return false;
    }
    client->Join(host, ClientCompleteJoin, this);
    return true;
}

bool Multiplayer::IsJoined()
{
    NetGameClient *client = GetClient();
    return client ? client->IsJoined() : false;
}

void Multiplayer::ClientCompleteJoin(void *user, NetError::Error error)
{
    Multiplayer *multiplayer   = reinterpret_cast<Multiplayer *>(user);
    multiplayer->mJoinError    = error;
    multiplayer->mJoinComplete = true;
    multiplayer->mJoinSuccess  = false;
    if (error == NetError::ERR_NONE)
    {
        printf("Game joined as player #%d\n", multiplayer->mConnection->GetId());
        multiplayer->mJoinSuccess = true;
    }
    else if (error == NetError::ERR_SERVER_FULL)
        ClientErrorServerFull();
    else
        ClientError();
}

bool Multiplayer::ClientReceive(void *userdata, const char *message, int length)
{
    Multiplayer *self = static_cast<Multiplayer *>(userdata);
    if (self->mConnection)
    {
        for (std::vector<MessageListener>::iterator i = self->mListeners.begin(), e = self->mListeners.end(); i != e; ++i)
            if ((*i).Notify(message, length))
                return true;
    }
    return false;
}

bool Multiplayer::ServerReceive(void *userdata, int player, const char *message, int length)
{
    Multiplayer *self = static_cast<Multiplayer *>(userdata);
    for (int i = 0; i < NetGameServer::PLAYER_COUNT; i++)
    {
        if (i == NetGameServer::HOST)
            ClientReceive(self, message, length); // handle it
        else if (i != player) // don't send it back to the player that sent it
            self->GetServer()->SendGameData(i, message, length);
    }
    return true;
}

bool Multiplayer::Update()
{
    return !mConnection ? false : mConnection->Process(); 
}