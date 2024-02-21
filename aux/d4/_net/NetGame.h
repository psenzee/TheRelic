#ifndef _NETGAME_H
#define _NETGAME_H

#include "Address.h"

#include <string.h>

enum
{
    NETGAME_PORT      = 18888,
    TIMEOUT_VALUE     = 200000, // 2s timeout
    MAX_MESSAGE_SIZE  = 65536,
    NAME_SIZE         = 30,
    PING_MILLISECONDS = 1000 / 15 // 15fps
};

struct NetError
{
    enum Error
    {
        ERR_NONE = 0,
        ERR_CANT_SEND,
        ERR_UNKNOWN,
        ERR_UNRECOGNIZED,
        ERR_DENIED,
        ERR_SERVER_FULL,
        ERR_TIMEOUT_DROPPING,
        ERR_ALREADY_JOINED
    };
};

template <unsigned char SIZE>
struct NetString
{
    unsigned char size;
    char          string[SIZE];

    enum { MAX_SIZE = SIZE - 1 };

    inline NetString()              { Set(0); }
    inline NetString(const char *s) { Set(s); }

    inline void Set(const char *s)
    {
        size = 0;
        memset(string, 0, SIZE);
        if (s && *s)
        {
            size_t ssz = strlen(s);
            if (ssz > MAX_SIZE) // make sure it fits
                ssz = MAX_SIZE;
            strncpy(string, s, ssz);
            size = (unsigned char)ssz;
        }
    }
};

struct NetGameMessage
{
    enum Type { 
                NONE = 0, NAME_REQUEST, NAME_RESPONSE,

                SVR_ERROR,        CLI_ERROR,
                SVR_PING,         CLI_PING,
                CLI_JOIN_REQUEST, SVR_JOIN_REQUEST_ACK,
                SVR_JOIN_ACCEPT,  CLI_JOIN_ACCEPT_ACK,

                CLI_GAME_DATA,    SVR_GAME_DATA,

                /*, the rest.. */ };
    enum      { HAS_ID = 0x80 };

    char type;

    inline Type GetType() const { return (Type)(type & (unsigned char)(~HAS_ID)); }
    inline bool HasId()   const { return (type & HAS_ID) != 0; }
};

struct NetGameMessageId : public NetGameMessage
{
    char clientId;
};

template <unsigned char SIZE>
struct NetGameMessage_String : public NetGameMessage
{
    NetString<SIZE> string;
};

template <unsigned char SIZE>
struct NetGameMessageId_String : public NetGameMessageId
{
    NetString<SIZE> string;
};

struct NetGameMessage_Error : public NetGameMessage
{
    char error;
};

struct NetGameMessage_JoinAccept : public NetGameMessage
{
    char assignedId;
};

struct NetPlayerInfo
{
    enum State { NONE = 0, HOSTING, OPEN, JOINING, JOINED };

    Address               address;
    State                 state;
    int                   timestamp;
    int                   pingSent;
    NetString<NAME_SIZE>  name;

    inline NetPlayerInfo() : state(NONE), timestamp(0), pingSent(0) {}

    inline void Reset() { state = NONE; timestamp = 0; }

    void        ResetAll();
};

class INetGameParticipant
{
public:

    virtual ~INetGameParticipant() {}

    virtual bool IsServer()       const = 0;
    virtual void SetName(const char *s) = 0;
    virtual bool Process()              = 0;
    virtual int  GetId()          const = 0;   
    virtual int  GetTime()        const = 0;
};

#endif // _NETGAME_H