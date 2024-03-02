#include "MessageSender.h"
#include "SendHandler.h"
#include "Message.h"

#include <vector>

MessageSender::MessageSender(SendFunctionContext send) : mSend(send)
{
}

MessageSender::~MessageSender()
{
    if (!mSenders.empty())
    {
        printf("\nSenders outstanding %d - ", mSenders.size());
        for (map_t::iterator i = mSenders.begin(), e = mSenders.end(); i != e; ++i)
        {
            printf("%p(%d/%d) ", (int)((*i).first >> 32), ((int)((*i).first >> 16) & 0x0ffff) + 1, (int)((*i).first) & 0x0ffff);
            delete (*i).second;
        }
        printf("\n");
        mSenders.clear();
    }
}

bool MessageSender::Send(const char *data, int length)
{
    Header &header = *(Header *)data;
    if (!IsGuaranteed(header))
        return mSend.function(mSend.user, data, length);
    else
    {
        int64_t muid = GetMUID(data);
        map_t::iterator i = mSenders.find(muid);
        if (i == mSenders.end())
        {
            SendHandler *sh = new SendHandler(mSend);
            sh->Send(data, length);
            mSenders[muid] = sh;
            return true;
        }
    }
    return false;
}

void MessageSender::Acknowledge(int64_t muid)
{
    map_t::iterator i = mSenders.find(muid);
    if (i != mSenders.end())
        (*i).second->Acknowledge();
}

void MessageSender::Abandon(int64_t muid)
{
    map_t::iterator i = mSenders.find(muid);
    if (i != mSenders.end())
        (*i).second->Abandon();
}

void MessageSender::Update()
{
    for (map_t::iterator i = mSenders.begin(), e = mSenders.end(); i != e;)
    {
        (*i).second->Update();
        if ((*i).second->IsDone())
        {
            delete (*i).second;
            mSenders.erase(i++);
        }
        else ++i;
    }
}