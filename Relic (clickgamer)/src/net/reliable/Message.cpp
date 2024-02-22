#include "Message.h"
#include "core/core_assert.h"
#include <stdlib.h> // for rand()

Header GetHeader(int messageid, int time, bool guaranteed, bool split, int chunk, int totalChunks)
{
    Header h;
    h.version     = Header::MESSAGE_VERSION;
    h.messageid   = static_cast<unsigned short>(messageid);
    h.time        = static_cast<unsigned short>(time);
    h.guaranteed  = guaranteed ? Header::MESSAGE_GUARANTEED : Header::MESSAGE_NOT_GUARANTEED;
    h.split       = split      ? Header::MESSAGE_SPLIT      : Header::MESSAGE_NOT_SPLIT;
    h.chunk       = chunk;
    h.totalChunks = static_cast<unsigned short>(totalChunks);
    return h;
}

bool IsValid(const Header &h)
{
    return  h.version    == Header::MESSAGE_VERSION &&
           (h.guaranteed == Header::MESSAGE_GUARANTEED || h.guaranteed == Header::MESSAGE_NOT_GUARANTEED) && 
           (h.split      == Header::MESSAGE_SPLIT      || h.split      == Header::MESSAGE_NOT_SPLIT);
}

bool IsSplit(const Header &h)
{
    core_assert(h.split == Header::MESSAGE_SPLIT || h.split == Header::MESSAGE_NOT_SPLIT);
    return h.split == Header::MESSAGE_SPLIT;
}

bool IsGuaranteed(const Header &h)
{
    core_assert(h.guaranteed == Header::MESSAGE_GUARANTEED || h.guaranteed == Header::MESSAGE_NOT_GUARANTEED);
    return h.guaranteed == Header::MESSAGE_GUARANTEED;
}

bool IsAcknowledge(const char *data, int length)
{
    return *(unsigned *)(data + sizeof(Header)) == ACKNOWLEDGE && length == ACKNOWLEDGE_SIZE;
}

int GetMessageId(const char *data)
{
    return ((Header *)data)->messageid;
}

int64_t GetMUID(const char *data)
{
    int64_t id = ((Header *)data)->messageid;
    return (id << 32) | ((int)((Header *)data)->chunk << 16) | (int)((Header *)data)->totalChunks;
}

void Message::Destroy()
{
    if (data)
        delete [] data;
    data = 0;
    length = 0;
}

void Message::Copy(const char *msgdata, int msglength)
{
    Destroy();
    data = new char [msglength];
    memcpy(data, msgdata, msglength);
    length = msglength;
}

Message CreateRandomTestMessage(int size, const Header &header)
{
    Message m(new char [size + sizeof(Header)], size + sizeof(Header));
    Header h = header;
    h.split = Header::MESSAGE_NOT_SPLIT;
    h.chunk = 0;
    h.totalChunks = 1;
    *(Header *)m.data = h;
    for (int i = 0; i < size; i++)
        m.data[sizeof(Header) + i] = (char)(rand() & 0xff);
    return m;
}

Message CreateRandomTestMessage(int size)
{
    Header h = GetHeader(GetTimeStamp(), 0);
    return CreateRandomTestMessage(size, h);
}

unsigned GetTimeStamp()
{
    static unsigned time = 0; // just a counting int - no magic
    return time++;
}

#ifdef WIN32
// for GetCurrentTimeMs()
#include <windows.h>
#include <mmsystem.h>

unsigned GetNetTimeMs()
{
    return ::timeGetTime();
}

#endif // WIN32