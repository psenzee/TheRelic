/*
 *  SaveGame.h
 *  GLGravity
 *
 *  Created by Paul Senzee on 1/3/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _SAVEGAME_H
#define _SAVEGAME_H

#include "LevelScore.h"

#include <vector>

class SaveGameInfo
{
public:

    int        index; // -1 means don't save?
    LevelScore data;  // a summary

    inline SaveGameInfo(int index, const LevelScore &data) : index(index), data(data) {}

    inline bool operator<(const SaveGameInfo &info) const // for sorting by timestamp, newest first
    {
        return info.data.timestamp < data.timestamp;
    }
};

class SaveGame
{
public:
    
    enum { MAX_SAVE_GAMES = 16 };

    inline SaveGame() : filename(0) {}
    ~SaveGame();    
    
    bool                     SetFile(const char *filename);
    bool                     AutoSave(const LevelScore &score);
    bool                     Save(const LevelScore &score);
    
    inline const LevelScore &GetScore() const { return saved; }

    static void GetSaveFileName(int index, char *buffer);
    static bool LoadGame(int index, SaveGame &save);
    static bool LoadMostRecentGame(SaveGame &game);
    static bool LoadGame(const SaveGameInfo &info, SaveGame &save);
    static void LoadGameInfo(std::vector<SaveGameInfo> &info);
    static bool LoadMostRecentGame(const std::vector<SaveGameInfo> &info, SaveGame &save);
    static void PrintGameInfo(const std::vector<SaveGameInfo> &info);
    
private:
    
    bool Read(const char *filename);
    bool Write(const char *filename) const;
    
    LevelScore  saved;
    const char *filename;
};

#endif // _SAVEGAME_H
