#include "Pack.h"
#include "core/Dictionary.h"

void PackString(char **to, const std::string &str, const Dictionary *dictionary)
{
    PackString(to, str.c_str(), static_cast<int>(str.size()), dictionary);
}

void PackString(char **to, const char *str, int strsize, const Dictionary *dictionary)
{
    if (dictionary)
    {
        int index = dictionary->GetId(str);
        if (index >= 0 && index < 32767)
        {
            PackUInt16be(to, (PACK_DICTIONARY_STRING << 8) | index);
            return;
        }
    }
    if (strsize > 65535)
    {
        PackUChar(to, PACK_L32_STRING);
        PackUInt(to, strsize);
    }
    else if (strsize > 255)
    {
        PackUChar(to, PACK_L16_STRING);
        PackUInt16(to, strsize);        
    }
    else
    {
        PackUChar(to, PACK_L8_STRING);
        PackUChar(to, strsize);
    }
    memcpy(*to, str, strsize);
    (*to) += strsize;
}

int GetUnpackStringSize(const char *from, const Dictionary *dictionary)
{
    char c = *from;
    if (!IsPackTypeString(c))
        return -1;
    if (c & PACK_DICTIONARY_STRING)
    {
        if (!dictionary)
        {
            // we actually have an error here..
            return -1;
        }
        int index = UnpackUInt16be(&from) & ~(PACK_DICTIONARY_STRING << 8);
        const char *text = dictionary->GetText(index);
        if (!text)
        {
            // we actually have an error here..
            return -1;
        }
        return static_cast<int>(strlen(dictionary->GetText(index)));
    }
    size_t size = 0;
    from++;
    switch (c)
    {
    case PACK_L32_STRING: size = UnpackUInt(&from);   break;
    case PACK_L16_STRING: size = UnpackUInt16(&from); break;
    case PACK_L8_STRING:  size = UnpackUChar(&from);  break;
    default: return -1;
    }
    return static_cast<int>(size);
}

int UnpackString(const char **from, char *str, int maxsize, const Dictionary *dictionary)
{
    char c = **from;
    if (!IsPackTypeString(c))
        return -1;
    if (c & PACK_DICTIONARY_STRING)
    {
        if (!dictionary)
        {
            // we actually have an error here..
            return -1;
        }
        int index = UnpackUInt16be(from) & ~(PACK_DICTIONARY_STRING << 8);
        const char *text = dictionary->GetText(index);
        if (!text)
        {
            // we actually have an error here..
            return -1;
        }
        strncpy(str, text, maxsize);
        str[maxsize] = '\0';
        return static_cast<int>(strlen(str));
    }
    int size = 0;
    switch (c)
    {
    case PACK_L32_STRING: (*from)++; size = UnpackUInt(from);   break;
    case PACK_L16_STRING: (*from)++; size = UnpackUInt16(from); break;
    case PACK_L8_STRING:  (*from)++; size = UnpackUChar(from);  break;
    default: return -1;
    }
    int copysize = maxsize < size ? maxsize : size;
    memcpy(str, *from, copysize);
    str[size] = '\0';
    (*from) += size;
    return copysize;
}

int UnpackString(const char **from, std::string &str, const Dictionary *dictionary)
{
    int size = GetUnpackStringSize(*from, dictionary);
    if (size < 0)
        return -1;
    str.resize(size, '\0');
    size = UnpackString(from, const_cast<char *>(str.data()), size, dictionary); // really evil hack, but safe and fast
    if (size < 0)
        return -1;
    return size;
}

void PackNumber(char **to, float number)
{
    if (floor(number) == number)
    {
        // then let's write this as an int
        int n = static_cast<int>(number);
        if (n == 0)
        {
            PackChar(to, PACK_ZERO);
        }
        else if (n > 0) // unsigned
        {
            if (n > 65535)
            {
                PackChar(to, PACK_UINT32);
                PackUInt(to, n);
            }
            else if (n > 255)
            {
                PackChar(to, PACK_UINT16);
                PackUInt16(to, n);
            }
            else
            {
                PackChar(to, PACK_UINT8);
                PackUChar(to, n);
            }
        }
        else // n < 0, signed
        {
            if (n < -32768)
            {
                PackChar(to, PACK_INT32);
                PackInt(to, n);
            }
            else if (n < -128)
            {
                PackChar(to, PACK_INT16);
                PackInt16(to, n);
            }
            else
            {
                PackChar(to, PACK_INT8);
                PackChar(to, n);
            }
        }
    }
    else
    {
        PackChar(to, PACK_FLOAT);
        Pack(to, number);
    }
}

bool UnpackNumber(const char **from, float *number)
{
    char c = **from;
    switch (c)
    {
    case PACK_ZERO:   (*from)++; *number = 0.0f; break;
    case PACK_UINT8:  (*from)++; *number = static_cast<float>(UnpackUChar(from));  break;
    case PACK_INT8:   (*from)++; *number = static_cast<float>(UnpackChar(from));   break;
    case PACK_UINT16: (*from)++; *number = static_cast<float>(UnpackUInt16(from)); break;
    case PACK_INT16:  (*from)++; *number = static_cast<float>(UnpackInt16(from));  break;
    case PACK_UINT32: (*from)++; *number = static_cast<float>(UnpackUInt(from));   break;
    case PACK_INT32:  (*from)++; *number = static_cast<float>(UnpackInt(from));    break;
    case PACK_FLOAT:  (*from)++; *number = static_cast<float>(UnpackFloat(from));  break;
    default: return false;
    }
    return true;
}