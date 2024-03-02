#ifndef _MESSAGE_COMBINER_345345_H
#define _MESSAGE_COMBINER_345345_H

#include "Message.h"

bool SplitMessage(const char *data, int length, SendFunction send, void *user);

class MessageCombiner
{
public:

    inline MessageCombiner() : mChunks(0), mChunksCount(-1), mChunksAcceptedCount(0), mMessageComplete(0), mMessageCompleteUser(0), mLength(sizeof(Header)) {}
    inline MessageCombiner(MessageCompleteFunction function, void *user) : mChunks(0), mChunksCount(-1), mChunksAcceptedCount(0), mMessageComplete(function), mMessageCompleteUser(user), mLength(sizeof(Header)) {}
    inline ~MessageCombiner() { DestroyChunks(); }

    bool Accept(const char *data, int length); // this takes the chunk number in the header, and checks that the total chunk count equals this chunk count

    bool IsComplete() const { return mChunksCount == mChunksAcceptedCount; }
    int  GetSize()    const { return mLength; }

    int  GetMessage(char *data);

private:

    int                      mChunksCount;
    int                      mChunksAcceptedCount;
    int                      mLength;
    Message                 *mChunks;
    MessageCompleteFunction  mMessageComplete;
    void                    *mMessageCompleteUser;
    Header                   mHeader;

    bool AddChunk(int i, const char *data, int length);
    void InitializeChunks(int count);
    void DestroyChunks();
};

#endif // _MESSAGE_COMBINER_345345_H