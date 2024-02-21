/*************************************************************************************************/
/*!
    \file   csv.cpp

    $Header: //blaze/games/Madden/2010-NG/trunk/component/franchise/stress/csv.cpp#2 $
    $Change: 44766 $
    $DateTime: 2009/06/04 15:07:27 $

    \attention
        (c) Electronic Arts Inc. 2009
*/
/*************************************************************************************************/

#include "framework/blaze.h"

#include "csv.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

static void SkipSpace(const char **s)
{
    while (isspace(**s)) (*s)++;
}

bool Read(const char **s, int &u)
{
    while (isspace(**s)) (*s)++;
    const char *ends = *s;
    u = (int)strtol(*s, const_cast<char **>(&ends), 10);
    if (ends && ends != *s)
    {
        *s = ends;
        return true;
    }
    return false;
}

bool Read(const char **s, float &u)
{
    while (isspace(**s)) (*s)++;
    const char *ends = *s;
    u = (float)strtod(*s, const_cast<char **>(&ends));
    if (ends && ends != *s)
    {
        *s = ends;
        return true;
    }
    return false;
}

bool Read(const char **s, unsigned int &u)
{
    while (isspace(**s)) (*s)++;
    const char *ends = *s;
    u = (unsigned int)strtoul(*s, const_cast<char **>(&ends), 10);
    if (ends && ends != *s)
    {
        *s = ends;
        return true;
    }
    return false;
}

bool Read(const char **s, bool &u)
{
    while (isspace(**s)) (*s)++;
    if      (strncmp(*s, "true", sizeof("true") - 1)  == 0)
    {
        u = true;
        return true;
    }
    else if (strncmp(*s, "false", sizeof("false") - 1) == 0)
    {
        u = false;
        return true;
    }
    int value = 0;
    if (!Read(s, value))
        return false;
    return (value != 0);
}

const char *GetAsString(int index, const eastl::vector<eastl::string> &values, const char *defaultValue)
{
    if (values.size() < (size_t)index)
        return values[index].c_str();
    return defaultValue;
}
/*
static void Trim(eastl::string &data)
{
    char *s = new char [data.size() + 1];
    memcpy(s, data.c_str(), data.size() + 1);
    char *p = s, *e = 0;
    while (*p && isspace(*p)) p++;
    e = p + strlen(p) - 1;
    while (e >= p && isspace(*e)) e--;
    e++; *e = 0;
    data = p;
    delete [] s;
}

static void DestroyStringList(char **strlist)
{
    for (int i = 0; strlist[i]; i++)
        delete [] strlist[i];
    delete [] strlist;
}

static unsigned CountChars(const char *p, char c)
{
    unsigned count = 0;
    while (*p)
    {
        count += (c == *p) ? 1 : 0;
        p++;
    }
    return count;
}

static unsigned CountCharsOf(const char *p, const char *of)
{
    if (!*(of + 1))
        return CountChars(p, *of);
    unsigned count = 0;
    while (*p)
    {
        count += strchr(of, *p) != 0 ? 1 : 0;
        p++;
    }
    return count;
}

static char **SplitCharsOf(const char *in, const char *of)
{
    char **splist = 0, *spstr = 0;
    int lin = (int)strlen(in), nspl = 0,
    j = 0, size = CountCharsOf(in, of) + 2;
    bool last = false, found = false, dchar = false;
    
    spstr  = new char [lin + 1];
    splist = new char * [size];
    memset(splist, 0, sizeof(char *) * size);
    memset(spstr, 0, lin + 1);
    
    for (int i = 0; i < lin; i++)
    {
        if (strchr(of, in[i]))
        {
            j = 0;
            splist[nspl] = new char [strlen(spstr) + 1];
            strcpy(splist[nspl], spstr);
            memset(spstr, 0, lin + 1);
            nspl++;
            last = true;
        }
        else
        {
            spstr[j++] = in[i];
            spstr[j] = 0;
            last = false;
        }
    }
    if (j)
    {
        splist[nspl] = new char [strlen(spstr) + 1];
        strcpy(splist[nspl], spstr);
        nspl++;
    }
    
    delete [] spstr;
    return splist;
}
*/

template <typename T>
static void FastClear(eastl::vector<T> &list, int reserve = 0)
{
    if (list.empty())
        return;
    if (reserve || (size_t)reserve > list.size())
        list.reserve(reserve);
    list.erase(list.begin(), list.end()); // don't clear() cause that invalidates reserve()..
}

static bool ReadQuoted(const char **s, char quote, char *buffer,
                       int size, bool escape)
{
    const char *p = *s;
    SkipSpace(&p);
    if (*p != quote)
        return false;
    p++;
    char *pb = buffer;
    while (*p && *p != quote)
    {
        if (escape && *p == '\\') // escape..
            p++;
        if ((pb - buffer) < (size - 1)) // if bigger than the buffer, don't add any more characters (truncate)
            *pb++ = *p;
        p++;
    }
    if (!*p)
    {
        printf("Attempting to parse an unterminated quoted string!");
        return false;
    }
    p++;
    *pb = 0;
    *s = p;
    return true;
}

static bool ReadUntil(const char **s, char until, char *buffer, int size)
{
    const char *p = *s;
    SkipSpace(&p);
    char *pb = buffer;
    while (*p && *p != until)
    {
        if ((pb - buffer) < (size - 1)) // if bigger than the buffer, we're going to have to truncate
            *pb++ = *p;
        p++;
    }
    *pb = 0;
    *s = p;
    return true;
}
/*
int ReadSeparatedValuesLine(const char *line, char separator, eastl::vector<eastl::string> &values)
{
    char sep[] = { separator, '\0' };
    char **items = SplitCharsOf(line, sep);
    if (!items)
        return 0;
    if (!**items)
    {
        DestroyStringList(items);
        return 0;
    }
    FastClear(values);
    char **p = items;
    eastl::string value;
    for (int count = 0; *p; count++, p++)
    {
        value = *p;
        Trim(value);
        values.push_back(value);
    }
    DestroyStringList(items);
    return count;
}
*/

int ReadQuotedSeparatedValuesLine(const char *line, char separator, eastl::vector<eastl::string> &values)
{
    enum { MAX_BUFFER_SIZE = 64 * 1024 };

    FastClear(values);
    const char **p = &line;
    char buffer[MAX_BUFFER_SIZE];
    memset(buffer, 0, MAX_BUFFER_SIZE);
    int count = 0;
    for (count = 0; **p; count++, (*p)++)
    {
        *buffer = 0;
        if (!ReadQuoted(p, '"',       buffer, MAX_BUFFER_SIZE, false) &&
            !ReadQuoted(p, '\'',      buffer, MAX_BUFFER_SIZE, false) &&
            !ReadUntil (p, separator, buffer, MAX_BUFFER_SIZE))
        {
            // error!
            printf("Error at row %d: Unable to parse value!\n", count);
            break;
        }
        SkipSpace(p);
        values.push_back(eastl::string(buffer));
        if (!**p)
            break; // done
        if (**p != separator)
        {
            // error!
            printf("Error at row %d: expected '%c'!\n", count, separator);
            break;
        }
    }
    return count;
}

int ReadSeparatedIntsLine(const char *line, char separator, eastl::vector<int> &values)
{
    FastClear(values);
    const char **p = &line;
    int value = 0, count = 0;
    for (count = 0; **p; count++, (*p)++)
    {
        value = 0;
        if (!Read(p, value))
        {
            // error!
            printf("Error at row %d: value is not an integer!\n", count);
            break;
        }
        SkipSpace(p);
        values.push_back(value);
        if (!**p)
            break; // done
        if (**p != separator)
        {
            // error!
            printf("Error at row %d: expected '%c'!\n", count, separator);
            break;
        }
    }
    return count;
}

bool ReadSeparatedValuesFile(const char *filename, char separator, eastl::vector< eastl::vector<eastl::string> > &data, int maxrows)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        printf("Error: Cannot open separated values file '%s'!\n", filename);
        return false;
    }
    enum { LINE_SIZE = 1024 * 1024 };
    char *line = new char [LINE_SIZE];
    eastl::vector<eastl::string> values;
    int count = 0;
    while (fgets(line, (int)(LINE_SIZE - 1), file))
    {
        const char *p = line;
        while (*p && isspace(*p)) // advance to first non-blank character
            p++;
        //if (*p && *p != '#' && ReadSeparatedValuesLine(p, separator, values)) // skip if empty or a comment (#)
          if (*p && *p != '#' && ReadQuotedSeparatedValuesLine(p, separator, values)) // skip if empty or a comment (#)
        {
            data.push_back(values);
            count++;
            if (count % 10000 == 0)
                printf(".");
            if (maxrows > 0 && count >= maxrows)
                break;
        }
    }
    fclose(file);
    delete [] line;
    return true;
}

bool ReadSeparatedIntsFile(const char *filename, char separator, eastl::vector< eastl::vector<int> > &data, int maxrows)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        printf("Error: Cannot open separated values file '%s'!\n", filename);
        return false;
    }
    enum { LINE_SIZE = 1024 * 1024 };
    char *line = new char [LINE_SIZE];
    eastl::vector<int> values;
    int count = 0;
    while (fgets(line, (int)(LINE_SIZE - 1), file))
    {
        const char *p = line;
        while (*p && isspace(*p)) // advance to first non-blank character
            p++;
        if (*p && *p != '#' && ReadSeparatedIntsLine(p, separator, values)) // skip if empty or a comment (#)
        {
            data.push_back(values);
            count++;
            if (count % 10000 == 0)
                printf(".");
            if (maxrows > 0 && count >= maxrows)
                break;
        }
    }
    fclose(file);
    delete [] line;
    return true;
}

void ReadInts(const eastl::vector<eastl::string> &in, eastl::vector<int> &out, int defaultValue)
{
    FastClear(out);
    for (eastl::vector<eastl::string>::const_iterator i = in.begin(), e = in.end(); i != e; ++i)
    {
        out.push_back(GetValue((*i).c_str(), defaultValue));
    }
}

void ReadInts(const eastl::vector< eastl::vector<eastl::string> > &in, eastl::vector< eastl::vector<int> > &out, int defaultValue)
{
    eastl::vector<int> ints;
    for (eastl::vector< eastl::vector<eastl::string> >::const_iterator i = in.begin(), e = in.end(); i != e; ++i)
    {
        ReadInts(*i, ints, defaultValue);
        out.push_back(ints);
    }
}

void PrintInts(const eastl::vector<int> &array)
{
    for (eastl::vector<int>::const_iterator i = array.begin(), e = array.end(); i != e; ++i)
    {
        printf("%d", *i);
        if (i + 1 != e)
            printf(", ");
    }
    printf("\n");
}

void PrintInts(const eastl::vector< eastl::vector<int> > &table)
{
    for (eastl::vector< eastl::vector<int> >::const_iterator i = table.begin(), e = table.end(); i != e; ++i)
    {
        PrintInts(*i);
    }
    printf("Contains ");
    PrintTableInfo(table);
}
