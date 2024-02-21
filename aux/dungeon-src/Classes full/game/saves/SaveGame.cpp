/*
 *  SaveGame.cpp
 *  GLGravity
 *
 *  Created by Paul Senzee on 1/3/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "SaveGame.h"
#include "core/global.h"

#include <stdio.h>
#include <string.h>
#include <time.h>

#include <vector>
#include <algorithm>

extern char *cpp_strdup(const char *s);

bool SaveGame::SetFile(const char *filename)
{
    if (this->filename)
    {
        delete [] this->filename;
        this->filename = 0;
    }
    if (filename)
    {
        this->filename = cpp_strdup(filename);
        return Read(this->filename);
    }
    return true; // no filename means don't save..
}

SaveGame::~SaveGame()
{
    if (filename)
        delete [] filename;
    filename = 0;
}

bool SaveGame::Read(const char *filename)
{
    if (!filename)
        return false;

    FILE *file = fopen(filename, "rb");
    LevelScore score;
    bool success = true;    
    if (!file)
        goto error;
    if (!fread(&score, sizeof(LevelScore), 1, file))
        success = false;
    fclose(file);
    saved = score;
    if (success)
    {
        printf("Read (Level %u Score %u) from save file '%s'\n",
               saved.level, saved.score, filename);
        return true;
    }
error:
    printf("Unable to read from save game file '%s'\n", filename);
    return false;
}

bool SaveGame::Write(const char *filename) const
{
    if (!filename)
        return false;

    FILE *file = fopen(filename, "wb");
    bool success = true;    
    if (!file)
        goto error;
    if (!fwrite(&saved, sizeof(LevelScore), 1, file))
        success = false;
    fclose(file);
    if (success)
    {
        printf("Wrote save game file\n");
        return true;
    }
error:
    printf("Unable to write to save game file '%s'\n", filename);
    return false;
}

bool SaveGame::Save(const LevelScore &score)
{
    saved = score;
    return Write(filename);
}

bool SaveGame::AutoSave(const LevelScore &score)
{
    // we save the latest level
    if (score.CompareLevel(saved) > 0)
        return Save(score);
    return false;
}

// static

void SaveGame::GetSaveFileName(int index, char *buffer)
{
    sprintf(buffer, "saved%u.bin", index);
    strcpy(buffer, globalTranslateReadWritePath(buffer));
}

bool SaveGame::LoadGame(int index, SaveGame &save)
{
    if (index < 0)
        return save.SetFile(0); // no save file
    char filename[1024];
    GetSaveFileName(index, filename);
    return save.SetFile(filename);
}

bool SaveGame::LoadMostRecentGame(SaveGame &game)
{
    SaveGame   save;
    LevelScore score;
    time_t     time  = 0;
    int        index = 0; // default to zeroth save game, if none available
    for (int i = 0; i < MAX_SAVE_GAMES; i++)
    {
        if (!LoadGame(i, save))
            break;
        score = save.GetScore();
        if (score.timestamp > time)
        {
            time  = score.timestamp;
            index = i;
        }
    }
    return LoadGame(index, game);
}

bool SaveGame::LoadGame(const SaveGameInfo &info, SaveGame &save)
{
    if (info.index < 0)
    {
        save.SetFile(0);
        return true;
    }
    return LoadGame(info.index, save);
}

void SaveGame::LoadGameInfo(std::vector<SaveGameInfo> &info)
{
    info.clear();
    SaveGame   save;
    LevelScore score;
    for (int i = 0; i < MAX_SAVE_GAMES; i++)
    {
        if (!LoadGame(i, save))
            continue;
        info.push_back(SaveGameInfo(i, save.GetScore()));
    }
    std::sort(info.begin(), info.end());
}

bool SaveGame::LoadMostRecentGame(const std::vector<SaveGameInfo> &info, SaveGame &save)
{
    if (info.empty())
        return LoadGame(0, save);
    return LoadGame(info[0], save);
}

void SaveGame::PrintGameInfo(const std::vector<SaveGameInfo> &info)
{
    for (std::vector<SaveGameInfo>::const_iterator i = info.begin(), e = info.end(); i != e; ++i)
    {
        char display[1024];
        (*i).data.GetDisplayName(display);
        printf("%s\n", display);
    }
}