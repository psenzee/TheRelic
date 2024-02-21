#ifndef _MULTIPLAYER_H
#define _MULTIPLAYER_H

class INetGameParticipant;
class UdpChannel;
class NetGameClient;
class NetGameServer;

#include "net/NetGame.h"

#include <vector>

struct MessageListener
{
    typedef bool (*NotifyCallback)(void *userdata, const char *message, int length);

    NotifyCallback  callback;
    void           *userdata;

    inline MessageListener(NotifyCallback callback = 0, void *userdata = 0) 
        : callback(callback), userdata(userdata) {}

    inline bool Notify(const char *message, int length)
    {
        if (callback) return callback(userdata, message, length);
        return false;
    }
};

class Multiplayer
{
public:

    enum { MAX_PLAYERS = 4 };

    Multiplayer();
    
    bool                 StartServer(int port, const char *profileName);
    bool                 StartClient(const char *profileName);

    void                 SetProfileName(const char *name);

    bool                 Join(const char *address, int port);
    bool                 IsJoined();
    int                  GetId() const { return mConnection ? mConnection->GetId() : -1; }

    void                 SendGameData(const char *data, int length);

    NetGameClient       *GetClient();
    const NetGameClient *GetClient() const;
    NetGameServer       *GetServer();
    const NetGameServer *GetServer() const;

    bool                 Update();

    bool                 IsActiveConnection() const;

    void                 AddMessageListener(MessageListener &listener);

private:

    static bool ClientReceive      (void *userdata, const char *message, int length);
    static bool ServerReceive      (void *userdata, int player, const char *message, int length);
    static void ClientCompleteJoin (void *userdata, NetError::Error error);

    INetGameParticipant          *mConnection;
    UdpChannel                   *mChannel;
    NetError::Error               mJoinError;
    bool                          mJoinSuccess,
                                  mJoinComplete;
    std::vector<MessageListener>  mListeners;
    int                           mPlayerId;
};

#endif // _MULTIPLAYER_H