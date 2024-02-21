/*
 *  HighScores.h
 *  GLGravity
 *
 *  Created by Paul Senzee on 1/3/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _HIGHSCORES_H
#define _HIGHSCORES_H

#include "LevelScore.h"

class HighScores
{
public:
    
    HighScores();
    ~HighScores();
    
    bool              SetFile(const char *filename);    
    
    int               GetHighScoreIndex(const LevelScore &score)          const;
    void              SetScoreAtIndex(int index, const LevelScore &score);
    bool              IsLevelHighScore(const LevelScore &score)           const;
    bool              SetLevelScore(const LevelScore &score);
    
    void              Update();
    
    const LevelScore &GetScore(int index)      const { return scores[index]; }
    const LevelScore &GetLevelScore(int index) const { return levels[index]; }

    void              Print()                                             const;
    void              Clear();

private: // these are of limited utility with the way i can get names, so i'm privatizing them for now

    int               SetScore(const LevelScore &score);
    
    void              SetInitial(unsigned rank, unsigned level, unsigned score); // to populate the initial list
    
private:
    
    bool Read(const char *filename);    
    bool Write(const char *filename) const;    
    
    enum { MAX_HIGH_SCORES = 10, MAX_LEVEL_SCORES = 128 };    

    LevelScore scores[MAX_HIGH_SCORES];
    LevelScore levels[MAX_LEVEL_SCORES];
    
    enum { DATA_SIZE = sizeof(LevelScore) * MAX_HIGH_SCORES + sizeof(LevelScore) * MAX_LEVEL_SCORES };
    
    // don't write/read these fields
    const char    *filename;
    bool           dirty;
};

class HighScoreContext
{
public:

    HighScoreContext(HighScores *scores, const LevelScore &score);

    HighScores *scores;
    LevelScore  score;

    void UpdateScores();

private:

    bool IsNameRequired() const;
    void Update();

    static void SetNameAndUpdate(const char *text, void *context);
};

#endif // _HIGHSCORES_H
