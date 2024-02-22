/*************************************************************************************************/
/*!
    \file   csv.cpp

    $Header: //blaze/games/Madden/2010-NG/trunk/component/franchise/stress/csv.cpp#2 $
    $Change: 46641 $
    $DateTime: 2009/06/19 14:08:13 $

    \attention
        (c) Electronic Arts Inc. 2009
*/
/*************************************************************************************************/

#include "framework/blaze.h"

#include "csv.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

/*************************************************************************************************/
/*!
    Csv 

    This file provides a series of functions that load and process .csv files to support the
    StatsTables class (statstable.h/.cpp).  Additional, some simple text parsing functions 
    are exposed here.

*/
/*************************************************************************************************/

//! Advance the text pointer until the current character is not whitespace
static void SkipSpace(const char **text)
{
    while (isspace(**text)) (*text)++;
}

//! Read an integer value from text and advance the pointer, returning true on success
bool Read(const char **text, int &value)
{
    const int DECIMAL_RADIX = 10;
    while (isspace(**text)) (*text)++;
    const char *ends = *text;
    value = (int)strtol(*text, const_cast<char **>(&ends), DECIMAL_RADIX);
    if (ends && ends != *text)
    {
        *text = ends;
        return true;
    }
    return false;
}

//! Read a float value from text and advance the pointer, returning true on success
bool Read(const char **text, float &value)
{
    while (isspace(**text)) (*text)++;
    const char *ends = *text;
    value = (float)strtod(*text, const_cast<char **>(&ends));
    if (ends && ends != *text)
    {
        *text = ends;
        return true;
    }
    return false;
}

//! Read an unsigned value from text and advance the pointer, returning true on success
bool Read(const char **text, unsigned int &value)
{
    const int DECIMAL_RADIX = 10;
    while (isspace(**text)) (*text)++;
    const char *ends = *text;
    value = (unsigned int)strtoul(*text, const_cast<char **>(&ends), DECIMAL_RADIX);
    if (ends && ends != *text)
    {
        *text = ends;
        return true;
    }
    return false;
}

//! Read an boolean value from text and advance the pointer, returning true on success
bool Read(const char **text, bool &value)
{
    while (isspace(**text)) (*text)++;
    if      (strncmp(*text, "true", sizeof("true") - 1)  == 0)
    {
        value = true;
        return true;
    }
    else if (strncmp(*text, "false", sizeof("false") - 1) == 0)
    {
        value = false;
        return true;
    }
    int intValue = 0;
    if (!Read(text, intValue))
        return false;
    return (intValue != 0);
}

//! Safely return a C string from a vector of strings and an index
const char *GetAsString(int index, const eastl::vector<eastl::string> &values, const char *defaultValue)
{
    if (values.size() < (size_t)index)
        return values[index].c_str();
    return defaultValue;
}

//! Clear a vector quickly, don't resize back to empty
template <typename T>
static void FastClear(eastl::vector<T> &list, int reserve = 0)
{
    if (list.empty())
        return;
    if (reserve || (size_t)reserve > list.size())
        list.reserve(reserve);
    list.erase(list.begin(), list.end()); // don't clear() cause that invalidates reserve()..
}

//! Read a quoted text from the provided string
static bool ReadQuoted(const char **text, char quote, char *buffer,
                       int size, bool escape)
{
    const char *p = *text;
    SkipSpace(&p);
    if (*p != quote)
        return false;
    p++;
    char *pb = buffer;
    while (*p != '\0' && *p != quote)
    {
        if (escape && *p == '\\') // escape..
            p++;
        if ((pb - buffer) < (size - 1)) // if bigger than the buffer, don't add any more characters (truncate)
            *pb++ = *p;
        p++;
    }
    if (*p == '\0')
    {
        printf("Attempting to parse an unterminated quoted string!");
        return false;
    }
    p++;
    *pb = '\0';
    *text = p;
    return true;
}

//! Read from a string until a given character is encountered
static bool ReadUntil(const char **text, char until, char *buffer, int size)
{
    const char *p = *text;
    SkipSpace(&p);
    char *pb = buffer;
    while (*p != '\0' && *p != until)
    {
        if ((pb - buffer) < (size - 1)) // if bigger than the buffer, we're going to have to truncate
            *pb++ = *p;
        p++;
    }
    *pb = '\0';
    *text = p;
    return true;
}

//! Read a list of strings from a single .csv line
int ReadQuotedSeparatedValuesLine(const char *line, char separator, eastl::vector<eastl::string> &values)
{
    enum { MAX_BUFFER_SIZE = 64 * 1024 };

    FastClear(values);
    const char **p = &line;
    char buffer[MAX_BUFFER_SIZE];
    memset(buffer, 0, MAX_BUFFER_SIZE);
    int count = 0;
    for (count = 0; **p != '\0'; count++, (*p)++)
    {
        *buffer = '\0';
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
        if (**p == '\0')
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

//! Read a list of integers from a single .csv line
int ReadSeparatedIntsLine(const char *line, char separator, eastl::vector<int> &values)
{
    FastClear(values);
    const char **p = &line;
    int value = 0, count = 0;
    for (count = 0; **p != '\0'; count++, (*p)++)
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
        if (**p == '\0')
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

//! Read a list of string arrays from a .csv file
bool ReadSeparatedValuesFile(const char *filename, char separator, eastl::vector< eastl::vector<eastl::string> > &data, int maxrows)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
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
        while (*p != '\0' && isspace(*p)) // advance to first non-blank character
            p++;
        if (*p != '\0' && *p != '#' && // skip if empty or a comment (#)
            ReadQuotedSeparatedValuesLine(p, separator, values)) 
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

//! Read a list of integer arrays from a .csv file
bool ReadSeparatedIntsFile(const char *filename, char separator, eastl::vector< eastl::vector<int> > &data, int maxrows)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
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
        while (*p != '\0' && isspace(*p)) // advance to first non-blank character
            p++;
        if (*p != '\0' && *p != '#' && // skip if empty or a comment (#)
            ReadSeparatedIntsLine(p, separator, values)) 
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

//! Convert a vector of strings containing numbers into a vector of ints
void ReadInts(const eastl::vector<eastl::string> &in, eastl::vector<int> &out, int defaultValue)
{
    FastClear(out);
    for (eastl::vector<eastl::string>::const_iterator i = in.begin(), e = in.end(); i != e; ++i)
    {
        out.push_back(GetValue((*i).c_str(), defaultValue));
    }
}

//! Convert a list of string vectors containing numbers into a list of int vectors
void ReadInts(const eastl::vector< eastl::vector<eastl::string> > &in, eastl::vector< eastl::vector<int> > &out, int defaultValue)
{
    eastl::vector<int> ints;
    for (eastl::vector< eastl::vector<eastl::string> >::const_iterator i = in.begin(), e = in.end(); i != e; ++i)
    {
        ReadInts(*i, ints, defaultValue);
        out.push_back(ints);
    }
}

//! Prints an array of ints as a comma-delimited line
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

//! Prints an array of arrays of ints as a comma-delimited table
void PrintInts(const eastl::vector< eastl::vector<int> > &table)
{
    for (eastl::vector< eastl::vector<int> >::const_iterator i = table.begin(), e = table.end(); i != e; ++i)
    {
        PrintInts(*i);
    }
    printf("Contains ");
    PrintTableInfo(table);
}
