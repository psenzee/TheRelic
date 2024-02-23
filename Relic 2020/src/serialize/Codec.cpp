#include "Codec.h"
#include "serialize/Pack.h"

#include <zlib/zlib.h>
#include <string.h>

static void ReportAverageCompression(size_t uncompressed, size_t compressed)
{
    static size_t in = 0, out = 0, count = 0;
    in  += uncompressed;
    out += compressed;
    count++;
    if (count % 100 == 0 && count != 0)
    {
        printf("Compression: in=%lub, out=%lub\n", in / count, out / count);
        in = out = count = 0;
    }
}

bool Codec::DoCompress(size_t size)
{
    return size > 150; // figure out what the best value is for this
}

ssize_t Codec::EncodeUncompressed(const char *uncompressed, size_t size, char *compressed)
{
    PackChar(&compressed, (char)UNCOMPRESSED);
    PackUInt16(&compressed, size);
    memcpy(compressed, uncompressed, size);
    return size + sizeof(unsigned short) + sizeof(char);
}

ssize_t Codec::EncodeCompressed(const char *uncompressed, size_t size, char *compressed)
{
    char *buffer = compressed + sizeof(unsigned short) + sizeof(char);
    //Z_NO_COMPRESSION - data is not compressed.
    //Z_BEST_SPEED - sacrifices compression ratio for improved speed.
    //Z_BEST_COMPRESSION - gain improved compression ratios but at a cost of execution speed.
    //Z_DEFAULT_COMPRESSION - this is a compromise between compression ratios and speed of execution. 
    bool sendCompressed = false;
    unsigned long compressedSize = size;
    int error = Z_OK;
    // int ZEXPORT compress2(Bytef *dest, uLongf *destLen, const Bytef *source, uLong SourceLen, int level);
    if ((error = compress2(reinterpret_cast<unsigned char *>(buffer), &compressedSize,
                           reinterpret_cast<const unsigned char *>(uncompressed), size, Z_BEST_SPEED)) == Z_OK)
    {
        sendCompressed = true;
        if (static_cast<int>(compressedSize) >= size) // if it's not smaller, send uncompressed
            sendCompressed = false;
    }
    if (error != Z_OK)
        sendCompressed = false;
    if (!sendCompressed)
        return -1;
    PackChar(&compressed, (char)COMPRESSED);
    PackUInt16(&compressed, compressedSize);
    ReportAverageCompression(size, compressedSize);
    return ssize_t(compressedSize + sizeof(unsigned short) + sizeof(char));
}

ssize_t Codec::Encode(const char *uncompressed, size_t size, char *compressed)
{
    if (size > 32 * 1024)
        printf("Encoding large message of %lu bytes ..\n", size);
    if (!DoCompress(size))
        return EncodeUncompressed(uncompressed, size, compressed);
    ssize_t compressedSize = EncodeCompressed(uncompressed, size, compressed);
    if (compressedSize == -1)
        return EncodeUncompressed(uncompressed, size, compressed);
    if (size > 32 * 1024)
        printf("  .. compressed to %ld bytes\n", compressedSize);
    return compressedSize;
}

ssize_t Codec::DecodeUncompressed(const char *compressed, char *uncompressed)
{
    char c = *compressed;
    if (static_cast<unsigned char>(c) != UNCOMPRESSED)
        return -1;
    UnpackChar(&compressed);
    ssize_t size = UnpackUInt16(&compressed);
    memcpy(uncompressed, compressed, size);
    return size;
}

ssize_t Codec::DecodeCompressed(const char *compressed, char *uncompressed)
{
    char c = *compressed;
    if (static_cast<unsigned char>(c) != COMPRESSED)
        return -1;
    UnpackChar(&compressed);
    int size = UnpackUInt16(&compressed);
    uLongf uncompressedSize = MAX_DECODE_SIZE;
    //int ZEXPORT uncompress(Bytef *dest, uLongf *destLen, const Bytef *source, uLongf sourceLen);
    int error = uncompress(reinterpret_cast<unsigned char *>(uncompressed), &uncompressedSize,
                           reinterpret_cast<const unsigned char *>(compressed), uLong(size));
    if (error != Z_OK)
    {
        printf("uncompress error!\n");
        return -1;
    }
    return ssize_t(uncompressedSize);
}

ssize_t Codec::Decode(const char *compressed, char *uncompressed)
{
    switch (static_cast<unsigned char>(*compressed))
    {
    case COMPRESSED:   return DecodeCompressed  (compressed, uncompressed);
    case UNCOMPRESSED: return DecodeUncompressed(compressed, uncompressed);
    default: break;
    }
    printf("Unknown block type '\\x%02x'!\n", *compressed);
    return -1;
}
