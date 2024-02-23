#ifndef _CODEC_H
#define _CODEC_H

#include <cstddef>
#include <sys/types.h>

class Codec
{
public:

    enum { COMPRESSED = 0xC2, UNCOMPRESSED = 0xC3 };

    enum { MAX_DECODE_SIZE = 65535 };

    static ssize_t Decode(const char *in, char *out);
    static ssize_t Encode(const char *in, size_t size, char *out);

private:

    static ssize_t DecodeCompressed(const char *in, char *out);
    static ssize_t DecodeUncompressed(const char *in, char *out);
    static ssize_t EncodeCompressed(const char *in, size_t size, char *out);
    static ssize_t EncodeUncompressed(const char *in, size_t size, char *out);

    static bool DoCompress(size_t size);
};

#endif // _CODEC_H
