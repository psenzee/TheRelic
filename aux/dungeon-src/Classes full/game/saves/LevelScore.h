/*
 *  LevelScore.h
 *  GLGravity
 *
 *  Created by Paul Senzee on 1/3/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _LEVELSCORE_H
#define _LEVELSCORE_H

#include <time.h>

enum { VERSION = 0x3300 };

class LevelScore
{
public:
    
    char           name[32];
    time_t         timestamp;
    unsigned short level, version;
    unsigned       score;
    
    inline LevelScore(const char *name, time_t timestamp, unsigned short level, unsigned score) : timestamp(timestamp), level(level), score(score), version(VERSION) { SetName(name); }
    inline LevelScore() : timestamp(0), level(1), score(0), version(VERSION) { SetName("<no name>"); }
    inline LevelScore(const LevelScore &u) : timestamp(u.timestamp), level(u.level), score(u.score), version(VERSION) { SetName(u.name); }

    void SetName(const char *name);

    int CompareScore(const LevelScore &u) const;
    int CompareLevel(const LevelScore &u) const;

    void GetDisplayName(char *display, bool username = false) const;
};

#endif // _LEVELSCORE_H
