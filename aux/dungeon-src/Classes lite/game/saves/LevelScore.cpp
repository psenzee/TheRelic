/*
 *  LevelScore.cpp
 *  GLGravity
 *
 *  Created by Paul Senzee on 1/3/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "LevelScore.h"

#include <string.h>
#include <stdio.h>

void LevelScore::SetName(const char *s)
{
    memset(name, 0, sizeof(name));
    strncpy(name, s, sizeof(name) - 1);
}

int LevelScore::CompareScore(const LevelScore &u) const
{
    int c = score - u.score;
    if (c == 0) c = level - u.level;
    if (c == 0) c = int(u.timestamp) - int(timestamp); // earlier is better
    if (c == 0) c = strcmp(name, u.name);
    return c;
}

int LevelScore::CompareLevel(const LevelScore &u) const
{
    int c = level - u.level;
    if (c == 0) c = score - u.score;
    if (c == 0) c = int(u.timestamp) - int(timestamp); // earlier is better
    if (c == 0) c = strcmp(name, u.name);
    return c;
}

void LevelScore::GetDisplayName(char *display, bool username) const
{ 
    struct tm  *ts = 0;
    char        buf[80];
    time_t      tms = timestamp;
    ts = localtime(&tms);
    strftime(buf, sizeof(buf), "%m/%d/%y %H:%M:%S", ts);
    if (!username) sprintf(display, "L%u %s", level, buf);
    else           sprintf(display, "L%u %s [%s %u]", level, buf, name, score);
}