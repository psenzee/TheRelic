/*
 *  HighScores.cpp
 *  GLGravity
 *
 *  Created by Paul Senzee on 1/3/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "HighScores.h"

#include <stdio.h>
#include <string.h>

typedef void (*text_callback)(const char *text, void *context);
//extern "C" void GetTextFromKeyboard(const char *message, text_callback cb, void *context);

void GetHighScoreName(const char *message, text_callback cb, void *context);

void HighScores::SetInitial(unsigned rank, unsigned level, unsigned score)
{
    LevelScore ls("\\#6600ffIndigo Ocho", 0, level, score);    
    if (rank < 10)
        scores[rank] = ls;
    levels[level] = ls;
}

HighScores::HighScores() : dirty(false)
{
    SetInitial(~0u,   1,   2200);
    SetInitial(~0u,   2,   3000);    
    SetInitial(~0u,   3,   8000);
    SetInitial(~0u,   4,   8800);    
    SetInitial(~0u,   5,  14000);        
    SetInitial(~0u,   6,  20000);
    SetInitial(~0u,   7,  22000);    
    SetInitial(~0u,   8,  32000);
    SetInitial(~0u,   9,  34000);    
    SetInitial(~0u,  10,  46000);        
    SetInitial(~0u,  11,  61000);            
    SetInitial(~0u,  12,  74000);
    SetInitial(~0u,  13,  82000);
    SetInitial(~0u,  14,  88000);
    SetInitial(~0u,  15,  93000);
    SetInitial(~0u,  16, 106000);
    SetInitial(~0u,  17, 113000);
    SetInitial(~0u,  18, 122000);
    SetInitial(9,    19, 125000);        
    SetInitial(8,    20, 130000);    
    SetInitial(7,    21, 135000);    
    SetInitial(6,    21, 140000);    
    SetInitial(5,    22, 145000);    
    SetInitial(4,    22, 155000);    
    SetInitial(3,    23, 160000);    
    SetInitial(2,    24, 165000);    
    SetInitial(1,    24, 170000);    
    SetInitial(0,    25, 175000);    
}

char *cpp_strdup(const char *s)
{
    if (!s) return 0;
    size_t sz = strlen(s);
    char *ns = new char [sz + 1];
    strcpy(ns, s);
    return ns;
}

bool HighScores::SetFile(const char *filename)
{
    this->filename = cpp_strdup(filename);
    dirty = false;
    return Read(this->filename);
}

HighScores::~HighScores()
{
    if (filename)
        delete [] filename;
    filename = 0;
}

bool HighScores::Read(const char *filename)
{
    FILE *file = fopen(filename, "rb");
    bool success = true;    
    if (!file)
        goto error;
    if (!fread(this, DATA_SIZE, 1, file))
        success = false;
    fclose(file);    
    if (success)
    {
        printf("Read high score file.\n");    
        return true;
    }
error:
    printf("Unable to read from high score file '%s'\n", filename);        
    return false;
}

bool HighScores::Write(const char *filename) const
{
    FILE *file = fopen(filename, "wb");
    bool success = true;    
    if (!file)
        goto error;
    if (!fwrite(this, DATA_SIZE, 1, file))
        success = false;
    fclose(file);
    if (success)
    {
        printf("Wrote high score file\n");
        return true;
    }
error:
    printf("Unable to write to high score file '%s'\n", filename);    
    return false;
}

int HighScores::GetHighScoreIndex(const LevelScore &score) const
{
    // find where it goes in the list..
    int at = -1;
    for (int i = 0; i < MAX_HIGH_SCORES; i++)
    {
        int c = scores[i].CompareScore(score);
        if (c < 0)
        {
            at = i;
            break;
        }
        else if (c == 0)
            return -1; // already entered into high score list..
    }
    return at;
}

void HighScores::SetScoreAtIndex(int index, const LevelScore &score)
{
    if (index == -1)
        return; // no high score, no rank
    if (index < (MAX_HIGH_SCORES - 1))
        memmove(&scores[index + 1], &scores[index], 
                (MAX_HIGH_SCORES - index - 1) * sizeof(LevelScore));
    scores[index] = score;
    dirty = true;
}

int HighScores::SetScore(const LevelScore &score)
{
    int at = GetHighScoreIndex(score);
    if (at == -1)
        return 0; // no high score, no rank
    SetScoreAtIndex(at, score);
    return at + 1; // the rank..! if it's #1, you're the best!
}

bool HighScores::IsLevelHighScore(const LevelScore &score) const
{
    if (score.level >= MAX_LEVEL_SCORES)
        // they're at a level beyond the max.. ? just ignore it.. ?
        return false;
    LevelScore old = levels[score.level];
    if (old.CompareLevel(score) >= 0)
        return false; // no high score
    return true;
}

void HighScores::Print() const
{
    for (int i = 0; i < MAX_HIGH_SCORES; i++)
    {
        char sstr[128];
        scores[i].GetDisplayName(sstr, true);
        printf("%d: %s\n", i, sstr);
    }
}

void HighScores::Clear()
{
    LevelScore empty;
    for (int i = 0; i < MAX_HIGH_SCORES; i++)
        scores[i] = empty;
    for (int i = 0; i < MAX_LEVEL_SCORES; i++)
        scores[i] = empty;
}

bool HighScores::SetLevelScore(const LevelScore &score)
{
    if (!IsLevelHighScore(score))
        return false;
    levels[score.level] = score;
    dirty = true;
    return true;
}

void HighScores::Update()
{
    if (dirty)
        Write(filename);
    dirty = false;
}

HighScoreContext::HighScoreContext(HighScores *scores, const LevelScore &score) : scores(scores), score(score)
{
}

void HighScoreContext::UpdateScores()
{
    if (!IsNameRequired())
    {
        Update(); // update directly..
        return;
    }
    GetHighScoreName("A New High Score!", SetNameAndUpdate, this);
}

bool HighScoreContext::IsNameRequired() const
{
    if (strcmp(score.name, "<no name>") != 0)
        return false;
    int hsi = scores->GetHighScoreIndex(score);
    return scores->IsLevelHighScore(score) || (hsi >= 0 && hsi < 10);
}

void HighScoreContext::Update()
{
    if (scores->IsLevelHighScore(score))
        scores->SetLevelScore(score);
    int hsi = scores->GetHighScoreIndex(score);
    if (hsi >= 0 && hsi < 10)
        scores->SetScoreAtIndex(hsi, score);
    scores->Update();
}

void HighScoreContext::SetNameAndUpdate(const char *text, void *context)
{
    HighScoreContext *info = (HighScoreContext *)context;
    if (info)
    {
        if (text && *text)
        {
            memset(info->score.name, 0, sizeof(info->score.name));
            strncpy(info->score.name, text, sizeof(info->score.name) - 1);
        }
        info->Update();
    }
}