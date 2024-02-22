#ifndef _MULTIPLAYER_H
#define _MULTIPLAYER_H

#include "net/ConnectionManager.h"

#include <vector>
#include <deque>
#include <string>

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

typedef bool (*OnReceiveCharacterUpdates)(const char *message, int length, void *user);

class Multiplayer
{
public:

    enum       { MAX_PLAYERS = 4 };

    enum Queue { DATA_MESSAGE_QUEUE = 0, LUA_MESSAGE_QUEUE, MAX_QUEUES };
    enum       { CHARACTER_UPDATE = 23, LUA_MESSAGE, RAW_DATA };

    Multiplayer();
    
    bool                 StartServer(int port, const char *profileName);
    bool                 StartClient(const char *profileName);

    bool                 StartENetServer(const char *name, int port);
    bool                 StartENetClient(const char *name, const char *address, int port);
    bool                 StartBluetoothServer(const char *name);
    bool                 StartBluetoothClient(const char *name);

    void                 SetProfileName(const char *name);

    bool                 Join(const char *address, int port);
    bool                 IsJoined();

    void                 SetId(int id) { mPlayerId = id; }
    int                  GetId() const { return mPlayerId; }

    bool                 SendGameData(const char *data, int length, bool reliable, bool sequenced);
    bool                 SendGameData(int id, const char *data, int length, bool reliable, bool sequenced);

    bool                 Update();

    bool                 IsActiveConnection() const;

    void                 AddMessageListener(MessageListener &listener);
    void                 ClearListeners() { mListeners.clear(); }

    void                 Close();

    bool                 SendTypedData(unsigned short type, const char *message, int length, bool reliable, bool sequenced);
    bool                 SendTypedData(int id, unsigned short type, const char *message, int length, bool reliable, bool sequenced);

    bool                 SendDataMessage(const char *message, int length, bool reliable, bool sequenced);
    bool                 SendDataMessage(int id, const char *message, int length, bool reliable, bool sequenced);
    bool                 SendLuaMessage(const char *message, int length, bool reliable = true, bool sequenced = true);
    bool                 SendLuaMessage(int id, const char *message, int length, bool reliable = true, bool sequenced = true);

    const std::string   &NextMessage(Queue queue) const;
    void                 RemoveMessage(Queue queue);
    bool                 HasMessage(Queue queue) const;
    void                 ClearMessages(Queue queue);
    bool                 ReceiveMessage(Queue queue, const char *message, int length);
    bool                 ReceiveMessage(const char *message, int length);

    void                 SetOnReceiveCharacterUpdates(OnReceiveCharacterUpdates callback, void *user);

    Connection::ConnectionManager *GetConnectionManager() { return mConnectionManager; }

private:

    enum ConnectionEvent { NONE, CLIENT_CONNECT, CLIENT_DISCONNECT, SERVER_CONNECT, SERVER_DISCONNECT };

    void OnConnectClient(Connection::IConnection *connection);
    void OnReceiveClient(int connectionid, const void *data, int length);
    void OnDisconnectClient(int connectionid, int reason);
    void OnConnectServer(Connection::IConnection *connection);
    void OnReceiveServer(int connectionid, const void *data, int length);
    void OnDisconnectServer(int connectionid, int reason);

    static void Multiplayer_OnConnectClient(Connection::IConnection *connection, void *user);
    static void Multiplayer_OnReceiveClient(int connectionid, const void *data, int length, void *user);
    static void Multiplayer_OnDisconnectClient(int connectionid, int reason, void *user);
    static void Multiplayer_OnConnectServer(Connection::IConnection *connection, void *user);
    static void Multiplayer_OnReceiveServer(int connectionid, const void *data, int length, void *user);
    static void Multiplayer_OnDisconnectServer(int connectionid, int reason, void *user);

    Connection::ConnectionManager *mConnectionManager;
    std::vector<MessageListener>   mListeners;
    int                            mPlayerId;                            
    bool                           mJoinComplete;
    ConnectionEvent                mConnectionEvent;
    int                            mConnectionEventConnectionId,
                                   mConnectionEventReason;
    std::deque<std::string>        mMessageQueues[MAX_QUEUES];
    OnReceiveCharacterUpdates      mOnReceiveCharacterUpdates;
    void                          *mOnReceiveCharacterUpdatesUser;
};

#endif // _MULTIPLAYER_H