/*************************************************************************************************/
/*!
    \file   csv.h

    $Header: //blaze/games/Madden/2010-NG/trunk/component/franchise/stress/csv.h#2 $
    $Change: 46641 $
    $DateTime: 2009/06/19 14:08:13 $

    \attention
        (c) Electronic Arts Inc. 2009
*/
/*************************************************************************************************/

#ifndef _CSV_H
#define _CSV_H

#include <stdio.h>

/*************************************************************************************************/
/*!
    Csv 

    This file provides a series of functions that load and process .csv files to support the
    StatsTables class (statstable.h/.cpp).  Additional, some simple text parsing functions 
    are exposed here.

*/
/*************************************************************************************************/

//! Read a list of strings from a single .csv line
int  ReadQuotedSeparatedValuesLine(const char *line, char separator, eastl::vector<eastl::string> &values);
//! Read a list of string arrays from a .csv file
bool ReadSeparatedValuesFile(const char *filename, char separator, eastl::vector< eastl::vector<eastl::string> > &data, int maxrows = 0);

//! Read a list of integers from a single .csv line
int  ReadSeparatedIntsLine(const char *line,     char separator, eastl::vector<int> &values);
//! Read a list of integer arrays from a .csv file
bool ReadSeparatedIntsFile(const char *filename, char separator, eastl::vector< eastl::vector<int> > &data, int maxrows = 0);

//! Read an integer value from text and advance the pointer, returning true on success
bool Read(const char **text, int &value);
//! Read a float value from text and advance the pointer, returning true on success
bool Read(const char **text, float &value);
//! Read an unsigned value from text and advance the pointer, returning true on success
bool Read(const char **text, unsigned int &value);
//! Read an boolean value from text and advance the pointer, returning true on success
bool Read(const char **text, bool &value);

//! Convert a vector of strings containing numbers into a vector of ints
void ReadInts(const eastl::vector<eastl::string> &in, eastl::vector<int> &out, int defaultValue = -1);
//! Convert a list of string vectors containing numbers into a list of int vectors
void ReadInts(const eastl::vector< eastl::vector<eastl::string> > &in, eastl::vector< eastl::vector<int> > &out, int defaultValue = -1);

//! Safely return a C string from a vector of strings and an index
const char *GetAsString (int index, const eastl::vector<eastl::string> &values, const char *defaultValue);

//! A function to convert a string into an int, unsigned int, float or bool
template <typename T>
T GetValue(const char *text, T defaultValue)
{
    T value = defaultValue;
    if (!Read(&text, value))
        value = defaultValue;
    return value;
}

//! A function to convert a value at a given index in a list of strings into an int, unsigned int, float or bool
template <typename T>
T GetValue(int index, const eastl::vector<eastl::string> &values, T defaultValue)
{
    return GetValue(GetAsString(index, values, ""), defaultValue);
}

//! PrintTableInfo will print out table rows and range of row sizes for a table of a given type
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

//! Prints an array of ints as a comma-delimited line
void PrintInts(const eastl::vector<int> &array);

//! Prints an array of arrays of ints as a comma-delimited table
void PrintInts(const eastl::vector< eastl::vector<int> > &array);

#endif // _CSV_H
