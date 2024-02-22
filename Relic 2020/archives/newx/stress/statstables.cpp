/*************************************************************************************************/
/*!
    \file   statstables.cpp

    $Header: //blaze/games/Madden/2010-NG/trunk/component/franchise/stress/statstables.cpp#3 $
    $Change: 46641 $
    $DateTime: 2009/06/19 14:08:13 $

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

#include "csv.h"


/*************************************************************************************************/
/*!
    \class StatsTables

    The StatsTables class reads and provides access to a global set of Blaze stress tester
    test data for Madden 2010's Online Franchise.  Each of these files corresponds to data
    that can be used to provide reasonable values for Madden server RPCs used when advancing a Franchise.

*/
/*************************************************************************************************/


StatsTables StatsTables::mStatsTables;

static bool ReadNumericStats  (const char *filename, eastl::vector<IntArray> &stats, int rows = 0);
static bool ReadScoringSummary(const char *filename, eastl::vector<ScoringSummary> &stats, int rows = 0);

//! Read a single ScoringSummary record from a vector of strings
static bool ReadScoringSummary(const StringArray &in, ScoringSummary &out, int defaultValue = -1)
{
    out.mStat0 = out.mStat2 = out.mStat3 = defaultValue;
    out.mStat1.clear();
    if (in.size() < ScoringSummary::SCORING_SUMMARY_ELEMENT_COUNT)
    {
        printf("Row does not have enough items to fill the scoring summary (%d/%d found)!\n", in.size(), ScoringSummary::SCORING_SUMMARY_ELEMENT_COUNT);
        return false;
    }
    out.mStat0 = GetValue(in[0].c_str(), defaultValue);
    out.mStat1 = in[1];
    out.mStat2 = GetValue(in[2].c_str(), defaultValue);
    out.mStat3 = GetValue(in[3].c_str(), defaultValue);
    return true;
}

//! Read an array of ScoringSummary records from a vector of vector of strings
static bool ReadScoringSummary(const eastl::vector<StringArray> &in, eastl::vector<ScoringSummary> &out)
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

//! Read a set of numeric stats from a file and store them in a vector of vector of ints
//! The rows argument specifies a maximum number of rows to be read.  Set to zero for all of them.
static bool ReadNumericStats(const char *filename, eastl::vector<IntArray> &stats, int rows)
{
    printf("Reading '%s' ", filename);

    if (!ReadSeparatedIntsFile(filename, ',', stats, rows))
    {
        printf("Unable to read csv file '%s'!\n", filename);
        return false;
    }

    PrintTableInfo(stats);

    return !stats.empty();
}

//! Read an array of ScoringSummary records from a file.
//! The rows argument specifies a maximum number of rows to be read.  Set to zero for all of them.
static bool ReadScoringSummary(const char *filename, eastl::vector<ScoringSummary> &stats, int rows)
{
    printf("Reading '%s' ", filename);

    eastl::vector<StringArray> data;

    if (!ReadSeparatedValuesFile(filename, ',', data, rows))
    {
        printf("Unable to read csv file '%s'!\n", filename);
        return false;
    }

    ReadScoringSummary(data, stats);

    PrintTableInfo(data);

    return !stats.empty();
}

//! Properly concatenate a directory and file to form a full path
static eastl::string MakePath(const char *dir, const char *file)
{
    eastl::string path(dir != NULL ? dir : "");
	if (!path.empty())
	{
        char ch = path[path.size() - 1];
		if (ch != '\\' && ch != '/')
			path += "/";
	}
	path += (file != NULL ? file : "");
    for (int i = 0, sz = (int)path.size(); i < sz; i++)
        if (path[i] == '\\')
            path[i] = '/';
    return path;
}

//! Read all the .csv data files and store the results in the StatsTable instance.
//! The rows argument specifies a maximum number of rows to be read.  Set to zero for all of them.
void StatsTables::ReadAll(const char *directory, int rows)
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

    eastl::string path;

    ReadNumericStats  ((path = MakePath(directory, FILENAME_INJURY)).c_str(),                mInjury,              rows);
    ReadScoringSummary((path = MakePath(directory, FILENAME_SCORING_SUMMARY)).c_str(),       mScoringSummary,      rows);
    ReadNumericStats  ((path = MakePath(directory, FILENAME_ATHLETE_STATS)).c_str(),         mAthleteStatsByGame,  rows);
    ReadNumericStats  ((path = MakePath(directory, FILENAME_PLAYOFF_SCHEDULE)).c_str(),      mPlayoffSchedule,     rows);
    ReadNumericStats  ((path = MakePath(directory, FILENAME_REGULAR_SCHEDULE)).c_str(),      mRegularSchedule,     rows);
    ReadNumericStats  ((path = MakePath(directory, FILENAME_TEAM_STATS)).c_str(),            mTeamStatsByGame,     rows);
    ReadNumericStats  ((path = MakePath(directory, FILENAME_DEFENSIVE_PLAYER)).c_str(),      mDefensivePlayer,     rows);
    ReadNumericStats  ((path = MakePath(directory, FILENAME_FIELD_PLAYER)).c_str(),          mFieldPlayer,         rows);
    ReadNumericStats  ((path = MakePath(directory, FILENAME_KICK_PLAYER)).c_str(),           mKickPlayer,          rows);
    ReadNumericStats  ((path = MakePath(directory, FILENAME_KICK_RETURN_PLAYER)).c_str(),    mKickReturnPlayer,    rows);
    ReadNumericStats  ((path = MakePath(directory, FILENAME_OFFENSIVE_LINE_PLAYER)).c_str(), mOffensiveLinePlayer, rows);
    ReadNumericStats  ((path = MakePath(directory, FILENAME_OFFENSIVE_PLAYER)).c_str(),      mOffensivePlayer,     rows);

    printf("Done.\n");
}
