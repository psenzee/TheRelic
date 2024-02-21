#include <string.h>
#include <stdio.h>
#include "core/core_assert.h"

#include "MessageReceiver.h"

#include "Message.h"
#include "MessageCombiner.h"

MessageReceiver::MessageReceiver(SendFunctionContext send, ReceiveFunctionContext receive) : mSend(send), mReceive(receive)
{
}

MessageReceiver::~MessageReceiver()
{
    if (!mReceivers.empty())
    {
        for (std::map<int, MessageCombiner *>::iterator i = mReceivers.begin(), e = mReceivers.end(); i != e; ++i)
            delete (*i).second;
        mReceivers.clear();
    }
    if (!mAvailable.empty())
    {
        for (std::deque< std::pair<int, MessageCombiner *> >::iterator i = mAvailable.begin(), e = mAvailable.end(); i != e; ++i)
            delete (*i).second;
        mAvailable.clear();
    }
}

int MessageReceiver::Receive(char *data, int length)
{
    int size = RECEIVE_ERROR; // or zero?
    while (true)
    {
        if (!mAvailable.empty())
        {   
            size = ReceiveAvailable(data, length);
            break;
        }
        size = mReceive.function(mReceive.user, data, length);
        core_assert(size <= 0 || IsValid(*(Header *)data));
        if (size <= 0 && mAvailable.empty())
            return RECEIVE_ERROR;
        if (size >= sizeof(Header))
        {
            if (IsGuaranteed(*(Header *)data))
                Acknowledge(data);
            if (!IsSplit(*(Header *)data))
                break;
            SendToCombiner(data, size);
        }
    }
    //if (size >= sizeof(Header) && IsGuaranteed(*(Header *)data))
    //    Acknowledge(data);
    return size;
}

bool MessageReceiver::Acknowledge(const char *data)
{
    unsigned ack = ACKNOWLEDGE;
    char buffer[sizeof(Header) + sizeof(ack)];
    *((Header *)buffer) = GetHeader(((Header *)data)->messageid, GetTimeStamp());
    ((Header *)buffer)->chunk = ((Header *)data)->chunk;
    ((Header *)buffer)->totalChunks = ((Header *)data)->totalChunks;
    memcpy(buffer + sizeof(Header), &ack, sizeof(ack));
    return mSend.function(mSend.user, buffer, sizeof(Header) + sizeof(ack));
}

int MessageReceiver::ReceiveAvailable(char *data, int length)
{
    if (mAvailable.empty())
        return -1;
    MessageCombiner *mc = mAvailable.front().second;
    if (length < mc->GetSize())
        return PROVIDED_SIZE_TOO_SMALL;
    mAvailable.pop_front();
    int size = mc->GetMessage(data);
    delete mc;        
    return size;
}

void MessageReceiver::SendToCombiner(const char *data, int length)
{
    Header &header = *((Header *)data);
    int mid = header.messageid;
    MessageCombiner *mc = 0;
    std::map<int, MessageCombiner *>::iterator i = mReceivers.find(mid);
    mc = (*i).second;
    if (i == mReceivers.end())
        mReceivers[mid] = mc = new MessageCombiner;
    if (!mc->Accept(data, length))
    {
        // is this a duplicate message?
        printf("D");
    }
    if (mc->IsComplete())
    {
        mAvailable.push_back(*i);
        mReceivers.erase(i);
    }
}