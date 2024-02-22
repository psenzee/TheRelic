/*************************************************************************************************/
/*!
    \file

    $Header: //blaze/games/Madden/2010-NG/trunk/component/franchise/stress/franchisemodule.h#37 $
    $Change: 46575 $
    $DateTime: 2009/06/19 09:46:10 $

    \attention
        (c) Electronic Arts Inc. 2009
*/
/*************************************************************************************************/

#ifndef BLAZE_STRESS_FRANCHISEMODULE_H
#define BLAZE_STRESS_FRANCHISEMODULE_H

/*** Include files *******************************************************************************/

#include "stressmodule.h"
#include "stressinstance.h"
#include "statstables.h"
#include "franchise/tdf/franchise.h"
#include "franchise/rpc/franchiseslave_proxy.h"
#include "framework/util/time.h"
#include <eathread/eathread_futex.h>

using namespace Blaze::Franchise;

// NEW LUA PIECE
#include "luainterpreter.h"
// END NEW LUA PIECE

/*** Defines/Macros/Constants/Typedefs ***********************************************************/

namespace Blaze
{
namespace Stress
{

// Create one of these for each member found.
struct MemberData
{
    MemberData() { bzid = 0; fid = 0; tid = 0; isGM = isBanned = false; persona[0] = '\0'; }
    BlazeId     bzid;
    char8_t     persona[MAX_PERSONA_LEN];
    FranchiseId fid;
    TeamId      tid;
    bool        isGM;        // this member may be the GM of this franchise
    bool        isBanned;
};

// Create one of these for each athlete found per franchise (same athlete may be in more than one franchise).
struct AthleteData
{
    AthleteData() { aid = 0; fid = 0; tid = 0; }
    AthleteId   aid;
    FranchiseId fid;
    TeamId      tid;
};

// Create one of these for each franchise found or created.
struct FranchiseIdData
{
    FranchiseIdData() { fid = 0; gmId = 0; weCreatedIt = weJoinedIt = false; }
    FranchiseId                 fid;
    BlazeId                     gmId;   // creator of the franchise
    bool                        weCreatedIt;
    bool                        weJoinedIt;
    bool                        fantasyDraftIsOn;
    SeasonId                    seasonId;
    WindowId                    windowId;
    FranchiseState              state;
    eastl::vector<MemberData*>  memberData;
};

struct rpcData
{
    uint64_t startingAve; // in us
    uint64_t currentAve;  // in us
    uint32_t exceedCount; // number of times the time has exceeded the average threshold
    uint32_t bumpCount;   // number of times we've bumped the threshold
    bool     allowRpc;    // set to 0 to turn off this rpc
    uint32_t errorCount;  // number of errors this rpc had
};

class StressInstance;
class StressConnection;
class Login;


class FranchiseModule : public StressModule
{
    NON_COPYABLE(FranchiseModule);

public:
    static StressModule* create();

    enum Action
    {
        ACTION_INVALID = -1,
        ACTION_NOOP,
        ACTION_CREATE_FRANCHISE,
        ACTION_CREATE_DELETE_FRANCHISE,
        ACTION_DELETE_FRANCHISE,
        ACTION_JOIN_FRANCHISE,
        ACTION_ALL_GETS,
        ACTION_INDEPTH_TEST,
        ACTION_INDEPTH_TEST2,
        ACTION_ADVANCE_WEEK
    };

    Action       getAction() const { return mAction; }
    uint32_t     getCreateFranchiseCount() const { return mCreateFranchiseCount; }
    uint32_t     getRpcDelay() const { return mRpcDelay; }
    uint32_t     getNumConnections() const { return mNumConnections; }
    StatsTables* getStatsTable() { return &StatsTables::GetInstance(); }
    bool         getStatsEnabled() { return mStatsEnabled; }

    enum Metric
    {
        METRIC_GET_DRAFTABLE_ATHLETES,
        METRIC_GET_DRAFT_PROFILE,
        METRIC_SET_DRAFT_PROFILE,
        METRIC_CREATE_FRANCHISE,
        METRIC_JOIN_FRANCHISE,
        METRIC_GET_FRANCHISE,
        METRIC_GET_FRANCHISES_BY_USER,
        METRIC_DELETE_FRANCHISE,
        METRIC_GET_MEMBER_BY_BLAZE_ID,
        METRIC_GET_MEMBER_BY_TEAM_ID,
        METRIC_FIND_FRANCHISES,
        METRIC_FIND_FRANCHISES_ASYNC,
        METRIC_REMOVE_MEMBER,
        METRIC_UPDATE_FRANCHISE_SETTINGS,
        METRIC_POST_NEWS,
        METRIC_GET_NEWS,
        METRIC_CLEAR_BAN,
        METRIC_SEND_INVITATION,
        METRIC_GET_INVITATIONS,
        METRIC_PROCESS_INVITATION,
        METRIC_PROPOSE_TRADE,
        METRIC_PROCESS_TRADE,
        METRIC_GET_TRADES,
        METRIC_GET_MEMBERS,
        METRIC_JOIN_DRAFT,
        METRIC_LEAVE_DRAFT,
        METRIC_BEGIN_DRAFT,
        METRIC_PAUSE_DRAFT,
        METRIC_DRAFT_ATHLETE,
        METRIC_GET_DRAFT_ORDER,
        METRIC_SEND_DRAFT_CHAT,
        METRIC_GET_DRAFT_HISTORY,
        METRIC_POLL_DRAFT,
        METRIC_CHECK_DRAFT,
        METRIC_GET_DISPLAY_ROSTER,
        METRIC_GET_GAMEPLAY_ROSTER,
        METRIC_GET_DISPLAY_ATHLETE,
        METRIC_GET_DEPTH_CHART,
        METRIC_SET_DEPTH_CHART,
        METRIC_GENERATE_DEPTH_CHART,
        METRIC_GET_REGULAR_SCHEDULE,
        METRIC_GET_PLAYOFF_SCHEDULE,
        METRIC_GET_GAMEPLAY_INFORMATION,
        METRIC_ADVANCE_FRANCHISE,
        METRIC_SIM_SEASON,
        METRIC_ATHLETE_PROGRESS,
        METRIC_GET_RAW_DATA,
        METRIC_SIGN_ATHLETE,
        METRIC_RELEASE_ATHLETE,
        METRIC_GET_NUM_TEAM_INJURIES,
        METRIC_GET_INJURIES_BY_TEAM,
        METRIC_GET_GAME_BOX_SCORE_BY_GAME,
        METRIC_GET_GAME_BOX_SCORES_BY_WEEK,
        METRIC_GET_GAME_BOX_SCORES_BY_TEAM,
        METRIC_GET_SCORING_SUMMARY,
        METRIC_GET_TEAM_STATS_BY_GAME,
        METRIC_GET_ATHLETE_STATS_BY_GAME,
        METRIC_PROMOTE_TO_GM,
        METRIC_GET_FRANCHISE_RANK,
        METRIC_GET_TOP_FRANCHISES,
        METRIC_GET_SUPER_BOWLS,
        METRIC_SUBMIT_FRANCHISE_GAME_STATS,
        METRIC_SUBMIT_SCORING_SUMMARY,
        METRIC_SUBMIT_ATHLETE_GAME_STATS,
        METRIC_SUBMIT_TEAM_GAME_STATS,
        METRIC_VALIDATE_ROSTERS,
        METRIC_VALIDATE_DEPTH_CHARTS,
        METRIC_GET_CPU_TRADES,
        METRIC_GET_DISPLAY_ROSTER_BY_ATHLETE,
        METRIC_SUBMIT_ANTI_CHEAT_INFO,
        METRIC_SET_MEMBER_STATUS,
        METRIC_GET_RETIREMENTS,
        METRIC_GET_GAME_STAT_REPORT,
        METRIC_VALIDATE_MATCH,
        METRIC_GET_SERVER_SETTINGS,
        METRIC_SET_JERSEY_NUMBER,
        NUM_METRICS
    };

    void addMetric(Metric metric, const TimeValue& ms);

    // Perform any necessary configuration prior to starting the stress test
    virtual bool initialize(const ConfigMapBase& config);

    // Called by the core system to create stress instances for this module
    virtual StressInstance* createInstance(StressConnection* connection, Login* login);

    // NEW LUA PIECE
    LuaInterpreter *getLuaInterpreter();
    // END NEW LUA PIECE

    virtual ~FranchiseModule();

    bool isRpcAllowed (int32_t rpcId) { return mRpcData[rpcId].allowRpc; }
    void incRpcErrorCount (int32_t rpcId) { mRpcData[rpcId].errorCount++; }
    void incNumUsers () { mNumUsers++; }
    void decNumUsers () { mNumUsers--; }

protected:
    FranchiseModule();

    bool parseConfig(const ConfigMapBase& config);

private:
    void dumpStats2();

    Action   mAction;

    //char8_t mContentCategoryName[Franchise::ContentInfo::MAX_CONTENTCATEGORY_LEN];
    uint32_t mCreateFranchiseCount;
    uint32_t mRpcDelay;
    uint32_t mNumConnections;
    uint32_t mBumpCountLimit;
    uint32_t mExceedCountLimit;
    uint32_t mNumUsers;
    uint32_t mMaxUsers;
    bool     mAllowTurnOffs;

    uint64_t mMetricTime[NUM_METRICS];
    uint64_t mMetricCount[NUM_METRICS];
    uint64_t mMetricMin[NUM_METRICS];
    uint64_t mMetricMax[NUM_METRICS];
    uint64_t mMetricAve[NUM_METRICS];

    rpcData  mRpcData[NUM_METRICS];

    TimerId  mTimerId2;
    EA::Thread::Futex mFutex;

    bool                           mStatsEnabled;

    // NEW LUA PIECE
    LuaInterpreter                *createLuaInterpreter();

    const char                    *mLuaScript;
    EA::Thread::ThreadLocalStorage mLuaInterpreterTls;
    // END NEW LUA PIECE
};


class FranchiseInstance : public StressInstance
{
    NON_COPYABLE(FranchiseInstance);
    friend class FranchiseModule;

public:
    virtual ~FranchiseInstance();

    // This is the entry point for a stress instance to start running
    virtual void start();

    int32_t getId() const { return mId; }

    FranchiseSlaveProxy *getProxy() { return mProxy; }

protected:
    //Override this method to do your task
    virtual BlazeRpcError execute();
    virtual const char8_t *getName() const;

    // Override these methods to track connection/disconnection events
    virtual void onDisconnected() {}
    virtual void onLogin(BlazeRpcError result) {}

    int32_t getSeed() const { return mTrialIndex + StressInstance::getIdent(); }

    FranchiseInstance(FranchiseModule *owner, StressConnection* connection, Login* login, int32_t id);

// NEW LUA PIECE
public: // exposing these to Lua, alternatively, we could set up a friend relationship or look at doing something else..
// END NEW LUA PIECE
    BlazeRpcError createFranchiseLocal(int32_t indexOfFranchise, FranchiseId* returnedCreatedFranId); 
    BlazeRpcError joinFranchiseLocal(FranchiseId createdFranId); 
    BlazeRpcError deleteFranchiseLocal(FranchiseId franId); 
    BlazeRpcError getFranchiseLocal(FranchiseId franId);
    BlazeRpcError getFranchisesByUserLocal();
    BlazeRpcError updateFranchiseSettingsLocal(FranchiseId franId);
    BlazeRpcError getMemberByBlazeIdLocal(FranchiseId franId);
    BlazeRpcError getMemberByTeamIdLocal(FranchiseId franId);
    BlazeRpcError getMembersLocal(FranchiseId franId);
    BlazeRpcError removeMemberLocal(FranchiseId franId);
    BlazeRpcError promoteToGMLocal(FranchiseId franId);
    BlazeRpcError getRegularScheduleLocal(FranchiseId franId);
    BlazeRpcError getPlayoffScheduleLocal(FranchiseId franId);
    BlazeRpcError getDisplayRosterLocal(FranchiseId franId);
    BlazeRpcError getGameplayRosterLocal(FranchiseId franId);
    BlazeRpcError getNumTeamInjuriesLocal(FranchiseId franId);
    BlazeRpcError getInjuriesByTeamLocal(FranchiseId franId);
    BlazeRpcError getGameBoxScoreByGameLocal(FranchiseId franId);
    BlazeRpcError getGameBoxScoresByWeekLocal(FranchiseId franId);
    BlazeRpcError getGameBoxScoresByTeamLocal(FranchiseId franId);
    BlazeRpcError getScoringSummaryLocal(FranchiseId franId);
    BlazeRpcError getTeamStatsByGameLocal(FranchiseId franId);
    BlazeRpcError getAthleteStatsByGameLocal(FranchiseId franId);
    BlazeRpcError clearBanLocal(FranchiseId franId);
    BlazeRpcError getTradesLocal (FranchiseId franId, TradeId& tradeId);
    BlazeRpcError getSuperbowlsLocal (FranchiseId franId);
    BlazeRpcError getDisplayAthleteLocal (FranchiseId franId);
    BlazeRpcError getRawDataLocal (FranchiseId franId);
    BlazeRpcError getDepthChartLocal (FranchiseId franId);
    BlazeRpcError getNewsLocal (FranchiseId franId);
    BlazeRpcError getInvitationsLocal (FranchiseId franId);
    BlazeRpcError getDraftProfileLocal (FranchiseId franId);
    BlazeRpcError getDraftHistoryLocal (FranchiseId franId);
    BlazeRpcError getDraftableAthletesLocal (FranchiseId franId);
    BlazeRpcError getDraftOrderLocal (FranchiseId franId);
    BlazeRpcError getTopFranchisesLocal (FranchiseId franId);
    BlazeRpcError getFranchiseRankLocal (FranchiseId franId);
    BlazeRpcError validateRostersLocal (FranchiseId franId);
    BlazeRpcError validateDepthChartsLocal (FranchiseId franId);
    BlazeRpcError setDraftProfileLocal (FranchiseId franId);
    BlazeRpcError findFranchisesAsyncLocal ();
    BlazeRpcError postNewsLocal (FranchiseId franId);
    BlazeRpcError sendInvitationLocal (FranchiseId franId, bool useOurFranchisesOnly = false);
    BlazeRpcError processInvitationLocal (FranchiseId franId);
    BlazeRpcError proposeTradeLocal (FranchiseId& franId);
    BlazeRpcError processTradeLocal (FranchiseId franId, TradeId tradeId);
    BlazeRpcError joinDraftLocal (FranchiseId franId);
    BlazeRpcError leaveDraftLocal (FranchiseId franId);
    BlazeRpcError beginDraftLocal (FranchiseId franId);
    BlazeRpcError pauseDraftLocal (FranchiseId franId);
    BlazeRpcError draftAthleteLocal (FranchiseId franId);
    BlazeRpcError sendDraftChatLocal (FranchiseId franId);
    BlazeRpcError pollDraftLocal (FranchiseId franId);
    BlazeRpcError checkDraftLocal (FranchiseId franId);
    BlazeRpcError setDepthChartLocal (FranchiseId franId);
    BlazeRpcError generateDepthChartLocal (FranchiseId franId);
    BlazeRpcError advanceFranchiseLocal (FranchiseId franId, FranchiseState nextState);
    BlazeRpcError simSeasonLocal (FranchiseId franId);
    BlazeRpcError athleteProgressLocal (FranchiseId franId);
    BlazeRpcError signAthleteLocal (FranchiseId franId);
    BlazeRpcError releaseAthleteLocal (FranchiseId franId);
    BlazeRpcError submitFranchiseGameStatsLocal (FranchiseId franId);
    BlazeRpcError submitScoringSummaryLocal (FranchiseId franId);
    BlazeRpcError submitAthleteGameStatsLocal (FranchiseId franId);
    BlazeRpcError submitTeamGameStatsLocal (FranchiseId franId);
    BlazeRpcError getCpuTradesLocal(FranchiseId franId);
    BlazeRpcError getDisplayRosterByAthleteLocal(FranchiseId franId);
    BlazeRpcError submitAntiCheatInfoLocal(FranchiseId franId);
    BlazeRpcError getGameplayInformationLocal (FranchiseId franId);
    BlazeRpcError setMemberStatusLocal (FranchiseId franId);
    BlazeRpcError getRetirementsLocal (FranchiseId franId);
    BlazeRpcError getGameStatReportLocal (FranchiseId franId);
    BlazeRpcError validateMatchLocal (FranchiseId franId);
    BlazeRpcError getServerSettingsLocal (FranchiseId franId);
    BlazeRpcError setJerseyNumberLocal (FranchiseId franId);
    BlazeRpcError advanceWeekTest ();

    BlazeRpcError getRegularSchedule (Franchise::GetRegularScheduleRequest& req, Franchise::GetRegularScheduleResponse& resp);
    BlazeRpcError validateRosters (Franchise::ValidateRostersRequest& req);
    BlazeRpcError getGameplayInformation (Franchise::GetGameplayInformationRequest& req, Franchise::GetGameplayInformationResponse& resp);
    BlazeRpcError advanceFranchise (Franchise::AdvanceFranchiseRequest& req);
    BlazeRpcError getFranchise (Franchise::GetFranchiseRequest& req, Franchise::Franchise& resp);
    BlazeRpcError submitScoringSummary (Franchise::SubmitScoringSummaryRequest& req, Franchise::TeamId homeTeamId, Franchise::TeamId awayTeamId);
    BlazeRpcError submitAthleteGameStats (Franchise::SubmitAthleteGameStatsRequest& req);
    BlazeRpcError submitTeamGameStats (Franchise::SubmitTeamGameStatsRequest& req);
    BlazeRpcError submitFranchiseGameStats (Franchise::SubmitFranchiseGameStatsRequest& req, Franchise::GetGameplayInformationResponse& ggiResp);
    BlazeRpcError beginDraft(Franchise::BeginDraftRequest& req);
    BlazeRpcError joinDraft (Franchise::JoinDraftRequest& req, Franchise::JoinDraftResponse& resp);

// NEW LUA PIECE
public: // exposing these to Lua, alternatively, we could set up a friend relationship or look at doing something else..
// END NEW LUA PIECE
    BlazeId       findBlazeIdOfSomeoneElse (FranchiseId franId);
    BlazeId       getBlazeId ();
    BlazeId       getGMForFranchise (FranchiseId franId);
    BlazeId       findNonGMMember (FranchiseId franId);
    BlazeId       findAnyMember (FranchiseId franId, int32_t which=1);
    BlazeId       findBannedMember (FranchiseId franId);
    void          findAllFranchiseIds ();
    FranchiseId   getRandomFranchiseId ();
    FranchiseId   getFranchiseWeCreated ();
    FranchiseId   getFranchiseWithFantasyDraftSetting (bool fantasyDraftDesired, bool needGM, BlazeId bzidDesired);
    FranchiseId   getFranchiseWeAreNotAMemberOf ();
    FranchiseId   getGMFranchiseIdForUser (bool needFantasyDraftOff = false);
    FranchiseId   getAnyFranchiseIdForUser (FranchiseId defaultFranId);
    BlazeRpcError getDisplayRosterInfo (TeamId tid, FranchiseId fid, Franchise::GetDisplayRosterResponse& out);
    BlazeRpcError getAthletesOnTeamInFranchise (FranchiseId franId, TeamId teamId, bool sleepOk = true);
    AthleteId     getOneAthleteOnTeamInFranchise (FranchiseId franId, TeamId teamId, bool sleepOk = true);
    AthleteId     getAthleteIdFromFranTeamId (FranchiseId franId, TeamId teamId);
    TeamId        getMyTeamId (FranchiseId franId, BlazeId blazeId);
    void          changeBannedFlag (FranchiseId franId, BlazeId bzId, bool newState);
    void          changeGMFlag (FranchiseId franId, BlazeId bzId, bool newState);
    void          findMembersInOtherFranchises (FranchiseId franId, eastl::vector<MemberData*>& membersVec, bool fransWeCreatedOnly = true);
    void          clearMemberDataForFranchise (FranchiseId franId);
    bool          isFranchiseInFranchiseVector (FranchiseId franId);
    TeamId        getDifferentTeamId (TeamId tId);
    FranchiseState getNextFranchiseState (FranchiseId franId);
    
    void           fillSubmitAthleteGameStatsRequest(Franchise::SubmitAthleteGameStatsRequest& req);
    void           fillAthleteStatsByTeam(AthleteStatsByTeam* athleteStats, Franchise::AthleteId athleteId);
    
    void         fillSubmitFranchiseGameStatsRequest(Franchise::SubmitFranchiseGameStatsRequest& req, Franchise::GetGameplayInformationResponse& ggiResp);
    void         fillFranchiseTeamStats(Franchise::SubmitFranchiseGameStatsRequest& req, Franchise::GetGameplayInformationResponse& ggiResp);
    void         fillInjuryPlayerStats(Franchise::SubmitFranchiseGameStatsRequest& req, Franchise::GetGameplayInformationResponse& ggiResp);
    void         fillOffensivePlayerStat(Franchise::SubmitFranchiseGameStatsRequest& req, Franchise::GetGameplayInformationResponse& ggiResp);
    void         fillOffensiveLinePlayerStat(Franchise::SubmitFranchiseGameStatsRequest& req, Franchise::GetGameplayInformationResponse& ggiResp);
    void         fillDefensivePlayerStats(Franchise::SubmitFranchiseGameStatsRequest& req, Franchise::GetGameplayInformationResponse& ggiResp);
    void         fillKickPlayerStats(Franchise::SubmitFranchiseGameStatsRequest& req, Franchise::GetGameplayInformationResponse& ggiResp);
    void         fillKickReturnPlayerStats(Franchise::SubmitFranchiseGameStatsRequest& req, Franchise::GetGameplayInformationResponse& ggiResp);
    void         fillFieldPlayerStat(Franchise::SubmitFranchiseGameStatsRequest& req, Franchise::GetGameplayInformationResponse& ggiResp);
    void         internalFillPlayerStats(Franchise::SubmitFranchiseGameStatsRequest& req, 
                                        Franchise::GetGameplayInformationResponse& ggiResp,
                                        int randomAthleteCount1,
                                        int randomAthleteCount2,
                                        const eastl::vector<IntArray> &statArray);



// NEW LUA PIECE
private:
// END NEW LUA PIECE

    FranchiseIdData* findFidDataFromFranId (FranchiseId franId);

    typedef eastl::map<AthleteId, AthleteData*>      athleteIdAthleteDataMapType;
    typedef eastl::vector<FranchiseIdData*>          FranchiseIdDataVectorType;
    typedef eastl::vector<MemberData*>               MemberDataVectorType;

    FranchiseModule*            mOwner;
    const char8_t*              mName;
    FranchiseSlaveProxy*        mProxy;
    int32_t                     mTrialIndex;
    int32_t                     mCreateFranchiseCount;
    int32_t                     mRpcDelay;
    int32_t                     mId;
    BlazeId                     mBlazeId;
    TeamId                      mTeamId;
    Franchise::GetDraftableAthletesResponse::DraftableAthleteList  mDraftableAthletesList;
    DraftManagerId              mDraftMgrId;
    uint32_t                    mDraftRound;
    uint32_t                    mDraftSeqNum;
    athleteIdAthleteDataMapType mAthleteDataMap;
    FranchiseIdDataVectorType   mFranchiseIdData;
    static  uint32_t            mTestCount;
    BlazeId                     mInviteeBlazeId;
    bool                        mWeAreGM;
    FranchiseId                 mDraftFranId;
    int32_t                     mRosterId;
    FranchiseIdData*            mCurrentFranData;

    // NEW LUA PIECE
    LuaThread                  *mLuaThread;
    // END NEW LUA PIECE
};

} // Stress
} // Blaze

#endif // BLAZE_STRESS_FRANCHISEMODULE_H

