#include "NetGameClient.h"
#include "Address.h"
#include "NetGame.h"
#include "UdpChannel.h"
#include "serialize/Codec.h"

#include "time/Timer.h"

NetGameClient::NetGameClient(UdpChannel *channel) : mChannel(channel), mMessage(0), mId(-1), mComplete(0), mUser(0), mTimestamp(0), mPingSent(0), mReceiveCallback(0), mReceiveUserData(0)
{
    mMessage = new char [MAX_MESSAGE_SIZE];
}

void NetGameClient::SetName(const char *s)
{
    mName.Set(s);
}

NetGameClient::~NetGameClient()
{
    delete [] mMessage;
    mMessage = 0;
}

void NetGameClient::Join(const Address &host, complete_fn_t complete, void *user)
{
    NetGameMessage_String<NAME_SIZE> m;
    m.type    = NetGameMessage::CLI_JOIN_REQUEST;
    m.string  = mName;

    mHost     = host;
    mComplete = complete;
    mUser     = user;

    printf("** Sending JOIN_REQUEST!\n");
    if (!mChannel->SendTo(mHost, (const char *)&m, sizeof(m)))
        Complete(NetError::ERR_CANT_SEND);
}

bool NetGameClient::IsJoined() const
{
    return mId >= 0;
}
    
bool NetGameClient::Send(const char *data, int length)
{
    return mChannel->SendTo(mHost, data, length);
}

int NetGameClient::Receive(char *data, int length)
{
    Address host;
    int sz = mChannel->ReceiveFrom(host, data, length);
    return (host == mHost)? sz : 0;
}

void NetGameClient::SendError(NetError::Error error)
{
    NetGameMessage_Error response;
    response.type  = NetGameMessage::SVR_ERROR;
    response.error = error;
    mChannel->SendTo(mHost, (const char *)&response, sizeof(response));
}

void NetGameClient::SendResponse(NetGameMessage::Type type)
{
    NetGameMessageId response;
    response.clientId = (unsigned char)mId;
    response.type     = type;
    mChannel->SendTo(mHost, (const char *)&response, sizeof(NetGameMessageId));
}

bool NetGameClient::ProcessTimeouts()
{
    return false;
}

void NetGameClient::SendPing()
{
    int time = GetTime();
    if ((time - mPingSent) >= PING_MILLISECONDS && mId >= 0)
    {
        NetGameMessageId message;
        message.type     = (unsigned char)(NetGameMessage::CLI_PING | NetGameMessage::HAS_ID);
        message.clientId = (unsigned char)mId;
        Send((const char *)&message, sizeof(message));
        mPingSent = time;
    }
}

void NetGameClient::SendGameData(const char *data, int length)
{
    NetGameMessageId response;
    response.clientId = (unsigned char)mId;
    response.type     = (unsigned char)(NetGameMessage::CLI_GAME_DATA | NetGameMessage::HAS_ID);
    memcpy(mMessage, &response, sizeof(NetGameMessageId));
    // here we want to compress data, length first
    int size = Codec::Encode(data, length, mMessage + sizeof(NetGameMessageId));
    mChannel->SendTo(mHost, mMessage, size + sizeof(NetGameMessageId));
}

bool NetGameClient::ReceiveGameData(const char *message, int length)
{
    if (!mReceiveCallback)
        return false;
    // we should use length to verify or limit the size, right?
    char out[Codec::MAX_DECODE_SIZE];
    int size = Codec::Decode(message, out);
    return mReceiveCallback(mReceiveUserData, out, size);
}

void NetGameClient::SendName()
{
    NetGameMessageId_String<NAME_SIZE> response;
    response.type   = (unsigned char)(NetGameMessage::NAME_RESPONSE | NetGameMessage::HAS_ID);
    response.string = mName;
    mChannel->SendTo(mHost, (const char *)&response, sizeof(NetGameMessageId));
}

bool NetGameClient::Process()
{
    bool anyProcessed = false;
    while (ProcessOne())
        anyProcessed = true;
    return anyProcessed;
}

bool NetGameClient::ProcessOne()
{
    SendPing();
    ProcessTimeouts();
    int messageSize = Receive(mMessage, MAX_MESSAGE_SIZE);
    if (messageSize <= 0)
        return false;
    NetGameMessage *message = (NetGameMessage *)mMessage;
    // set up a client
    mTimestamp = GetTime();
    switch (message->GetType())
    {
    case NetGameMessage::SVR_PING: // don't need to do anything, just needed to update timestamp
        break;
    case NetGameMessage::SVR_JOIN_ACCEPT:
        printf("** Received JOIN_ACCEPT!\n");
        mId = (int)((NetGameMessage_JoinAccept *)message)->assignedId;
        SendResponse((NetGameMessage::Type)(NetGameMessage::CLI_JOIN_ACCEPT_ACK | NetGameMessage::HAS_ID));
        Complete(NetError::ERR_NONE);
        break;
    case NetGameMessage::NAME_REQUEST:
        SendName();
        break;
    case NetGameMessage::SVR_ERROR:
        Complete((NetError::Error)(((NetGameMessage_Error *)message)->error));
        break;
    case NetGameMessage::SVR_GAME_DATA:
        return ReceiveGameData((const char *)message + sizeof(NetGameMessageId), messageSize - sizeof(NetGameMessageId));
    }
    return true;
}

void NetGameClient::Complete(NetError::Error error)
{
    if (mComplete)
        mComplete(mUser, error);
    mComplete = 0;
    mUser     = 0;
}
    
int NetGameClient::GetTime() const
{
    return GetCurrentTimeMs();
}