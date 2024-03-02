#ifndef _NETGAMESERVER_H
#define _NETGAMESERVER_H

#include "NetGame.h"

class UdpConnection;

class NetGameServer : public INetGameParticipant
{
public:

    enum { HOST = 0, PLAYER_COUNT = 4 };

    typedef bool (*ServerReceiveCallback)(void *userdata, int player, const char *message, int length);

    NetGameServer(UdpConnection *connection);
    ~NetGameServer();

    void SetName(const char *s);

    bool Process();
    
    bool SendTo     (int player, const char *data, int length);
    int  ReceiveFrom(int *player, char *data, int length);

    bool SendGameData(int player, const char *data, int length);
    bool BroadcastGameData(const char *data, int length);

    void SetReceiveCallback(ServerReceiveCallback callback, void *userdata)
    {
        mReceiveCallback = callback;
        mReceiveUserData = userdata;
    }

    int   GetAddressIndex(const Address &address);

    bool  IsGameFull() const { return mPlayerCount >= PLAYER_COUNT; }

    int   GetId() const { return HOST; }

    int   GetTime() const;

    bool  IsServer() const { return true; }

    NetPlayerInfo &GetPlayer(int i) { return mPlayers[i]; }

private:

    bool ProcessOne();

    int             Add(const Address &addr, const NetGameMessage *m);
    NetError::Error Join(const Address &addr, const NetGameMessage *m);
    bool            ProcessClient(const Address &from, const NetGameMessageId &m, int messageSize);
    void            ProcessTimeouts();
    void            SendPing();
    bool            IsUnconnected(int i) const;
    int             GetUnconnected() const;
    void            DropClient(int index);
    void            SendName(const Address &to);
    void            SendError(const Address &to, NetError::Error error);
    bool            ReceiveGameData(int clientId, const char *message, int length);

    UdpConnection         *mConnection;
    char                  *mMessage;
    int                    mPlayerCount;
    NetPlayerInfo          mPlayers[PLAYER_COUNT];
    NetString<NAME_SIZE>   mName;
    ServerReceiveCallback  mReceiveCallback;
    void                  *mReceiveUserData;
};

#endif // _NETGAMESERVER_H