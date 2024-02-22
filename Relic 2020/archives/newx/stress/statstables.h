/*************************************************************************************************/
/*!
    \file   statstables.h

    $Header: //blaze/games/Madden/2010-NG/trunk/component/franchise/stress/statstables.h#3 $
    $Change: 46641 $
    $DateTime: 2009/06/19 14:08:13 $

    \attention
        (c) Electronic Arts Inc. 2009
*/
/*************************************************************************************************/

#ifndef _STATSTABLES_H
#define _STATSTABLES_H

typedef eastl::vector<int>           IntArray;
typedef eastl::vector<eastl::string> StringArray;


/*************************************************************************************************/
/*!
    \class ScoringSummary

    The ScoringSummary struct represents a single record of recorded data for the
    franchise_scoring_summary table as populated by the SubmitScoringSummary() RPC.
    The ScoringSummary struct is necessary because it is the only table of these that
    has a non-numeric field.

*/
/*************************************************************************************************/

struct ScoringSummary
{
    enum { SCORING_SUMMARY_ELEMENT_COUNT = 4 };

    int           mStat0;
    eastl::string mStat1;
    int           mStat2;
    int           mStat3;
};

/*************************************************************************************************/
/*!
    \class StatsTables

    The StatsTables class reads and provides access to a global set of Blaze stress tester
    test data for Madden 2010's Online Franchise.  Each of these files corresponds to data
    that can be used to provide reasonable values for Madden server RPCs used when advancing a Franchise.

*/
/*************************************************************************************************/

class StatsTables
{
public:

    //! Read all the .csv data files and store the results in the StatsTable instance.
    //! The rows argument specifies a maximum number of rows to be read.  Set to zero for all of them.
    void ReadAll(const char *directory, int rows = 0);

    //! Return the ScoringSummary table
    inline const eastl::vector<ScoringSummary> &GetScoringSummary()
    {
        return mScoringSummary;
    }

    //! Return the Injury table
    inline const eastl::vector<IntArray> &GetInjury()
    {
        return mInjury;
    }

    //! Return the AthleteStatsByGame table
    inline const eastl::vector<IntArray> &GetAthleteStatsByGame()
    {
        return mAthleteStatsByGame;
    }

    //! Return the PlayoffSchedule table
    inline const eastl::vector<IntArray> &GetPlayoffSchedule()
    {
        return mPlayoffSchedule;
    }

    //! Return the RegularSchedule table
    inline const eastl::vector<IntArray> &GetRegularSchedule()
    {
        return mRegularSchedule;
    }

    //! Return the TeamStatsByGame table
    inline const eastl::vector<IntArray> &GetTeamStatsByGame()
    {
        return mTeamStatsByGame;
    }

    //! Return the DefensivePlayer table
    inline const eastl::vector<IntArray> &GetDefensivePlayer()
    {
        return mDefensivePlayer;
    }

    //! Return the FieldPlayer table
    inline const eastl::vector<IntArray> &GetFieldPlayer()
    {
        return mFieldPlayer;
    }

    //! Return the KickPlayer table
    inline const eastl::vector<IntArray> &GetKickPlayer()
    {
        return mKickPlayer;
    }

    //! Return the KickReturnPlayer table
    inline const eastl::vector<IntArray> &GetKickReturnPlayer()
    {
        return mKickReturnPlayer;
    }

    //! Return the OffensiveLinePlayer table
    inline const eastl::vector<IntArray> &GetOffensiveLinePlayer()
    {
        return mOffensiveLinePlayer;
    }

    //! Return the OffensivePlayer table
    inline const eastl::vector<IntArray> &GetOffensivePlayer()
    {
        return mOffensivePlayer;
    }

    //! Return a single instance of the StatsTables
    static StatsTables &GetInstance()
    {
        return mStatsTables;
    }

private:

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

    // make these private so that there can only be one
    StatsTables() {}
    StatsTables &operator=(const StatsTables &);

    static StatsTables mStatsTables;
};

#endif // _STATSTABLES_H
