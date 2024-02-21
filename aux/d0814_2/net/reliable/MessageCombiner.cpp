#include "core/core_assert.h"
#include <string.h>

#include "Message.h"
#include "MessageCombiner.h"

bool SplitMessage(const char *data, int length, SendFunction send, void *user)
{
    Header header = *(Header *)data;
    int count = ((length - sizeof(Header)) + MAX_MESSAGE_SIZE - 1) / MAX_MESSAGE_SIZE;
    if (count == 1)
        return send(user, data, length);
    char buffer[MAX_MESSAGE_SIZE + sizeof(Header)];
    const char *p = data + sizeof(Header);
    int remaining = length - sizeof(Header);
    header.split = Header::MESSAGE_SPLIT;
    header.totalChunks = count;
    for (int i = 0; i < count; i++)
    {
        header.chunk = i;
        *(Header *)buffer = header;
        int size = remaining < MAX_MESSAGE_SIZE ? remaining : MAX_MESSAGE_SIZE;
        memcpy(buffer + sizeof(Header), p, size);
        remaining = remaining - MAX_MESSAGE_SIZE;
        if (!send(user, buffer, size + sizeof(Header)))
            return false;
        p += size;
    }
    return true;
}

void MessageCombiner::InitializeChunks(int count)
{
    if (mChunks)
        delete [] mChunks;
    mChunksCount = count;
    mChunks = new Message [mChunksCount];
    memset(mChunks, 0, sizeof(Message) * mChunksCount);
}

void MessageCombiner::DestroyChunks()
{
    if (mChunks)
    {
        for (int i = 0; i < mChunksCount; i++)
            if (mChunks[i].data)
                delete [] mChunks[i].data;
        delete [] mChunks;
        mChunks = 0;
    }
}

bool MessageCombiner::AddChunk(int i, const char *data, int length)
{
    if (mChunks[i].data)
    {
        // this can be legit with guaranteed messages
        if (length == mChunks[i].length &&
            memcmp(data, mChunks[i].data, length) == 0)
            return true;
        core_assert(false && "data has already been specified for this chunk!!"); 
        return false;
    }
    mChunks[i].Copy(data, length);
    mLength += length;
    mChunksAcceptedCount++;
    if (mMessageComplete && mChunksAcceptedCount == mChunksCount)
        mMessageComplete(mMessageCompleteUser, mLength);
    return true;
}

// this takes the chunk number in the header, and checks that the total chunk count equals this chunk count
bool MessageCombiner::Accept(const char *data, int length)
{
    Header &header = *(Header *)data;
    if (!IsSplit(header))
        return false;
    if (mChunksCount == -1) // uninitialized
    {
        mHeader = header;
        InitializeChunks(header.totalChunks);
    }
    if (header.totalChunks != mChunksCount)
    {
        core_assert(false && "total chunk count is not equal to the number for this combiner, different message");
        return false;
    }
    return AddChunk((int)header.chunk, data + sizeof(Header), length - sizeof(Header));
}

int MessageCombiner::GetMessage(char *data)
{
    if (!IsComplete())
        return -1;
    int size = sizeof(Header);
    mHeader.chunk = 0;
    mHeader.totalChunks = 1;
    mHeader.split = Header::MESSAGE_NOT_SPLIT;
    memcpy(data, &mHeader, sizeof(Header));
    for (int i = 0; i < mChunksCount; i++)
    {
        int length = mChunks[i].length;
        memcpy(data + size, mChunks[i].data, length);
        size += length;
    }
    return size;
}
