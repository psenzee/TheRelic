/*************************************************************************************************/
/*!
    \file   csv.h

    $Header: //blaze/games/Madden/2010-NG/trunk/component/franchise/stress/csv.h#2 $
    $Change: 44737 $
    $DateTime: 2009/06/04 14:33:39 $

    \attention
        (c) Electronic Arts Inc. 2009
*/
/*************************************************************************************************/

#ifndef _CSV_H
#define _CSV_H

#include <stdio.h>

int  ReadSeparatedValuesLine(const char *line,     char separator, eastl::vector<eastl::string> &values);
bool ReadSeparatedValuesFile(const char *filename, char separator, eastl::vector< eastl::vector<eastl::string> > &data, int maxrows = 0);

int  ReadSeparatedIntsLine(const char *line,     char separator, eastl::vector<int> &values);
bool ReadSeparatedIntsFile(const char *filename, char separator, eastl::vector< eastl::vector<int> > &data, int maxrows = 0);

bool Read(const char **s, int &u);
bool Read(const char **s, float &u);
bool Read(const char **s, unsigned int &u);
bool Read(const char **s, bool &u);

void ReadInts(const eastl::vector<eastl::string> &in, eastl::vector<int> &out, int defaultValue = -1);
void ReadInts(const eastl::vector< eastl::vector<eastl::string> > &in, eastl::vector< eastl::vector<int> > &out, int defaultValue = -1);

const char *GetAsString (int index, const eastl::vector<eastl::string> &values, const char *defaultValue);

template <typename T>
T GetValue(const char *s, T defaultValue)
{
    T value = defaultValue;
    if (!Read(&s, value))
        value = defaultValue;
    return value;
}

template <typename T>
T GetValue(int index, const eastl::vector<eastl::string> &values, T defaultValue)
{
    return GetValue(GetAsString(index, values, ""), defaultValue);
}

template <typename T>
void PrintTableInfo(const eastl::vector<T> &table)
{
    if (table.empty())
    {
        printf("0 rows.\n");
        return;
    }

    size_t minsize = INT_MAX, maxsize = 0;
    for (typename eastl::vector<T>::const_iterator i = table.begin(), e = table.end(); i != e; ++i)
    {
        size_t size = (*i).size();
        if (size < minsize)
            minsize = size;
        if (size > maxsize)
            maxsize = size;
    }

    if (minsize == maxsize)
    {
        printf("%d rows size %d.\n", table.size(), (int)minsize);
    }
    else
    {
        printf("%d rows sizes %d-%d.\n", table.size(), (int)minsize, (int)maxsize);
    }
}

void PrintInts(const eastl::vector<int> &array);
void PrintInts(const eastl::vector< eastl::vector<int> > &array);

#endif // _CSV_H