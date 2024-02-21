/*************************************************************************************************/
/*!
    \file   statstables.cpp

    $Header: //blaze/games/Madden/2010-NG/trunk/component/franchise/stress/statstables.cpp#2 $
    $Change: 44766 $
    $DateTime: 2009/06/04 15:07:27 $

    \attention
        (c) Electronic Arts Inc. 2009
*/
/*************************************************************************************************/

#include "framework/blaze.h"

#include "statstables.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

/*
#ifdef WIN32
#include <windows.h>
#include <mmsystem.h>
#endif
*/

#include "csv.h"

StatsTables StatsTables::mStatsTables;

bool ReadScoringSummary(const StringArray &in, ScoringSummary &out, int defaultValue)
{
    out.stat0 = out.stat2 = out.stat3 = defaultValue;
    out.stat1.clear();
    if (in.size() < ScoringSummary::SCORING_SUMMARY_ELEMENT_COUNT)
    {
        printf("Row does not have enough items to fill the scoring summary (%d/%d found)!\n", in.size(), ScoringSummary::SCORING_SUMMARY_ELEMENT_COUNT);
        return false;
    }
    out.stat0 = GetValue(in[0].c_str(), defaultValue);
    out.stat1 = in[1];
    out.stat2 = GetValue(in[2].c_str(), defaultValue);
    out.stat3 = GetValue(in[3].c_str(), defaultValue);
    return true;
}

bool ReadScoringSummary(const eastl::vector<StringArray> &in, eastl::vector<ScoringSummary> &out)
{
    ScoringSummary stats;
    for (eastl::vector<StringArray>::const_iterator i = in.begin(), e = in.end(); i != e; ++i)
    {
        if (!ReadScoringSummary(*i, stats))
        {
            printf("Error: failed to read scoring summary file at line '%d'\n", (int)(i - in.begin() + 1));
            return false;
        }
        out.push_back(stats);
    }
    return true;
}

bool ReadNumericStats(const char *filename, eastl::vector<IntArray> &stats, int rows)
{
    printf("Reading '%s' ", filename);

//#ifdef WIN32
//    unsigned time = ::timeGetTime();
//#endif

    if (!ReadSeparatedIntsFile(filename, ',', stats, rows))
    {
        printf("Unable to read csv file '%s'!\n", filename);
        return false;
    }

//#ifdef WIN32
//    printf("(%.2fs); ", (::timeGetTime() - time) / 1000.0f);
//#endif

    PrintTableInfo(stats);

    return !stats.empty();
}

bool ReadScoringSummary(const char *filename, eastl::vector<ScoringSummary> &stats, int rows)
{
    printf("Reading '%s' ", filename);

    eastl::vector<StringArray> data;

//#ifdef WIN32
//    unsigned time = ::timeGetTime();
//#endif

    if (!ReadSeparatedValuesFile(filename, ',', data, rows))
    {
        printf("Unable to read csv file '%s'!\n", filename);
        return false;
    }

    ReadScoringSummary(data, stats);

//#ifdef WIN32
//    printf("(%.2fs); ", (::timeGetTime() - time) / 1000.0f);
//#endif

    PrintTableInfo(data);

    return !stats.empty();
}

void StatsTables::ReadAll(int rows)
{
    const char *FILENAME_INJURY                = "franchise_injury.csv",
               *FILENAME_ATHLETE_STATS         = "franchise_athlete_stats_by_game.csv",
               *FILENAME_SCORING_SUMMARY       = "franchise_scoring_summary.csv",
               *FILENAME_PLAYOFF_SCHEDULE      = "franchise_playoff_schedule.csv",
               *FILENAME_REGULAR_SCHEDULE      = "franchise_regular_schedule.csv",
               *FILENAME_TEAM_STATS            = "franchise_team_stats_by_game.csv",
               *FILENAME_DEFENSIVE_PLAYER      = "stats_alltime_defensiveplayerstats.csv",
               *FILENAME_FIELD_PLAYER          = "stats_alltime_fieldplayerstats.csv",
               *FILENAME_KICK_PLAYER           = "stats_alltime_kickplayerstats.csv",
               *FILENAME_KICK_RETURN_PLAYER    = "stats_alltime_kickreturnplayerstats.csv",
               *FILENAME_OFFENSIVE_LINE_PLAYER = "stats_alltime_offensivelineplayerstats.csv",
               *FILENAME_OFFENSIVE_PLAYER      = "stats_alltime_offensiveplayerstats.csv";

    printf("Reading stats tables..\n");

//#ifdef WIN32
//    unsigned totalTime = ::timeGetTime();
//#endif

    ReadNumericStats  (FILENAME_INJURY,                mInjury,              rows);
    ReadScoringSummary(FILENAME_SCORING_SUMMARY,       mScoringSummary,      rows);
    ReadNumericStats  (FILENAME_ATHLETE_STATS,         mAthleteStatsByGame,  rows);
    ReadNumericStats  (FILENAME_PLAYOFF_SCHEDULE,      mPlayoffSchedule,     rows);
    ReadNumericStats  (FILENAME_REGULAR_SCHEDULE,      mRegularSchedule,     rows);
    ReadNumericStats  (FILENAME_TEAM_STATS,            mTeamStatsByGame,     rows);
    ReadNumericStats  (FILENAME_DEFENSIVE_PLAYER,      mDefensivePlayer,     rows);
    ReadNumericStats  (FILENAME_FIELD_PLAYER,          mFieldPlayer,         rows);
    ReadNumericStats  (FILENAME_KICK_PLAYER,           mKickPlayer,          rows);
    ReadNumericStats  (FILENAME_KICK_RETURN_PLAYER,    mKickReturnPlayer,    rows);
    ReadNumericStats  (FILENAME_OFFENSIVE_LINE_PLAYER, mOffensiveLinePlayer, rows);
    ReadNumericStats  (FILENAME_OFFENSIVE_PLAYER,      mOffensivePlayer,     rows);

//#ifdef WIN32
//    printf("Total time %.2fs.\n", (::timeGetTime() - totalTime) / 1000.0f);
//#endif

    printf("Done.\n");
}

