/*************************************************************************************************/
/*!
    \file   statstables.h

    $Header: //blaze/games/Madden/2010-NG/trunk/component/franchise/stress/statstables.h#2 $
    $Change: 44737 $
    $DateTime: 2009/06/04 14:33:39 $

    \attention
        (c) Electronic Arts Inc. 2009
*/
/*************************************************************************************************/

#ifndef _STATSTABLES_H
#define _STATSTABLES_H

typedef eastl::vector<int>           IntArray;
typedef eastl::vector<eastl::string> StringArray;

struct ScoringSummary
{
    enum { SCORING_SUMMARY_ELEMENT_COUNT = 4 };

    int           stat0;
    eastl::string stat1;
    int           stat2;
    int           stat3;
};

bool ReadScoringSummary(const StringArray &in, ScoringSummary &out, int defaultValue = -1);
bool ReadScoringSummary(const eastl::vector<StringArray> &in, eastl::vector<ScoringSummary> &out);
bool ReadNumericStats  (const char *filename, eastl::vector<IntArray> &stats, int rows = 0);
bool ReadScoringSummary(const char *filename, eastl::vector<ScoringSummary> &stats, int rows = 0);

class StatsTables
{
public:

    eastl::vector<ScoringSummary> mScoringSummary;
    eastl::vector<IntArray>       mInjury;
    eastl::vector<IntArray>       mAthleteStatsByGame;
    eastl::vector<IntArray>       mPlayoffSchedule;
    eastl::vector<IntArray>       mRegularSchedule;
    eastl::vector<IntArray>       mTeamStatsByGame;
    eastl::vector<IntArray>       mDefensivePlayer;
    eastl::vector<IntArray>       mFieldPlayer;
    eastl::vector<IntArray>       mKickPlayer;
    eastl::vector<IntArray>       mKickReturnPlayer;
    eastl::vector<IntArray>       mOffensiveLinePlayer;
    eastl::vector<IntArray>       mOffensivePlayer;

    void ReadAll(int rows = 0);

    static StatsTables &GetInstance() { return mStatsTables; }

private:

    static StatsTables mStatsTables;
};

#endif // _STATSTABLES_H

