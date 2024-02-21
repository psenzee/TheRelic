/*************************************************************************************************/
/*!
    \file

    $Header: //blaze/games/Madden/2010-NG/trunk/component/franchise/stress/franchisemodule.cpp#60 $
    $Change: 46633 $
    $DateTime: 2009/06/19 13:41:13 $

    \attention
        (c) Electronic Arts Inc. 2009
*/
/*************************************************************************************************/

/*************************************************************************************************/
/*!
    \class FranchiseModule

    Stress tests franchise component.
    ---------------------------------------
    TODO.

*/
/*************************************************************************************************/

/*** Include Files *******************************************************************************/

#include "framework/blaze.h"
#include "blazerpclog.h"
#include "framework/config/config_file.h"
#include "franchisemodule.h"
#include "loginmanager.h"
#include "framework/connection/selector.h"
#include "framework/config/config_map.h"
#include "framework/config/config_map_base.h"
#include "framework/util/random.h"
#include "blazerpcerrors.h"
#include "framework/connection/selector.h"
#include "component/franchise/franchiseconstants.h"
#include "framework/util/shared/blazestring.h"
#include "EASTL/algorithm.h"
#include "EAStdC/EAString.h"

// NEW LUA PIECE
#include "luathread.h"
// END NEW LUA PIECE

namespace Blaze
{
namespace Stress
{

/*
 * List of available RPCs that can be called by the module.
 * Put these strings in the .cfg file; match each string
 * here to an "enum Action" value in the .h file.
 */
static const char8_t* ACTION_STRINGS[] =  {
    "noop",
    "createFranchise",
    "createDeleteFranchise",
    "deleteFranchise",
    "joinFranchise",
    "allGets",
    "indepthTest",
    "indepthTest2",
    "advanceWeek"
};

static const char8_t* METRIC_STRINGS[] =  {
    "getDraftableAthletes",
    "getDraftProfile",
    "setDraftProfile",
    "createFranchise",
    "joinFranchise",
    "getFranchise",
    "getFranchisesByUser",
    "deleteFranchise",
    "getMemberByBlazeId",
    "getMemberByTeamId",
    "findFranchises",
    "findFranchisesAsync",
    "removeMember",
    "updateFranchiseSettings",
    "postNews",
    "getNews",
    "clearBan",
    "sendInvitation",
    "getInvitations",
    "processInvitation",
    "proposeTrade",
    "processTrade",
    "getTrades",
    "getMembers",
    "joinDraft",
    "leaveDraft",
    "beginDraft",
    "pauseDraft",
    "draftAthlete",
    "getDraftOrder",
    "sendDraftChat",
    "getDraftHistory",
    "pollDraft",
    "checkDraft",
    "getDisplayRoster",
    "getGameplayRoster",
    "getDisplayAthlete",
    "getDepthChart",
    "setDepthChart",
    "generateDepthChart",
    "getRegularSchedule",
    "getPlayoffSchedule",
    "getGameplayInformation",
    "advanceFranchise",
    "simSeason",
    "athleteProgress",
    "getRawData",
    "signAthlete",
    "releaseAthlete",
    "getNumTeamInjuries",
    "getInjuriesByTeam",
    "getGameBoxScoreByGame",
    "getGameBoxScoresByWeek",
    "getGameBoxScoresByTeam",
    "getScoringSummary",
    "getTeamStatsByGame",
    "getAthleteStatsByGame",
    "promoteToGM",
    "getFranchiseRank",
    "getTopFranchises",
    "getSuperBowls",
    "submitFranchiseGameStats",
    "submitScoringSummary",
    "submitAthleteGameStats",
    "submitTeamGameStats",
    "validateRosters",
    "validateDepthCharts",
    "getCpuTrades",
    "getDisplayRosterByAthlete",
    "submitAntiCheatInfo",
    "setMemberStatus",
    "getRetirements",
    "getGameStatReport",
    "validateMatch",
    "getServerSettings",
    "setJerseyNumber"
};

uint32_t FranchiseInstance::mTestCount = 0;

static uint64_t gRpcAverages[FranchiseModule::NUM_METRICS] =
    { 20000, 10000, 10000, 400000, 15000, 5000, 15000,          // getDrAths - getfranbyuser
      600000, 6000, 10000, 20000, 0, 50000, 40000,              // delfr - updfrstgs
      40000, 20000, 15000, 40000, 20000, 20000,                 // postnws - procinvite
      20000, 30000, 5000, 35000, 20000, 2000, 2000, 10000,      // proptrde - pausedr
      50000, 8000, 20000, 6000, 8000, 3000, 10000, 40000, 7000, // drftath - getdsplath
      10000, 35000, 50000, 5000, 5000, 5000, 150000, 0, 20000,  // getdptcht - athprog
      40000, 80000, 120000, 4000, 4000, 4000, 4000, 4000,       // getraw - getgmbxscbytm
      4000, 4000, 20000, 7000, 5000, 3000, 5000, 5000, 5000,    // getscgsum - subscgsum
      5000, 35000, 100000, 2000, 4000, 10000, 40000, 50000,     // subathgmsts - setMemStatus
      6000, 12000, 10000, 10000, 10000                          // getRtrmts - setJrsyNum
};

/*** Defines/Macros/Constants/Typedefs ***********************************************************/

const int32_t    DEFAULT_RPC_DELAY     = 500;
const int32_t    DEFAULT_QUARTER_LEN   = 5;
const SkillLevel DEFAULT_SKILL_LEVEL   = SKILL_LEVEL_PRO;
const char8_t*   DEFAULT_PASSWORD      = "password";
const uint32_t   BUMP_COUNT_LIMIT      = 3;
const uint32_t   EXCEED_COUNT_LIMIT    = 4;
const uint32_t   DUMP_STATS2_INTERVAL  = (15 * 60);  // in seconds; this is 15 minutes

#define CallRpcIfAllowed(whichRpc, result, func, franId)  { if (mOwner->isRpcAllowed (whichRpc)) { result = func (franId); } }

#define PROCESS_ERROR(err, fn, rpc) \
    if (err != ERR_OK) { BLAZE_ERR (BlazeRpcLog::franchise, "*%d** "fn" failed:  Error = %s (%s)", \
                         mId, ErrorHelp::getErrorDescription(err), ErrorHelp::getErrorName(err)); \
                         mOwner->incRpcErrorCount(rpc); mOwner->incErrCount(); } // inc rpc count and total err count

#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) < (b) ? (a) : (b))

#ifdef _DEBUG
#define MYASSERT(x) assert(x)
#else
#define MYASSERT(x)
#endif

// Shamelessly stolen from logger.cpp.
// Note that this *declares* timestamp (ie, whatever name you pass in is declared as a char buffer).
#define CREATE_TIMESTAMP(timestamp) \
    char8_t timestamp[32]; \
    uint32_t year; \
    uint32_t month; \
    uint32_t day; \
    uint32_t hour; \
    uint32_t min; \
    uint32_t sec; \
    uint32_t msec; \
    TimeValue::getTimeComponents( \
            TimeValue::getTimeOfDay(), &year, &month, &day, &hour, &min, &sec, &msec); \
    blaze_snzprintf(timestamp, sizeof(timestamp), "%d/%02d/%02d-%02d:%02d:%02d.%03d", \
            year, month, day, hour, min, sec, msec);

/*** FranchiseInstance Implementation ***************************************************************/


/*** FranchiseModule Methods ******************************************************************************/

// static
StressModule* FranchiseModule::create()
{
    return new FranchiseModule();
}


FranchiseModule::FranchiseModule()
: mFutex(), mStatsEnabled(false), mLuaScript(NULL)
{
    mAction = ACTION_INVALID;
    mCreateFranchiseCount = 0;
    mRpcDelay = DEFAULT_RPC_DELAY; // default delay between RPC calls, in ms
    mNumConnections = 100;
    mBumpCountLimit = BUMP_COUNT_LIMIT;
    mExceedCountLimit = EXCEED_COUNT_LIMIT;
    mTimerId2 = INVALID_TIMER_ID;
    mAllowTurnOffs = false;
    mNumUsers = 0;
    mMaxUsers = 0;

    for (int i = 0; i < NUM_METRICS; ++i)
    {
        mMetricTime[i]  = 0;
        mMetricCount[i] = 0;
        mMetricMin[i]   = 0;
        mMetricMax[i]   = 0;
        mMetricAve[i]   = 0;

        mRpcData[i].startingAve = gRpcAverages[i];
        mRpcData[i].currentAve  = gRpcAverages[i];
        mRpcData[i].exceedCount = 0;
        mRpcData[i].bumpCount   = 0;
        mRpcData[i].allowRpc    = 1;    // means we allow the rpc to be called
        mRpcData[i].errorCount  = 0;
    }
}

FranchiseModule::~FranchiseModule()
{
    dumpStats2 ();
    gSelector->cancelTimer (mTimerId2); // call this AFTER dumpStats2
}

void FranchiseModule::dumpStats2 ()
{
    mFutex.Lock(); // try to prevent multiple entries at the same time; will this work w/ fibers?

    uint32_t totalRpcCalls = 0;
    uint32_t totalTestsStopped = 0;
    int64_t  testEndTime = TimeValue::getTimeOfDay().getMicroSeconds();

    CREATE_TIMESTAMP(timestamp);
    BLAZE_INFO(BlazeRpcLog::stats, "Date:  %s", timestamp);
    if (mStatsFileHandle != NULL)
    {
        fprintf(mStatsFileHandle, "\n\nDate:  %s\n",timestamp);
    }

    // Dump stat info; loop thru all metric enums (all RPCs):
    for (int32_t i=0; i<NUM_METRICS; ++i)
    {
        totalRpcCalls += (uint32_t)mMetricCount[i];
        if (mRpcData[i].allowRpc == 0)
        {
            totalTestsStopped++;
        }

        if (mStatsFileHandle != NULL)
        {
            fprintf(mStatsFileHandle, "\n%25s - Avg time: %12"PRIu64", Min time: %14"PRIu64", Max time: %10"PRIu64"\n",
                METRIC_STRINGS[i], mMetricAve[i], mMetricMin[i], mMetricMax[i]);
            fprintf(mStatsFileHandle, "%25s - Orig Tbl Ave: %8"PRIu64", Cur cmptd Ave: %9"PRIu64", Excd/Bmp/Allw  %d %d %d\n",
                METRIC_STRINGS[i], mRpcData[i].startingAve, mRpcData[i].currentAve, mRpcData[i].exceedCount,
                mRpcData[i].bumpCount, mRpcData[i].allowRpc);
            fprintf(mStatsFileHandle, "%25s - Num Errors: %10d, Num calls: %13"PRIu64"\n",
                METRIC_STRINGS[i], mRpcData[i].errorCount, mMetricCount[i]);
        }
        BLAZE_INFO(BlazeRpcLog::stats, "           [%s] Avg time: %"PRIu64", Min time: %"PRIu64", Max time: %"PRIu64"",
            METRIC_STRINGS[i], mMetricAve[i], mMetricMin[i], mMetricMax[i]);
        BLAZE_INFO(BlazeRpcLog::stats, "           [%s] Orig Tbl Ave: %"PRIu64", Cur cmptd Ave: %"PRIu64", Excd/Bmp/Allw %d %d %d",
            METRIC_STRINGS[i], mRpcData[i].startingAve, mRpcData[i].currentAve, mRpcData[i].exceedCount,
            mRpcData[i].bumpCount, mRpcData[i].allowRpc);
        BLAZE_INFO(BlazeRpcLog::stats, "           [%s] Num Errors: %d, Num calls: %"PRIu64"",
            METRIC_STRINGS[i], mRpcData[i].errorCount, mMetricCount[i]);
    }
    double testTimeInSecs = (double)(((double)(testEndTime - mTestStartTime)) / 1000000.0);

    if (mStatsFileHandle != NULL)
    {
        fprintf(mStatsFileHandle, "\n\nTotal tests run = %d, total RPC calls = %d, total RPCs stopped = %d\n",
            FranchiseInstance::mTestCount, totalRpcCalls, totalTestsStopped);
        fprintf(mStatsFileHandle, "Test run took = %.6g secs = %7.2g mins = %5.2g hrs\n",
            testTimeInSecs, testTimeInSecs/60.0, testTimeInSecs/3600.0);
        fprintf(mStatsFileHandle, "Total error count = %"PRIu64", ok logins = %d, pending logins = %d, failed logins = %d\n",
            mTotalErrCount, mSucceededLogins, mPendingLogins, mFailedLogins);
        fprintf(mStatsFileHandle, "Final average # RPC calls per second = %.6g\n", (double)totalRpcCalls / testTimeInSecs);
        fprintf(mStatsFileHandle, "------------------------------------------------------------------\n\n");
        fflush(mStatsFileHandle);
    }
    BLAZE_INFO(BlazeRpcLog::stats, "           Total tests run = %d, total RPC calls = %d, total RPCs stopped = %d",
        FranchiseInstance::mTestCount, totalRpcCalls, totalTestsStopped);
    BLAZE_INFO(BlazeRpcLog::stats, "           Test run took = %.6g secs = %7.2g mins = %5.2g hrs",
        testTimeInSecs, testTimeInSecs/60.0, testTimeInSecs/3600.0);
    BLAZE_INFO(BlazeRpcLog::stats, "           Total error count = %"PRIu64", ok logins = %d, pending logins = %d, failed logins = %d",
        mTotalErrCount, mSucceededLogins, mPendingLogins, mFailedLogins);
    BLAZE_INFO(BlazeRpcLog::stats, "           Final average # RPC calls per second = %.6g, max users at once = %d",
        (double)totalRpcCalls / testTimeInSecs, mMaxUsers);

    // Reschedule this function to get called again in the future.
    mTimerId2 = gSelector->scheduleTimerCall (TimeValue::getTimeOfDay()
            + (DUMP_STATS2_INTERVAL * 1000 * 1000), this, &FranchiseModule::dumpStats2);

    mFutex.Unlock();
}

// NEW LUA PIECE
LuaInterpreter *FranchiseModule::getLuaInterpreter()
{
    EA::Thread::AutoFutex lock(mFutex);
    if (mLuaInterpreterTls.GetValue() == NULL)
    {
        mLuaInterpreterTls.SetValue(createLuaInterpreter());
    }
    return (LuaInterpreter *)mLuaInterpreterTls.GetValue();
}
// END NEW LUA PIECE

//  Franchise Stress Test Configuration
//      Defines fields passed to Franchise RPCs invoked during a stress test.
//      Each Test uses a subset of the listed fields.
//
bool FranchiseModule::parseConfig(const ConfigMapBase& config)
{
    mCreateFranchiseCount = config.getUInt32("createFranchiseCount", 1);
    mBumpCountLimit = config.getUInt32("bumpCountLimit", BUMP_COUNT_LIMIT);
    mExceedCountLimit = config.getUInt32("exceedCountLimit", EXCEED_COUNT_LIMIT);
    mRpcDelay = config.getUInt32("delay", 500); // in ms
    mAllowTurnOffs = config.getBool("allowTurnOffs", false);

    BLAZE_DEBUG3(BlazeRpcLog::franchise, "FranchiseModule : parseConfig - franchise count = %d, delay = %d.", mCreateFranchiseCount, mRpcDelay);

    return true;
}

// NEW LUA PIECE
LuaInterpreter *FranchiseModule::createLuaInterpreter()
{
    LuaInterpreter *lua = NULL;
    if (mLuaScript != NULL)
    {
        lua = new LuaInterpreter;
        if (!lua->RunFile(mLuaScript))
        {
            delete lua;
            return NULL;
        }
    }
    return lua;
}
// END NEW LUA PIECE

bool FranchiseModule::initialize(const ConfigMapBase& config)
{
    BLAZE_INFO(BlazeRpcLog::franchise, "FranchiseModule : initialize - enter.");

    if (!StressModule::initialize(config))
        return false;

    if (!parseConfig(config))
        return false;

    // New stats reading code from PaulS.
    mStatsEnabled = config.getBool("enableStatsReading", 0);

    if (mStatsEnabled)
    {
        int         maximumStatsCsvRows = config.getInt32 ("maximumStatsCsvRows", 1000);
        const char *statsCsvPath        = config.getString("statsCsvPath",        "");
        StatsTables::GetInstance().ReadAll(statsCsvPath, maximumStatsCsvRows);
    }

    // NEW LUA PIECE
    mLuaScript = config.getString("luaScript", 0);
    BLAZE_INFO(Log::SYSTEM, "[FranchiseModule:%p]:: luaScript=%s.", this, mLuaScript);
    if (mLuaScript != NULL)
    {
        getLuaInterpreter(); // prime the lua pump

        mTimerId2 = gSelector->scheduleTimerCall (TimeValue::getTimeOfDay()
            + (DUMP_STATS2_INTERVAL * 1000 * 1000), this, &FranchiseModule::dumpStats2);
        return true;
    }
    // END NEW LUA PIECE

    const char8_t* action = config.getString("action", "");
    if (blaze_stricmp(action, ACTION_STRINGS[ACTION_NOOP]) == 0)
    {
        mAction = ACTION_NOOP;
    }
    else if (blaze_stricmp(action, ACTION_STRINGS[ACTION_CREATE_FRANCHISE]) == 0)
    {
        mAction = ACTION_CREATE_FRANCHISE;
    }
    else if (blaze_stricmp(action, ACTION_STRINGS[ACTION_ADVANCE_WEEK]) == 0)
    {
        mAction = ACTION_ADVANCE_WEEK;
    }
    else if (blaze_stricmp(action, ACTION_STRINGS[ACTION_CREATE_DELETE_FRANCHISE]) == 0)
    {
        mAction = ACTION_CREATE_DELETE_FRANCHISE;
    }
    else if (blaze_stricmp(action, ACTION_STRINGS[ACTION_DELETE_FRANCHISE]) == 0)
    {
        mAction = ACTION_DELETE_FRANCHISE;
    }
    else if (blaze_stricmp(action, ACTION_STRINGS[ACTION_JOIN_FRANCHISE]) == 0)
    {
        mAction = ACTION_JOIN_FRANCHISE;
    }
    else if (blaze_stricmp(action, ACTION_STRINGS[ACTION_INDEPTH_TEST2]) == 0)
    {
        mAction = ACTION_INDEPTH_TEST2;
    }
    else if (blaze_stricmp(action, ACTION_STRINGS[ACTION_ALL_GETS]) == 0)
    {
        mAction = ACTION_ALL_GETS;
    }
    else if (blaze_stricmp(action, ACTION_STRINGS[ACTION_INDEPTH_TEST]) == 0)
    {
        mAction = ACTION_INDEPTH_TEST;
    }
    else
    {
        BLAZE_ERR(BlazeRpcLog::franchise, "FranchiseModule : unrecognized action: '%s'", action);
        return false;
    }

    BLAZE_INFO(BlazeRpcLog::franchise, "FranchiseModule : %s action selected.", action);

    mTimerId2 = gSelector->scheduleTimerCall (TimeValue::getTimeOfDay()
            + (DUMP_STATS2_INTERVAL * 1000 * 1000), this, &FranchiseModule::dumpStats2);


    return true;
}

// input 'ms' is the amount of time it took to do a task, like call an rpc.
void FranchiseModule::addMetric(Metric metric, const TimeValue& ms)
{
    mFutex.Lock (); // try to prevent multiple entries at the same time; will this work w/ fibers?

    uint64_t newElapsedTime = ms.getMicroSeconds();
    if (newElapsedTime == 0LL)
    {
        // Do this so that the 'min' will only be set directly to newElapsedTime
        // once, and then from then on the min() function will be used. Otherwise,
        // if newElapsedTime goes to zero every now and then, the 'min' variable
        // will get reset to newElapsedTime on the next call, screwing it up.
        newElapsedTime++;
    }
    mMetricTime[metric] += newElapsedTime;
    mMetricCount[metric]++;

    // Make sure 'min' doesn't get stuck at 0 forever.
    if (mMetricMin[metric] == 0LL && newElapsedTime != 0LL)
    {
        mMetricMin[metric] = newElapsedTime;
    }
    else
    {
        mMetricMin[metric] = eastl::min<uint64_t>(mMetricMin[metric], newElapsedTime);
    }

    mMetricMax[metric] = eastl::max<uint64_t>(mMetricMax[metric], newElapsedTime);
    mMetricAve[metric] = mMetricTime[metric] / mMetricCount[metric];
    mMaxUsers = eastl::max<uint32_t>(mMaxUsers, mNumUsers);  // keep track of the max # users at any one point

    // If the new average is > 1.5 times the current high-mark average, then
    // check the bump count.  If we've bumped enough, don't bump anymore but
    // turn off the rpc.  Otherwise, check the exceed count to see how many
    // times we've exceeded the average by 1.5x.  If too many, bump the bump
    // count and bump the high-mark average; otherwise, just inc the exceed count.
    if (mMetricAve[metric] > (uint64_t)(mRpcData[metric].currentAve * 1.5))
    {
        if (mRpcData[metric].allowRpc)
        {
            if (mRpcData[metric].bumpCount > mBumpCountLimit)
            {
                if (mAllowTurnOffs)
                {
                    mRpcData[metric].allowRpc = false;  // turn off this rpc; it's taking too much time
                    BLAZE_INFO(BlazeRpcLog::stats, " ======== [%s] TURNING OFF THIS RPC", METRIC_STRINGS[metric]);
                }
            }
            else
            {
                if (mRpcData[metric].exceedCount > mExceedCountLimit)
                {
                    mRpcData[metric].exceedCount = 0;
                    mRpcData[metric].bumpCount++;
                    mRpcData[metric].currentAve = (uint64_t)(mRpcData[metric].currentAve * 1.5); // increase ave limit by 1.5x
                }
                else
                {
                    mRpcData[metric].exceedCount++;
                }
            }
        }
    }

    if ((mMetricCount[metric] % 100) == 0)
    {
        BLAZE_INFO(BlazeRpcLog::stats, "         [%s] Avg time: %"PRIu64", Min time: %"PRIu64", Max time: %"PRIu64"",
            METRIC_STRINGS[metric], mMetricAve[metric], mMetricMin[metric], mMetricMax[metric]);
    }
    mFutex.Unlock();
}



/*** FranchiseInstance Methods ******************************************************************************/

StressInstance* FranchiseModule::createInstance(StressConnection* connection, Login* login)
{
    static int32_t instanceId = 0;  // simple counter to keep track of all franchise instances created
    return new FranchiseInstance(this, connection, login, instanceId++);
}


FranchiseInstance::FranchiseInstance(FranchiseModule *owner, StressConnection* connection, Login* login, int32_t id) :
    StressInstance(owner, connection, login, BlazeRpcLog::franchise),
    mOwner(owner),
    mName("None"),
    mProxy(new FranchiseSlaveProxy(getConnection()->getAddress(), getConnection())),
    mTrialIndex(-1),
    mCreateFranchiseCount(0),
    mRpcDelay(500),
    mId(id),
    mBlazeId(0),
    mTeamId(0),
    mDraftMgrId(0),
    mDraftRound(0),
    mDraftSeqNum(0),
    mInviteeBlazeId(0),
    mWeAreGM(false),
    mDraftFranId(0),
    mCurrentFranData(NULL),
// NEW LUA PIECE
    mLuaThread(0)
// END NEW LUA PIECE
{
}


FranchiseInstance::~FranchiseInstance()
{
    delete mProxy;

    // Delete all AthleteData pointers in the mAthleteDataMap map.  They were allocated in getAthletesOnTeamInFranchise.
    athleteIdAthleteDataMapType::iterator iter = mAthleteDataMap.begin();
    for (; iter != mAthleteDataMap.end(); iter++)
    {
        AthleteData* data = iter->second;
        delete data;
    }

    // Delete all FranchiseIdData pointers in the mFranchiseIdData vector.  They were allocated in findAllFranchiseIds.
    FranchiseIdDataVectorType::iterator iterFrans = mFranchiseIdData.begin();
    for (; iterFrans != mFranchiseIdData.end(); iterFrans++)
    {
        FranchiseIdData* data = *iterFrans;
        MemberDataVectorType::reverse_iterator iterMembers = data->memberData.rbegin();
        for (; iterMembers != data->memberData.rend(); iterMembers++)
        {
            delete *iterMembers;
        }
        data->memberData.clear ();
        delete data;
    }

    mDraftableAthletesList.clear ();
}


void FranchiseInstance::start()
{
    StressInstance::start();
}


BlazeRpcError FranchiseInstance::execute()
{
    BlazeRpcError rpcResult = ERR_OK;
    mCreateFranchiseCount = mOwner->getCreateFranchiseCount ();
    mRpcDelay = mOwner->getDelay ();
    mOwner->incNumUsers();

    mTeamId = (TeamId) Blaze::Random::getRandomNumber(32); // get a random team number
    if (mTeamId == 0)
        mTeamId++; // 0 is invalid

    mBlazeId = getBlazeId (); // we'll need this

    BLAZE_INFO (BlazeRpcLog::franchise, "[%d] --------- START --------------------------- bzid=%d, tid=%d ", mId, mBlazeId, mTeamId);
    mTrialIndex++;

    // NEW LUA PIECE
    LuaInterpreter *interpreter = mOwner->getLuaInterpreter();
    // determine if using lua or actions
    if (interpreter != NULL && interpreter->GetState()) // if a state is created, then we're using lua
    {
        if (mLuaThread == NULL)
        {
            mLuaThread = interpreter->CreateThread(this);
        }
        mLuaThread->Execute("Main"); // for now, we're just executing Main, but I can see this being customizable with some stress tester architecture changes
        return ERR_OK;
    }
    // END NEW LUA PIECE

    switch (mOwner->getAction())
    {
    case FranchiseModule::ACTION_NOOP:
        {
            BLAZE_INFO(BlazeRpcLog::franchise, "[%d] execute: NO-OP TEST", mId);
            for (int32_t index = 0; index < mCreateFranchiseCount; ++index)
            {
                FranchiseId createdId = 0; // dummy code just to simulate doing something; mainly, we just want to loop
                createdId++;
            }
            shutdown();
        }
        break;

    case FranchiseModule::ACTION_ADVANCE_WEEK:
        {
            BLAZE_INFO(BlazeRpcLog::franchise, "[%d] execute: AdvanceWeek TEST", mId);
            getFranchisesByUserLocal (); // find all existing franchise ids for our blaze id, put them in a vector
            for (int32_t index = 0; index < mCreateFranchiseCount; ++index)
            {
                rpcResult = advanceWeekTest ();
            }
            shutdown();
        }
        break;

    case FranchiseModule::ACTION_CREATE_FRANCHISE:
        {
            BLAZE_INFO(BlazeRpcLog::franchise, "[%d] execute: (CREATE) START OF ACTION ON CLIENT", mId);
            for (int32_t index = 0; index < mCreateFranchiseCount; ++index)
            {
                FranchiseId createdId = 0;
                rpcResult = createFranchiseLocal(index, &createdId);
            }
            shutdown();
        }
        break;

    case FranchiseModule::ACTION_CREATE_DELETE_FRANCHISE:
        {
            BLAZE_INFO(BlazeRpcLog::franchise, "[%d] execute: (CREATE/DEL) START OF ACTION ON CLIENT", mId);
            for (int32_t index = 0; index < mCreateFranchiseCount; ++index)
            {
                // Create the franchise.
                FranchiseId createdId = 0;
                rpcResult = createFranchiseLocal(index, &createdId);

                if (rpcResult == FRANCHISE_ERR_FRANCHISE_NAME_IN_USE  || // ie, we created it before so it already exists; that's ok then
                    rpcResult == ERR_OK)
                {
                    BLAZE_INFO(BlazeRpcLog::franchise, "[%d] execute: (create/del): fran ID to delete = %d",
                        mId, createdId);

                    // Delete the found franchise.
                    rpcResult = deleteFranchiseLocal (createdId);
                }
            }
            shutdown();
        }
        break;

    case FranchiseModule::ACTION_JOIN_FRANCHISE:
        {
            BLAZE_INFO(BlazeRpcLog::franchise, "[%d] execute: (JOIN) START OF ACTION ON CLIENT, count=%d", mId, mCreateFranchiseCount);
            findAllFranchiseIds (); // find all existing franchise ids, put them in a vector
            for (int32_t index = 0; index < mCreateFranchiseCount; ++index)
            {
                // Join an existing franchise.
                FranchiseId tempId = getFranchiseWeAreNotAMemberOf ();
                getFranchiseLocal (tempId);
                rpcResult = joinFranchiseLocal (tempId);
            }
            shutdown();
        }
        break;

    // This test assumes we have complete control of the database, so we may be using any
    // franchise id we find.
    case FranchiseModule::ACTION_INDEPTH_TEST:
        {
            //BLAZE_INFO(BlazeRpcLog::franchise, "[%d] execute: (INDEPTH_TEST) START OF ACTION ON CLIENT, count=%d, mBlazeId=%d",
            //    mId, mCreateFranchiseCount, mBlazeId);
            FranchiseId franId = 0;
            bool        randomFranchise = false;

            // Find *all* existing franchise ids, put them in a vector; also calls getFranchisesByUserLocal.
            // Note that there may be ZERO franchises at this point, if we're just starting and no database
            // pre-population was done.
            findAllFranchiseIds ();

            for (int32_t index = 0; index < mCreateFranchiseCount; index++)
            {
                franId = 0;              // reset for each loop
                mTestCount++;            // absolute test counter for this instance; counts every test
                mWeAreGM = false;        // we're expecting to create a franchise
                randomFranchise = false; // whether we've picked a franchise at random, or one of ours

                BLAZE_DEBUG(BlazeRpcLog::franchise, "[%d] index=%d, mTestCount=%d", mId, index, mTestCount); // drl debug only

                // Create a franchise (every other time). First time here - create a franchise.
                if (((mTestCount+1) % 2) == 0)
                {
                    if (mOwner->isRpcAllowed (FranchiseModule::METRIC_CREATE_FRANCHISE))
                    {
                        rpcResult = createFranchiseLocal (index, &franId);
                        mWeAreGM  = true;
                    }
                    BLAZE_DEBUG(BlazeRpcLog::franchise, "[%d] Just called create franchise, franId=%d, err=%d", mId, franId, rpcResult); // drl debug only
                }

                if (franId == 0) // then grab a franchise id we've already created, if any
                {
                    franId = getFranchiseWeCreated ();
                    BLAZE_DEBUG(BlazeRpcLog::franchise, "[%d] Just called getFranchiseWeCreated, franId=%d", mId, franId); // drl debug only
                }

                if (franId == 0) // then get any franchise for the current user
                {
                    franId = getAnyFranchiseIdForUser (0); // ok for this test, which is defined as destructive
                    BLAZE_DEBUG(BlazeRpcLog::franchise, "[%d] Just called getAnyFranchiseIdForUser, franId=%d", mId, franId); // drl debug only
                }

                if (franId == 0) // if don't have one yet, we're desparate for a franchise id
                {
                    franId = getRandomFranchiseId ();
                    BLAZE_DEBUG(BlazeRpcLog::franchise, "[%d] Just called getRandomFranchiseId, franId=%d", mId, franId); // drl debug only
                    randomFranchise = true;
                }

                // Join a franchise (every 3rd time).
                if ((mTestCount % 3) == 0)
                {
                    FranchiseId tempId = getFranchiseWeAreNotAMemberOf ();
                    BLAZE_DEBUG(BlazeRpcLog::franchise, "[%d] Just called getFranchiseWeAreNotAMemberOf, tempId=%d", mId, tempId); // drl debug only
                    if (tempId && mOwner->isRpcAllowed (FranchiseModule::METRIC_JOIN_FRANCHISE))
                    {
                        rpcResult = joinFranchiseLocal (tempId);
                        BLAZE_DEBUG(BlazeRpcLog::franchise, "[%d] Just called joinFranchiseLocal, franId=%d, err=%d", mId, tempId, rpcResult); // drl debug only
                    }
                }

                BLAZE_INFO(BlazeRpcLog::franchise, "[%d] execute: TOP OF LOOP - IN DEPTH TEST, fran_id=%d, error=%d, index=%d, abs index=%d",
                    mId, franId, rpcResult, index, mTestCount);

                if (franId > 0 && !randomFranchise)
                {
                    // Franchise and member rpcs.  getMembersLocal called by findAllFranchiseIds above.
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_FRANCHISE, rpcResult, getFranchiseLocal, franId);
                    if (mWeAreGM)
                    {
                        CallRpcIfAllowed (FranchiseModule::METRIC_UPDATE_FRANCHISE_SETTINGS, rpcResult, updateFranchiseSettingsLocal, franId);
                    }
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_MEMBER_BY_BLAZE_ID, rpcResult, getMemberByBlazeIdLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_MEMBER_BY_TEAM_ID, rpcResult, getMemberByTeamIdLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_SET_MEMBER_STATUS, rpcResult, setMemberStatusLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_CLEAR_BAN, rpcResult, clearBanLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_REMOVE_MEMBER, rpcResult, removeMemberLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_PROMOTE_TO_GM, rpcResult, promoteToGMLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_VALIDATE_MATCH, rpcResult, validateMatchLocal, franId);

                    // More 'get' rpcs.
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_REGULAR_SCHEDULE, rpcResult, getRegularScheduleLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_PLAYOFF_SCHEDULE, rpcResult, getPlayoffScheduleLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_DISPLAY_ROSTER, rpcResult, getDisplayRosterLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_GAMEPLAY_ROSTER, rpcResult, getGameplayRosterLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_NUM_TEAM_INJURIES, rpcResult, getNumTeamInjuriesLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_GAME_BOX_SCORE_BY_GAME, rpcResult, getGameBoxScoreByGameLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_GAME_BOX_SCORES_BY_WEEK, rpcResult, getGameBoxScoresByWeekLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_GAME_BOX_SCORES_BY_TEAM, rpcResult, getGameBoxScoresByTeamLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_TEAM_STATS_BY_GAME, rpcResult, getTeamStatsByGameLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_ATHLETE_STATS_BY_GAME, rpcResult, getAthleteStatsByGameLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_SCORING_SUMMARY, rpcResult, getScoringSummaryLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_DISPLAY_ROSTER_BY_ATHLETE, rpcResult, getDisplayRosterByAthleteLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_CPU_TRADES, rpcResult, getCpuTradesLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_SUPER_BOWLS, rpcResult, getSuperbowlsLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_RAW_DATA, rpcResult, getRawDataLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_TOP_FRANCHISES, rpcResult, getTopFranchisesLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_FRANCHISE_RANK, rpcResult, getFranchiseRankLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_GAMEPLAY_INFORMATION, rpcResult, getGameplayInformationLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_RETIREMENTS, rpcResult, getRetirementsLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_GAME_STAT_REPORT, rpcResult, getGameStatReportLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_SERVER_SETTINGS, rpcResult, getServerSettingsLocal, franId);

                    // Depth charts and invitations.
                    CallRpcIfAllowed (FranchiseModule::METRIC_SET_DEPTH_CHART, rpcResult, setDepthChartLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_DEPTH_CHART, rpcResult, getDepthChartLocal, franId);
                    if ((mTestCount % 129) == 0) // do every 29th
                    {
                        CallRpcIfAllowed (FranchiseModule::METRIC_GENERATE_DEPTH_CHART, rpcResult, generateDepthChartLocal, franId);
                        CallRpcIfAllowed (FranchiseModule::METRIC_VALIDATE_DEPTH_CHARTS, rpcResult, validateDepthChartsLocal, franId);
                    }
                    CallRpcIfAllowed (FranchiseModule::METRIC_SEND_INVITATION, rpcResult, sendInvitationLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_INVITATIONS, rpcResult, getInvitationsLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_PROCESS_INVITATION, rpcResult, processInvitationLocal, franId);

                    // Drafts, part 1.  May need to advance the franchise state to draft.
                    if ((mTestCount % 125) == 0) // do every 25th
                    {
                        if (mOwner->isRpcAllowed (FranchiseModule::METRIC_ADVANCE_FRANCHISE)) { rpcResult = advanceFranchiseLocal (franId, FRANCHISE_STATE_REGULAR_SEASON); }
                    }
                    if ((mTestCount % 128) == 0) // do every 28th
                    {
                        if (mOwner->isRpcAllowed (FranchiseModule::METRIC_ADVANCE_FRANCHISE)) { rpcResult = advanceFranchiseLocal (franId, FRANCHISE_STATE_PLAYOFFS); }
                    }
                    CallRpcIfAllowed (FranchiseModule::METRIC_SET_DRAFT_PROFILE, rpcResult, setDraftProfileLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_DRAFT_PROFILE, rpcResult, getDraftProfileLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_DRAFT_HISTORY, rpcResult, getDraftHistoryLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_DRAFT_ORDER, rpcResult, getDraftOrderLocal, franId);

                    // Drafts, part 2.
                    if ((mTestCount % 159) == 0) // do every 159th
                    {
                        CallRpcIfAllowed (FranchiseModule::METRIC_GET_DRAFTABLE_ATHLETES, rpcResult, getDraftableAthletesLocal, franId);
                        CallRpcIfAllowed (FranchiseModule::METRIC_JOIN_DRAFT, rpcResult, joinDraftLocal, franId);
                        if (rpcResult == ERR_OK)
                        {
                            CallRpcIfAllowed (FranchiseModule::METRIC_CHECK_DRAFT, rpcResult, checkDraftLocal, mDraftFranId);
                            if (rpcResult == ERR_OK)
                            {
                                CallRpcIfAllowed (FranchiseModule::METRIC_BEGIN_DRAFT, rpcResult, beginDraftLocal, mDraftFranId);
                                if (rpcResult == ERR_OK)
                                {
                                    CallRpcIfAllowed (FranchiseModule::METRIC_PAUSE_DRAFT, rpcResult, pauseDraftLocal, mDraftFranId);
                                    CallRpcIfAllowed (FranchiseModule::METRIC_BEGIN_DRAFT, rpcResult, beginDraftLocal, mDraftFranId);
                                    CallRpcIfAllowed (FranchiseModule::METRIC_DRAFT_ATHLETE, rpcResult, draftAthleteLocal, mDraftFranId);
                                    CallRpcIfAllowed (FranchiseModule::METRIC_SEND_DRAFT_CHAT, rpcResult, sendDraftChatLocal, mDraftFranId);
                                    CallRpcIfAllowed (FranchiseModule::METRIC_POLL_DRAFT, rpcResult, pollDraftLocal, mDraftFranId);
                                    CallRpcIfAllowed (FranchiseModule::METRIC_LEAVE_DRAFT, rpcResult, leaveDraftLocal, mDraftFranId);
                                }
                            }
                        }
                    }

                    // Trades.  Can't do trades if we aren't GM, which we won't be if
                    // we joined but we will be if we created the franchise.
                    if (mWeAreGM && (mTestCount % 43) == 0)
                    {
                        FranchiseId tradeFranId = 0;
                        TradeId     tradeId = 0;
                        if (mOwner->isRpcAllowed (FranchiseModule::METRIC_PROPOSE_TRADE)) { rpcResult = proposeTradeLocal (tradeFranId); }
                        if (mOwner->isRpcAllowed (FranchiseModule::METRIC_GET_TRADES))    { rpcResult = getTradesLocal (tradeFranId, tradeId); }
                        if (mOwner->isRpcAllowed (FranchiseModule::METRIC_PROCESS_TRADE)) { processTradeLocal (tradeFranId, tradeId); }
                    }

                    // Submits.
                    CallRpcIfAllowed (FranchiseModule::METRIC_SUBMIT_FRANCHISE_GAME_STATS, rpcResult, submitFranchiseGameStatsLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_SUBMIT_SCORING_SUMMARY, rpcResult, submitScoringSummaryLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_SUBMIT_ATHLETE_GAME_STATS, rpcResult, submitAthleteGameStatsLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_SUBMIT_TEAM_GAME_STATS, rpcResult, submitTeamGameStatsLocal, franId);
                    if ((mTestCount % 15) == 0) // do every 15th
                    {
                        CallRpcIfAllowed (FranchiseModule::METRIC_SUBMIT_ANTI_CHEAT_INFO, rpcResult, submitAntiCheatInfoLocal, franId);
                    }

                    // News.
                    CallRpcIfAllowed (FranchiseModule::METRIC_POST_NEWS, rpcResult, postNewsLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_NEWS, rpcResult, getNewsLocal, franId);

                    // Misc.
                    CallRpcIfAllowed (FranchiseModule::METRIC_SET_JERSEY_NUMBER, rpcResult, setJerseyNumberLocal, franId);
                    if (mOwner->isRpcAllowed (FranchiseModule::METRIC_PROMOTE_TO_GM)) { rpcResult = promoteToGMLocal (franId); }
                    if ((mTestCount % 17) == 0) // do every 17th
                    {
                        CallRpcIfAllowed (FranchiseModule::METRIC_VALIDATE_ROSTERS, rpcResult, validateRostersLocal, franId);
                    }
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_DISPLAY_ATHLETE, rpcResult, getDisplayAthleteLocal, franId);
                    if ((mTestCount % 250) == 0) // do every 250
                    {
                        CallRpcIfAllowed (FranchiseModule::METRIC_ATHLETE_PROGRESS, rpcResult, athleteProgressLocal, franId); // LEAKY! RE-ENABLE AFTER MEM LEAK TEST COMPLETE; also takes 1.2 secs!
                    }
                    CallRpcIfAllowed (FranchiseModule::METRIC_RELEASE_ATHLETE, rpcResult, releaseAthleteLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_SIGN_ATHLETE, rpcResult, signAthleteLocal, franId);

                    // Don't call these too often.
                    if (mWeAreGM && (mTestCount % 150) == 0) // delete every 150th
                    {
                        CallRpcIfAllowed (FranchiseModule::METRIC_DELETE_FRANCHISE, rpcResult, deleteFranchiseLocal, franId);
                    }
                }
                else if (1) // random franchise id obtained, so just do 'get' functions
                {
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_FRANCHISE, rpcResult, getFranchiseLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_MEMBER_BY_TEAM_ID, rpcResult, getMemberByTeamIdLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_REGULAR_SCHEDULE, rpcResult, getRegularScheduleLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_PLAYOFF_SCHEDULE, rpcResult, getPlayoffScheduleLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_DISPLAY_ROSTER, rpcResult, getDisplayRosterLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_GAMEPLAY_ROSTER, rpcResult, getGameplayRosterLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_NUM_TEAM_INJURIES, rpcResult, getNumTeamInjuriesLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_GAME_BOX_SCORE_BY_GAME, rpcResult, getGameBoxScoreByGameLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_GAME_BOX_SCORES_BY_WEEK, rpcResult, getGameBoxScoresByWeekLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_GAME_BOX_SCORES_BY_TEAM, rpcResult, getGameBoxScoresByTeamLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_TEAM_STATS_BY_GAME, rpcResult, getTeamStatsByGameLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_ATHLETE_STATS_BY_GAME, rpcResult, getAthleteStatsByGameLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_SCORING_SUMMARY, rpcResult, getScoringSummaryLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_DISPLAY_ROSTER_BY_ATHLETE, rpcResult, getDisplayRosterByAthleteLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_CPU_TRADES, rpcResult, getCpuTradesLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_SUPER_BOWLS, rpcResult, getSuperbowlsLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_RAW_DATA, rpcResult, getRawDataLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_TOP_FRANCHISES, rpcResult, getTopFranchisesLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_FRANCHISE_RANK, rpcResult, getFranchiseRankLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_GAMEPLAY_INFORMATION, rpcResult, getGameplayInformationLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_RETIREMENTS, rpcResult, getRetirementsLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_GAME_STAT_REPORT, rpcResult, getGameStatReportLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_DEPTH_CHART, rpcResult, getDepthChartLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_INVITATIONS, rpcResult, getInvitationsLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_DRAFT_HISTORY, rpcResult, getDraftHistoryLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_DRAFT_ORDER, rpcResult, getDraftOrderLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_DRAFTABLE_ATHLETES, rpcResult, getDraftableAthletesLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_SERVER_SETTINGS, rpcResult, getServerSettingsLocal, franId);
                }
                EA::Thread::ThreadSleep(100); // experimental, so this app doesn't consume 100% of the cpu
                BLAZE_INFO(BlazeRpcLog::franchise, "[%d] execute: BOTTOM OF LOOP, fran_id=%d, rpcResult=%d, index=%d",
                    mId, franId, rpcResult, index);
            }
            shutdown();
            rpcResult = ERR_OK; // errors are captured via the CallRpcIfAllowed macro
        }
        break;

    case FranchiseModule::ACTION_DELETE_FRANCHISE:
        {
            BLAZE_INFO(BlazeRpcLog::franchise, "[%d] execute: START OF ACTION ON CLIENT, count=%d", mId, mCreateFranchiseCount);
            findAllFranchiseIds (); // find *all* existing franchise ids, put them in a vector; also calls getFranchisesByUserLocal

            FranchiseId createdId = 0;

            // Get a franchise id for the current blazeid.
            createdId = getAnyFranchiseIdForUser (0);
            //createdId = getRandomFranchiseId ();//getGMFranchiseIdForUser ();
            //size_t numFranchises = 0;
            //createdId = getRandomFranchiseId ();
            if (createdId != 0)
            {
                for (int32_t index = 0; index < mCreateFranchiseCount; ++index)
                {
                    //rpcResult = createFranchiseLocal(index, &createdId);
                    //FranchiseId createdId = getRandomFranchiseId (); // make sure we don't modify someone else's franchise
                    rpcResult = deleteFranchiseLocal(createdId);
                    //rpcResult = getNumTeamInjuriesLocal (createdId);
                    //rpcResult = advanceFranchiseLocal(createdId, FRANCHISE_STATE_PLAYOFFS);
                    //rpcResult = promoteToGMLocal(createdId);
                    //rpcResult = updateFranchiseSettingsLocal(createdId);
                    //rpcResult = joinDraftLocal(createdId); // need to set currseason in db = 1 for all franchises
                    //rpcResult = beginDraftLocal(createdId);
                    //rpcResult = pauseDraftLocal(createdId);
                    //rpcResult = beginDraftLocal(createdId);
                    //rpcResult = getDraftableAthletesLocal (createdId);
                    //rpcResult = draftAthleteLocal(createdId);
                    //rpcResult = sendDraftChatLocal(createdId); // need to call joinDraftLocal 1st
                    //rpcResult = pollDraftLocal(createdId);
                    //rpcResult = checkDraftLocal(createdId);
                    //rpcResult = setDraftProfileLocal(createdId);
                    //rpcResult = getDraftProfileLocal (createdId);
                    //rpcResult = getDraftHistoryLocal (createdId);
                    //rpcResult = getDraftOrderLocal (createdId);
                    //rpcResult = leaveDraftLocal(createdId);
                    //rpcResult = removeMemberLocal(createdId);
                    //rpcResult = clearBanLocal(createdId);
                    //rpcResult = postNewsLocal(createdId);
                    //rpcResult = sendInvitationLocal(createdId);
                    //rpcResult = processInvitationLocal(createdId);
                    //rpcResult = setDepthChartLocal(createdId);
                    //rpcResult = generateDepthChartLocal(createdId);
                    //rpcResult = validateRostersLocal(createdId);
                    //rpcResult = validateDepthChartsLocal(createdId);
                    //rpcResult = submitFranchiseGameStatsLocal(createdId);
                    //rpcResult = submitScoringSummaryLocal(createdId);
                    //rpcResult = submitAthleteGameStatsLocal(createdId);
                    //rpcResult = submitTeamGameStatsLocal(createdId);
                    //rpcResult = submitAntiCheatInfoLocal(createdId);
                    //rpcResult = simSeasonLocal(createdId);
                    //rpcResult = athleteProgressLocal(createdId);
                    //rpcResult = releaseAthleteLocal(createdId);
                    //rpcResult = signAthleteLocal(createdId);
                    //FranchiseId tradeFranId = 0;
                    //TradeId     tradeId = 0;
                    //rpcResult = proposeTradeLocal (tradeFranId);
                    //rpcResult = getTradesLocal (tradeFranId, tradeId);
                    //rpcResult = processTradeLocal (tradeFranId, tradeId);
                    //rpcResult = getRawDataLocal(createdId);
                    //rpcResult = getGameplayInformationLocal(createdId);
                    //rpcResult = setMemberStatusLocal(createdId);
                    if (rpcResult != ERR_OK)
                    {
                        BLAZE_ERR(BlazeRpcLog::franchise, "%d ----------- Error 0x%x", mId, rpcResult);
                    }
                }
            }
            shutdown();
        }
        break;

    // This test assumes that we can only write to those franchises we create, although we can read anything.
    // Assume this is someone else's blaze server and we don't want to kill it or impact it's database much.
    case FranchiseModule::ACTION_INDEPTH_TEST2:
        {
            //BLAZE_INFO(BlazeRpcLog::franchise, "[%d] execute: (INDEPTH_TEST2) START OF ACTION ON CLIENT, count=%d, mBlazeId=%d",
            //    mId, mCreateFranchiseCount, mBlazeId);
            FranchiseId franId = 0;

            // Find *all* existing franchise ids, put them in a vector; also calls getFranchisesByUserLocal.
            // Note that there may be ZERO franchises at this point, if we're just starting and no database
            // pre-population was done.
            findAllFranchiseIds ();

            for (int32_t index = 0; index < mCreateFranchiseCount; index++)
            {
                franId = 0;      // reset for each loop
                mTestCount++;    // absolute test counter for this instance; counts every test
                mWeAreGM = true; // we're expecting to create a franchise

                BLAZE_DEBUG(BlazeRpcLog::franchise, "[%d] index=%d, mTestCount=%d", mId, index, mTestCount); // drl debug only

                // Create a franchise (every other time). First time here - create a franchise.
                if (((mTestCount+1) % 2) == 0)
                {
                    if (mOwner->isRpcAllowed (FranchiseModule::METRIC_CREATE_FRANCHISE))
                    {
                        rpcResult = createFranchiseLocal (index, &franId);
                    }
                    BLAZE_DEBUG(BlazeRpcLog::franchise, "[%d] Just called create franchise, franId=%d, err=%d", mId, franId, rpcResult); // drl debug only
                    // If that failed, find a current franchise id and use that.
                    if (rpcResult != ERR_OK || franId == 0)
                    {
                        franId = getFranchiseWeCreated ();
                        BLAZE_DEBUG(BlazeRpcLog::franchise, "[%d] Just called getFranchiseWeCreated 1, franId=%d", mId, franId); // drl debug only
                    }
                }
                else // grab a franchise id we've already created, if any
                {
                    franId = getFranchiseWeCreated ();
                    BLAZE_DEBUG(BlazeRpcLog::franchise, "[%d] Just called getFranchiseWeCreated 2, franId=%d", mId, franId); // drl debug only
                }

                BLAZE_INFO(BlazeRpcLog::franchise, "[%d] execute: TOP OF LOOP - IN DEPTH2 TEST, fran_id=%d, error=%d, index=%d, abs index=%d",
                    mId, franId, rpcResult, index, mTestCount);


                if (franId > 0)
                {
                    // Franchise and member rpcs.  getMembersLocal called by findAllFranchiseIds above.
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_FRANCHISE, rpcResult, getFranchiseLocal, franId);
                    if (mWeAreGM)
                    {
                        CallRpcIfAllowed (FranchiseModule::METRIC_UPDATE_FRANCHISE_SETTINGS, rpcResult, updateFranchiseSettingsLocal, franId);
                    }
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_MEMBER_BY_BLAZE_ID, rpcResult, getMemberByBlazeIdLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_MEMBER_BY_TEAM_ID, rpcResult, getMemberByTeamIdLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_SET_MEMBER_STATUS, rpcResult, setMemberStatusLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_CLEAR_BAN, rpcResult, clearBanLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_REMOVE_MEMBER, rpcResult, removeMemberLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_VALIDATE_MATCH, rpcResult, validateMatchLocal, franId);

                    // Here, we can call promoteToGM if we promote one of our of users, and the franchise is ours also.
                    if (mOwner->isRpcAllowed (FranchiseModule::METRIC_PROMOTE_TO_GM)) { rpcResult = promoteToGMLocal (franId); }

                    // More 'get' rpcs.
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_REGULAR_SCHEDULE, rpcResult, getRegularScheduleLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_PLAYOFF_SCHEDULE, rpcResult, getPlayoffScheduleLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_DISPLAY_ROSTER, rpcResult, getDisplayRosterLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_GAMEPLAY_ROSTER, rpcResult, getGameplayRosterLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_NUM_TEAM_INJURIES, rpcResult, getNumTeamInjuriesLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_GAME_BOX_SCORE_BY_GAME, rpcResult, getGameBoxScoreByGameLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_GAME_BOX_SCORES_BY_WEEK, rpcResult, getGameBoxScoresByWeekLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_GAME_BOX_SCORES_BY_TEAM, rpcResult, getGameBoxScoresByTeamLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_TEAM_STATS_BY_GAME, rpcResult, getTeamStatsByGameLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_ATHLETE_STATS_BY_GAME, rpcResult, getAthleteStatsByGameLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_SCORING_SUMMARY, rpcResult, getScoringSummaryLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_DISPLAY_ROSTER_BY_ATHLETE, rpcResult, getDisplayRosterByAthleteLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_CPU_TRADES, rpcResult, getCpuTradesLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_SUPER_BOWLS, rpcResult, getSuperbowlsLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_RAW_DATA, rpcResult, getRawDataLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_TOP_FRANCHISES, rpcResult, getTopFranchisesLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_FRANCHISE_RANK, rpcResult, getFranchiseRankLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_GAMEPLAY_INFORMATION, rpcResult, getGameplayInformationLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_RETIREMENTS, rpcResult, getRetirementsLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_GAME_STAT_REPORT, rpcResult, getGameStatReportLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_SERVER_SETTINGS, rpcResult, getServerSettingsLocal, franId);

                    CallRpcIfAllowed (FranchiseModule::METRIC_SET_DEPTH_CHART, rpcResult, setDepthChartLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_DEPTH_CHART, rpcResult, getDepthChartLocal, franId);
                    if ((mTestCount % 29) == 0) // do every 29th
                    {
                        CallRpcIfAllowed (FranchiseModule::METRIC_GENERATE_DEPTH_CHART, rpcResult, generateDepthChartLocal, franId);
                        CallRpcIfAllowed (FranchiseModule::METRIC_VALIDATE_DEPTH_CHARTS, rpcResult, validateDepthChartsLocal, franId);
                    }
                    // Is sendInvitation allowable in this test?  I'd have to alter the code such that the other member
                    // that is being invited is one I created (todo).
                    if (mOwner->isRpcAllowed (FranchiseModule::METRIC_SEND_INVITATION))
                    {
                        rpcResult = sendInvitationLocal (franId, true);
                    }
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_INVITATIONS, rpcResult, getInvitationsLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_PROCESS_INVITATION, rpcResult, processInvitationLocal, franId);

                    if ((mTestCount % 25) == 0) // do every 25th
                    {
                        if (mOwner->isRpcAllowed (FranchiseModule::METRIC_ADVANCE_FRANCHISE)) { rpcResult = advanceFranchiseLocal (franId, FRANCHISE_STATE_REGULAR_SEASON); }
                    }
                    if ((mTestCount % 28) == 0) // do every 28th
                    {
                        if (mOwner->isRpcAllowed (FranchiseModule::METRIC_ADVANCE_FRANCHISE)) { rpcResult = advanceFranchiseLocal (franId, FRANCHISE_STATE_PLAYOFFS); }
                    }
                    CallRpcIfAllowed (FranchiseModule::METRIC_SET_DRAFT_PROFILE, rpcResult, setDraftProfileLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_DRAFT_PROFILE, rpcResult, getDraftProfileLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_DRAFT_HISTORY, rpcResult, getDraftHistoryLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_DRAFT_ORDER, rpcResult, getDraftOrderLocal, franId);

                    //rpcResult = getDraftableAthletesLocal (createdId);
                    //rpcResult = checkDraftLocal (createdId);
                    //rpcResult = joinDraftLocal (createdId);
                    //rpcResult = beginDraftLocal (createdId);
                    //rpcResult = pauseDraftLocal (createdId);
                    //rpcResult = draftAthleteLocal (createdId);
                    //rpcResult = sendDraftChatLocal (createdId);
                    //rpcResult = pollDraftLocal (createdId);
                    //rpcResult = leaveDraftLocal (createdId);

                    // Trades.  Can't do trades if we aren't GM, which we will be if we created the franchise.
                    BLAZE_DEBUG(BlazeRpcLog::franchise, "[%d] ABOUT TO DO TRADES, mWeAreGM=%d", mId, mWeAreGM); // drl debug only
                    if (mWeAreGM && (mTestCount % 43) == 0)
                    {
                        FranchiseId tradeFranId = 0;
                        TradeId     tradeId = 0;
                        if (mOwner->isRpcAllowed (FranchiseModule::METRIC_PROPOSE_TRADE)) { rpcResult = proposeTradeLocal (tradeFranId); }
                        if (mOwner->isRpcAllowed (FranchiseModule::METRIC_GET_TRADES))    { rpcResult = getTradesLocal (tradeFranId, tradeId); }
                        if (mOwner->isRpcAllowed (FranchiseModule::METRIC_PROCESS_TRADE)) { processTradeLocal (tradeFranId, tradeId); }
                    }

                    // Submits.
                    CallRpcIfAllowed (FranchiseModule::METRIC_SUBMIT_FRANCHISE_GAME_STATS, rpcResult, submitFranchiseGameStatsLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_SUBMIT_SCORING_SUMMARY, rpcResult, submitScoringSummaryLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_SUBMIT_ATHLETE_GAME_STATS, rpcResult, submitAthleteGameStatsLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_SUBMIT_TEAM_GAME_STATS, rpcResult, submitTeamGameStatsLocal, franId);
                    if ((mTestCount % 5) == 0) // do every 5th
                    {
                        CallRpcIfAllowed (FranchiseModule::METRIC_SUBMIT_ANTI_CHEAT_INFO, rpcResult, submitAntiCheatInfoLocal, franId);
                    }

                    // News.
                    CallRpcIfAllowed (FranchiseModule::METRIC_POST_NEWS, rpcResult, postNewsLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_NEWS, rpcResult, getNewsLocal, franId);

                    // Misc.
                    CallRpcIfAllowed (FranchiseModule::METRIC_SET_JERSEY_NUMBER, rpcResult, setJerseyNumberLocal, franId);
                    //rpcResult = simSeasonLocal (createdId); // this may take too long to run, or cause other problems
                    if ((mTestCount % 7) == 0) // do every 7th
                    {
                        CallRpcIfAllowed (FranchiseModule::METRIC_VALIDATE_ROSTERS, rpcResult, validateRostersLocal, franId);
                    }
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_DISPLAY_ATHLETE, rpcResult, getDisplayAthleteLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_RELEASE_ATHLETE, rpcResult, releaseAthleteLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_SIGN_ATHLETE, rpcResult, signAthleteLocal, franId);

                    // Don't call these too often.
                    if (mWeAreGM && (mTestCount % 50) == 0) // delete every 50th
                    {
                        CallRpcIfAllowed (FranchiseModule::METRIC_DELETE_FRANCHISE, rpcResult, deleteFranchiseLocal, franId);
                    }
                }
                EA::Thread::ThreadSleep(100); // experimental, so this app doesn't consume 100% of the cpu
                BLAZE_INFO(BlazeRpcLog::franchise, "[%d] execute: BOTTOM OF LOOP, fran_id=%d, rpcResult=%d, index=%d",
                    mId, franId, rpcResult, index);
            }
            shutdown();
            rpcResult = ERR_OK; // errors are captured via the CallRpcIfAllowed macro
        }
        break;

    case FranchiseModule::ACTION_ALL_GETS:
        {
            //BLAZE_INFO(BlazeRpcLog::franchise, "[%d] execute: (GETS_TEST) START OF ACTION ON CLIENT, count=%d, mBlazeId=%d",
            //    mId, mCreateFranchiseCount, mBlazeId);
            FranchiseId franId = 0;

            // Find *all* existing franchise ids, put them in a vector; also calls getFranchisesByUserLocal.
            // Note that there may be ZERO franchises at this point, if we're just starting and no database
            // pre-population was done.
            findAllFranchiseIds ();

            for (int32_t index = 0; index < mCreateFranchiseCount; ++index)
            {
                mTestCount++;     // absolute test counter for this instance; counts every test

                franId = getRandomFranchiseId (); // make sure we don't modify someone else's franchise
                BLAZE_INFO(BlazeRpcLog::franchise, "[%d] execute: TOP OF LOOP, fran_id=%d, index=%d", mId, franId, index);

                if (franId > 0)
                {
                    // Call some other functions that just retrieve values.
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_FRANCHISE, rpcResult, getFranchiseLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_MEMBER_BY_BLAZE_ID, rpcResult, getMemberByBlazeIdLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_MEMBER_BY_TEAM_ID, rpcResult, getMemberByTeamIdLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_VALIDATE_MATCH, rpcResult, validateMatchLocal, franId);

                    // More 'get' rpcs.
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_REGULAR_SCHEDULE, rpcResult, getRegularScheduleLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_PLAYOFF_SCHEDULE, rpcResult, getPlayoffScheduleLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_DISPLAY_ROSTER, rpcResult, getDisplayRosterLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_GAMEPLAY_ROSTER, rpcResult, getGameplayRosterLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_NUM_TEAM_INJURIES, rpcResult, getNumTeamInjuriesLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_GAME_BOX_SCORE_BY_GAME, rpcResult, getGameBoxScoreByGameLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_GAME_BOX_SCORES_BY_WEEK, rpcResult, getGameBoxScoresByWeekLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_GAME_BOX_SCORES_BY_TEAM, rpcResult, getGameBoxScoresByTeamLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_TEAM_STATS_BY_GAME, rpcResult, getTeamStatsByGameLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_ATHLETE_STATS_BY_GAME, rpcResult, getAthleteStatsByGameLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_SCORING_SUMMARY, rpcResult, getScoringSummaryLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_DISPLAY_ROSTER_BY_ATHLETE, rpcResult, getDisplayRosterByAthleteLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_CPU_TRADES, rpcResult, getCpuTradesLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_SUPER_BOWLS, rpcResult, getSuperbowlsLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_RAW_DATA, rpcResult, getRawDataLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_TOP_FRANCHISES, rpcResult, getTopFranchisesLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_FRANCHISE_RANK, rpcResult, getFranchiseRankLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_GAMEPLAY_INFORMATION, rpcResult, getGameplayInformationLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_RETIREMENTS, rpcResult, getRetirementsLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_GAME_STAT_REPORT, rpcResult, getGameStatReportLocal, franId);

                    // Even more 'get' rpcs.
                    TradeId     tradeId = 0;
                    if (mOwner->isRpcAllowed (FranchiseModule::METRIC_GET_TRADES))    { rpcResult = getTradesLocal (franId, tradeId); }

                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_DISPLAY_ATHLETE, rpcResult, getDisplayAthleteLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_DEPTH_CHART, rpcResult, getDepthChartLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_NEWS, rpcResult, getNewsLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_INVITATIONS, rpcResult, getInvitationsLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_DRAFT_PROFILE, rpcResult, getDraftProfileLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_DRAFT_HISTORY, rpcResult, getDraftHistoryLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_DRAFT_ORDER, rpcResult, getDraftOrderLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_DRAFTABLE_ATHLETES, rpcResult, getDraftableAthletesLocal, franId);
                    CallRpcIfAllowed (FranchiseModule::METRIC_GET_SERVER_SETTINGS, rpcResult, getServerSettingsLocal, franId);
                }
                EA::Thread::ThreadSleep(50); // experimental, so this app doesn't consume 100% of the cpu
            }
            shutdown();
        }
        break;

    default:
        BLAZE_ERR(BlazeRpcLog::franchise, "[%d] execute: invalid action (%d) specified. ------------", mId, mOwner->getAction());
    }

    mOwner->decNumUsers();

    return rpcResult;
}

// Helper function.  We'll need our blaze id at certain times.
BlazeId FranchiseInstance::getBlazeId ()
{
    Login* login = getLogin ();
    const Authentication::SessionInfo* sessionInfo = login->getSessionInfo ();
    return sessionInfo->getBlazeUserId ();
}

BlazeRpcError FranchiseInstance::createFranchiseLocal (int32_t indexOfFranchise, FranchiseId* returnedCreatedFranId)
{
    BLAZE_INFO (BlazeRpcLog::franchise, "[%d] Start of createFranchise test", mId);

    if (returnedCreatedFranId)
    {
        *returnedCreatedFranId = 0; // init
    }

    char str[64];
    BlazeRpcError rpcResult = ERR_OK;
    FranchiseId createdFranId = 0;
    Franchise::CreateFranchiseRequest  req;
    Franchise::CreateFranchiseResponse resp;
    TimeValue localTime = TimeValue::getTimeOfDay ();
    int32_t microSeconds = (int32_t)(localTime.getMicroSeconds () & 0x1ffff);
    bool fantasyDraft = ((microSeconds % 2) == 0) ? false : true; // generate random boolean

    // Set up all the parameters for a franchise.
    blaze_snzprintf(str, sizeof(str), "Fran%06d_%03d_%05d",
        mId, indexOfFranchise, microSeconds); //Blaze::Random::getRandomNumber(RAND_MAX));
    req.setName(str);
    req.setVersion(FRANCHISE_VERSION_FRANCHISE);
    blaze_snzprintf(str, sizeof(str), "Fr%03d", indexOfFranchise);
    req.setAbbrev(str);
    blaze_snzprintf(str, sizeof(str), "Stress test created franchise %03d", indexOfFranchise);
    req.setDescription(str);
    req.setPassword(DEFAULT_PASSWORD);
    req.setMaxHumans(32);
    //blaze_snzprintf(str, sizeof(str), "Roster%03d", Blaze::Random::getRandomNumber(15)+1); // 1 thru 15
    req.setFantasyDraftEnabled(fantasyDraft);
    req.setTradeType(FRANCHISE_TRADES_ENABLED_ALL);
    req.setSalaryCapEnabled(1);
    req.setInjuriesEnabled(1);
    req.setWeatherType(WEATHER_TYPE_RANDOM);
    req.setQuarterLength(DEFAULT_QUARTER_LEN);
    req.setSkillLevel(DEFAULT_SKILL_LEVEL);
    req.setMaxDNF(50);       // ??
    req.setLogo(1);          // ??
    req.setCreatorTeamId(mTeamId);

    TimeValue startTime = TimeValue::getTimeOfDay();
    rpcResult = mProxy->createFranchise(&req, &resp); // CALL SERVER
    TimeValue endTime = TimeValue::getTimeOfDay();

    mOwner->addMetric(FranchiseModule::METRIC_CREATE_FRANCHISE, endTime - startTime);
    PROCESS_ERROR (rpcResult, "createFranchiseLocal", FranchiseModule::METRIC_CREATE_FRANCHISE);

    if (rpcResult == ERR_OK)
    {
        createdFranId = resp.getFranchiseId();
        if (returnedCreatedFranId)
        {
            *returnedCreatedFranId = createdFranId;
        }
        FranchiseIdData* fidData  = new FranchiseIdData;
        fidData->fid              = createdFranId;
        fidData->gmId        = mBlazeId;
        fidData->weCreatedIt      = true;
        fidData->weJoinedIt       = false;  // only set to true in joinFranchiseLocal
        fidData->fantasyDraftIsOn = fantasyDraft;
        fidData->state            = FRANCHISE_STATE_NONE;
        fidData->seasonId         = 0;
        fidData->windowId         = 0;
        mFranchiseIdData.push_back (fidData);
        BLAZE_INFO(BlazeRpcLog::franchise, "[%d] createFranchiseLocal: franchise %d created.", mId, createdFranId);
    }
    else
    {
        BLAZE_ERR(BlazeRpcLog::franchise, "[%d] createFranchiseLocal: franchise NOT created, err=%s",
            mId, ErrorHelp::getErrorName(rpcResult));
    }
    gSelector->sleep (mRpcDelay * 1000 * 3);  // arg is in microsecs; give extra time; this is a slow task

    // If we created a new franchise, find all athletes on all teams in the franchise and store the
    // data in structs pointed to by a map.
    if (rpcResult == ERR_OK)
    {
        getMembersLocal (createdFranId);      // populate the member data for this franchise
        validateRostersLocal (createdFranId); // have to do this to update the m_v_a_p table w/ all {aid,tmid} data for all new franchises
        for (TeamId i=1; i <= 32; i++)
        {
            rpcResult = getAthletesOnTeamInFranchise (createdFranId, i);
        }
        rpcResult = getAthletesOnTeamInFranchise (createdFranId, 1009); // get all athletes on the free agent team
        rpcResult = getAthletesOnTeamInFranchise (createdFranId, 100);  // get all athletes on the draft team
    }

    return rpcResult;
}


BlazeRpcError FranchiseInstance::deleteFranchiseLocal (FranchiseId franId)
{
    BLAZE_INFO (BlazeRpcLog::franchise, "[%d] Start of deleteFranchise test, fran to delete = %d", mId, franId);

    BlazeRpcError rpcResult = ERR_OK;
    Franchise::DeleteFranchiseRequest req;

    // Set up all the parameters to delete a franchise.
    req.setFranchiseId (franId);

    TimeValue startTime = TimeValue::getTimeOfDay();
    rpcResult = mProxy->deleteFranchise(&req);        // CALL SERVER
    TimeValue endTime = TimeValue::getTimeOfDay();

    mOwner->addMetric(FranchiseModule::METRIC_DELETE_FRANCHISE, endTime - startTime);
    PROCESS_ERROR (rpcResult, "deleteFranchiseLocal", FranchiseModule::METRIC_DELETE_FRANCHISE);

    if (rpcResult == ERR_OK)
    {
        BLAZE_INFO(BlazeRpcLog::franchise, "[%d] deleteFranchiseLocal: franchise %d deleted.", mId, franId);

        // Find the franchise in our vector of franchise data and delete it.
        eastl::vector<FranchiseIdData*>::iterator iter = mFranchiseIdData.begin();
        for (; iter != mFranchiseIdData.end(); iter++)
        {
            if ((*iter)->fid == franId)
            {
                FranchiseIdData* data = *iter;
                MemberDataVectorType::reverse_iterator iterMembers = data->memberData.rbegin();
                for (; iterMembers != data->memberData.rend(); iterMembers++)
                {
                    delete *iterMembers;
                }
                data->memberData.clear ();
                delete data;
                mFranchiseIdData.erase (iter);
                break;
            }
        }
    }
    else
    {
        BLAZE_ERR(BlazeRpcLog::franchise, "[%d] deleteFranchiseLocal: franchise %d NOT deleted, err=%s",
            mId, franId, ErrorHelp::getErrorName(rpcResult));
    }
    gSelector->sleep (mRpcDelay * 1000 * 2); // give it some extra time; arg is in microsecs

    return rpcResult;
}

BlazeRpcError FranchiseInstance::joinFranchiseLocal (FranchiseId franId)
{
    BlazeRpcError rpcResult = ERR_OK;
    JoinFranchiseRequest joinReq;

    BLAZE_INFO (BlazeRpcLog::franchise, "[%d] Start of joinFranchises test, fid=%d", mId, franId);

    joinReq.setFranchiseId (franId);
    joinReq.setPassword (DEFAULT_PASSWORD);
    joinReq.setTeamId ((TeamId)Blaze::Random::getRandomNumber(31)+1); // fn rtns 0-based number, so bump up 1
    TimeValue startTime = TimeValue::getTimeOfDay();
    rpcResult = mProxy->joinFranchise (&joinReq);
    TimeValue endTime = TimeValue::getTimeOfDay();
    mOwner->addMetric(FranchiseModule::METRIC_JOIN_FRANCHISE, endTime - startTime);
    PROCESS_ERROR (rpcResult, "joinFranchiseLocal", FranchiseModule::METRIC_JOIN_FRANCHISE);

    if (rpcResult == ERR_OK)
    {
        BLAZE_INFO (BlazeRpcLog::franchise, "[%d] Joined franchise = %d", mId, franId);
        FranchiseIdData* fidData;
        fidData = findFidDataFromFranId (franId); // we just want to set the weJoinedIt flag
        MYASSERT(fidData); // is it legal for this to be NULL?
        fidData->weJoinedIt = true;

        // Update member data, now that we've altered it.
        getMembersLocal (franId);
    }
    else if (rpcResult == FRANCHISE_ERR_ALREADY_A_MEMBER)
    {
        BLAZE_INFO(BlazeRpcLog::franchise, "[%d] Join franchise test: already a member of franchise = %d", mId, franId);
    }
    else if (rpcResult == FRANCHISE_ERR_TEAM_IN_USE)
    {
        BLAZE_INFO(BlazeRpcLog::franchise, "[%d] Join franchise test: team id = 3 is already in use for franchise %d", mId, franId);
    }
    else if (rpcResult == FRANCHISE_ERR_FRANCHISE_FULL)
    {
        BLAZE_INFO(BlazeRpcLog::franchise, "[%d] Join franchise test: franchise = %d already has too many members", mId, franId);
    }
    else
    {
        BLAZE_ERR(BlazeRpcLog::franchise, "[%d] Join franchise test: COULD NOT join franchise = %d, err = %s",
            mId, franId, ErrorHelp::getErrorName(rpcResult));
    }
    gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs; give findFranchises time to run
    return rpcResult;
}


BlazeRpcError FranchiseInstance::getFranchisesByUserLocal()
{
    BlazeRpcError rpcResult = ERR_OK;
    Franchise::FindFranchisesResponse resp;
    int32_t count = 0;

    BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] Start of getFranchisesByUser test, bzid=%d", mId, mBlazeId);
    TimeValue startTime = TimeValue::getTimeOfDay();
    rpcResult = mProxy->getFranchisesByUser (&resp);
    TimeValue endTime = TimeValue::getTimeOfDay();
    mOwner->addMetric(FranchiseModule::METRIC_GET_FRANCHISES_BY_USER, endTime - startTime);
    PROCESS_ERROR (rpcResult, "getFranchisesByUserLocal", FranchiseModule::METRIC_GET_FRANCHISES_BY_USER);

    if (rpcResult == ERR_OK)
    {
        // Iterate thru all franchises that *are* ours and add them to our vector of franchise data.
        FindFranchisesResponse::FranchisesList::const_iterator franchisesItr = resp.getFranchises()->begin();
        FindFranchisesResponse::FranchisesList::const_iterator franchisesItrEnd = resp.getFranchises()->end();

        // Loop thru all franchises found.
        for (; franchisesItr != franchisesItrEnd; franchisesItr++)
        {
            // getFranchisesByUser will return multiple instances of the same franchise if there are
            // multiple instances of that franchise in the members table for a particular member id.
            // We only need one franchise entry, so check to see if we've already covered it.
            if (!isFranchiseInFranchiseVector ((*franchisesItr)->getFranchiseId()))
            {
                FranchiseIdData* fidData  = new FranchiseIdData;
                fidData->fid              = (*franchisesItr)->getFranchiseId();
                fidData->gmId             = (*franchisesItr)->getGM()->getBlazeId();
                fidData->weCreatedIt      = false;  // only set to true in createFranchiseLocal
                fidData->weJoinedIt       = false;  // only set to true in joinFranchiseLocal
                fidData->fantasyDraftIsOn = (*franchisesItr)->getFantasyDraftEnabled() == 0 ? false : true;
                fidData->state            = (*franchisesItr)->getState ();
                fidData->seasonId         = (*franchisesItr)->getCurrSeason ();
                fidData->windowId         = (*franchisesItr)->getCurrWindow ();
                mRosterId                 = (*franchisesItr)->getRosterId (); // keep the last one we find
                mFranchiseIdData.push_back (fidData);

                // Find all members for this franchise and populate the data structures.
                getMembersLocal (fidData->fid);
                count++;
            }
        }
    }
    BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] getFranchisesByUser - found %d unique franchises for bzid=%d", mId, count, mBlazeId);

    gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs

    return rpcResult;
}

BlazeRpcError FranchiseInstance::getFranchiseLocal(FranchiseId franId)
{
    BlazeRpcError rpcResult = ERR_OK;
    Franchise::GetFranchiseRequest req;
    Franchise::Franchise resp;

    req.setFranchiseId (franId);

    BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] Start of getFranchise test, fid=%d", mId, franId);
    TimeValue startTime = TimeValue::getTimeOfDay();
    rpcResult = mProxy->getFranchise (&req, &resp);
    TimeValue endTime = TimeValue::getTimeOfDay();
    mOwner->addMetric(FranchiseModule::METRIC_GET_FRANCHISE, endTime - startTime);
    PROCESS_ERROR (rpcResult, "getFranchiseLocal", FranchiseModule::METRIC_GET_FRANCHISE);
    gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs
    // Grab the roster id from the response data.
    if (rpcResult == ERR_OK)
    {
        mRosterId = resp.getRosterId ();
    }
    return rpcResult;
}

BlazeRpcError FranchiseInstance::getFranchise (Franchise::GetFranchiseRequest& req, Franchise::Franchise& resp)
{
    BlazeRpcError rpcResult = ERR_OK;

    BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] Start of getFranchise test, fid=%d", mId, req.getFranchiseId());
    TimeValue startTime = TimeValue::getTimeOfDay();
    rpcResult = mProxy->getFranchise (&req, &resp);
    TimeValue endTime = TimeValue::getTimeOfDay();
    mOwner->addMetric(FranchiseModule::METRIC_GET_FRANCHISE, endTime - startTime);
    PROCESS_ERROR (rpcResult, "getFranchise", FranchiseModule::METRIC_GET_FRANCHISE);
    gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs
    // Grab the roster id from the response data.
    if (rpcResult == ERR_OK)
    {
        mRosterId = resp.getRosterId ();
    }

    return rpcResult;
}

BlazeRpcError FranchiseInstance::getMemberByBlazeIdLocal(FranchiseId franId)
{
    BlazeRpcError rpcResult = ERR_OK;
    Franchise::GetMemberByBlazeIdRequest req;
    Franchise::MemberInfo resp;

    req.setBlazeId (mBlazeId);
    req.setFranchiseId (franId); // rpc only works if bzid is a member of the franchise we pass

    BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] Start of getMemberByBlazeId test, fid=%d", mId, franId);
    TimeValue startTime = TimeValue::getTimeOfDay();
    rpcResult = mProxy->getMemberByBlazeId (&req, &resp);
    TimeValue endTime = TimeValue::getTimeOfDay();
    mOwner->addMetric(FranchiseModule::METRIC_GET_MEMBER_BY_BLAZE_ID, endTime - startTime);
    PROCESS_ERROR (rpcResult, "getMemberByBlazeIdLocal", FranchiseModule::METRIC_GET_MEMBER_BY_BLAZE_ID);
    gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs

    return rpcResult;
}

BlazeRpcError FranchiseInstance::getMemberByTeamIdLocal(FranchiseId franId)
{
    BlazeRpcError rpcResult = ERR_OK;
    Franchise::GetMemberByTeamIdRequest req;
    Franchise::MemberInfo resp;

    req.setTeamId (mTeamId);
    req.setFranchiseId (franId);

    BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] Start of getMemberByTeamId test, fid=%d", mId, franId);
    TimeValue startTime = TimeValue::getTimeOfDay();
    rpcResult = mProxy->getMemberByTeamId (&req, &resp);
    TimeValue endTime = TimeValue::getTimeOfDay();
    mOwner->addMetric(FranchiseModule::METRIC_GET_MEMBER_BY_TEAM_ID, endTime - startTime);
    PROCESS_ERROR (rpcResult, "getMemberByTeamIdLocal", FranchiseModule::METRIC_GET_MEMBER_BY_TEAM_ID);
    gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs

    return rpcResult;
}


BlazeRpcError FranchiseInstance::getMembersLocal(FranchiseId franId)
{
    BlazeRpcError rpcResult = ERR_OK;
    Franchise::GetMembersRequest req;
    Franchise::GetMembersResponse resp;
    BlazeId currentGM = 0;

    req.setFranchiseId (franId);

    BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] Start of getMembers test, fid=%d", mId, franId);
    TimeValue startTime = TimeValue::getTimeOfDay();
    rpcResult = mProxy->getMembers (&req, &resp);
    TimeValue endTime = TimeValue::getTimeOfDay();
    mOwner->addMetric(FranchiseModule::METRIC_GET_MEMBERS, endTime - startTime);
    PROCESS_ERROR (rpcResult, "getMembersLocal", FranchiseModule::METRIC_GET_MEMBERS);

    if (rpcResult == ERR_OK)
    {
        Franchise::GetMembersResponse::MemberInfoList* list = resp.getMemberList ();
        Franchise::GetMembersResponse::MemberInfoList::const_iterator iter = list->begin ();
        Franchise::GetMembersResponse::MemberInfoList::const_iterator iterEnd = list->end ();

        clearMemberDataForFranchise (franId);

        // For all members in this franchise, create a member struct and store it away with it's franchise data.
        for (; iter != iterEnd; iter++)
        {
            // Create a MemberData per user found here; put in vector of members.
            MemberData* member = new MemberData;
            member->bzid = (*iter)->getBlazeId ();
            member->tid  = (*iter)->getTeamId ();
            member->fid  = (*iter)->getFranchiseId ();
            member->isGM = (*iter)->getIsGM () ? true : false;
            EA::StdC::Strcpy (member->persona, (*iter)->getPersona ());
    
            //printf ("[%d] fid=%d, tid=%d, bzid=%d, MEMBER=%s\n\r",
            //    mId, franId, member->tid, member->bzid, member->persona); // DRL DEBUG ONLY

            FranchiseIdData* fidData = findFidDataFromFranId (franId);
            MYASSERT (fidData);
            if (fidData)
            {
                fidData->memberData.push_back (member);
            }

            // Find the member who is the GM for this franchise.
            if (member->isGM)
            {
                currentGM = member->bzid;
            }
        }
    }
    gSelector->sleep (500 * 1000); // called a lot; keep it short; arg is in microsecs

    return rpcResult;
}

BlazeRpcError FranchiseInstance::removeMemberLocal (FranchiseId franId)
{
    BlazeRpcError rpcResult = ERR_OK;
    Franchise::RemoveMemberRequest req;

    BlazeId bzId = findNonGMMember (franId);
    if (bzId) // it may be rare that we find such a member
    {
        req.setFranchiseId (franId);
        req.setBlazeId (bzId);

        BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] Start of removeMember test, fid=%d", mId, franId);
        TimeValue startTime = TimeValue::getTimeOfDay();
        rpcResult = mProxy->removeMember (&req);
        TimeValue endTime = TimeValue::getTimeOfDay();
        mOwner->addMetric(FranchiseModule::METRIC_REMOVE_MEMBER, endTime - startTime);
        PROCESS_ERROR (rpcResult, "removeMemberLocal", FranchiseModule::METRIC_REMOVE_MEMBER);

        // If this succeeded, remove the member from the vector of member data for this franchise.
        if (rpcResult == ERR_OK)
        {
            FranchiseIdDataVectorType::iterator iterFrans = mFranchiseIdData.begin();
            FranchiseIdDataVectorType::iterator iterFransEnd = mFranchiseIdData.end();
            for (; iterFrans != iterFransEnd; iterFrans++)
            {
                FranchiseIdData* data = *iterFrans;
                if (data != NULL && data->fid == franId)
                {
                    bool done = false;
                    while (!done)
                    {
                        MemberDataVectorType::iterator iterMembers = data->memberData.begin();
                        for (; iterMembers != data->memberData.end(); iterMembers++)
                        {
                            done = true;
                            if ((*iterMembers)->bzid == bzId)
                            {
                                delete *iterMembers;                  // delete the MemberData structure
                                data->memberData.erase (iterMembers); // zap the vector entry as well
                                done = false;
                            }
                        }
                    }
                    break;
                }
            }
        }

        gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs
    }
    return rpcResult;
}

BlazeRpcError FranchiseInstance::clearBanLocal (FranchiseId franId)
{
    BlazeRpcError rpcResult = ERR_OK;
    Franchise::ClearBanRequest req;

    BlazeId bzId = findBannedMember (franId);
    if (bzId) // it may be rare that we find such a member
    {
        req.setFranchiseId (franId);
        req.setBlazeId (bzId);

        BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] Start of clearBan test, fid=%d, bzId=%d", mId, franId, bzId);
        TimeValue startTime = TimeValue::getTimeOfDay();
        rpcResult = mProxy->clearBan (&req);
        TimeValue endTime = TimeValue::getTimeOfDay();
        mOwner->addMetric(FranchiseModule::METRIC_CLEAR_BAN, endTime - startTime);
        PROCESS_ERROR (rpcResult, "clearBanLocal", FranchiseModule::METRIC_CLEAR_BAN);
        gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs

        // Clear the 'isBanned' flag for the member.
        if (rpcResult == ERR_OK)
        {
            changeBannedFlag (franId, bzId, false);
        }
    }

    return rpcResult;
}

BlazeRpcError FranchiseInstance::promoteToGMLocal (FranchiseId franId)
{
    BlazeRpcError rpcResult = ERR_OK;
    Franchise::PromoteToGMRequest req;

    BlazeId bzId = findNonGMMember (franId);
    if (bzId) // it may be rare that we find such a member
    {
        BlazeId bzIdOldGM = getGMForFranchise (franId);

        req.setFranchiseId (franId);
        req.setBlazeId (bzId);
        req.setBypass (0);  // force GM

        BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] Start of promoteToGM test, fid=%d", mId, franId);
        TimeValue startTime = TimeValue::getTimeOfDay();
        rpcResult = mProxy->promoteToGM (&req);
        TimeValue endTime = TimeValue::getTimeOfDay();
        mOwner->addMetric(FranchiseModule::METRIC_PROMOTE_TO_GM, endTime - startTime);
        PROCESS_ERROR (rpcResult, "promoteToGMLocal", FranchiseModule::METRIC_PROMOTE_TO_GM);
        gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs

        // Patch up the isGM flag for the members in this franchise.
        if (rpcResult == ERR_OK)
        {
            changeGMFlag (franId, bzIdOldGM, true);
            changeGMFlag (franId, bzId, true);
        }
    }
    return rpcResult;
}

BlazeRpcError FranchiseInstance::getRegularScheduleLocal (FranchiseId franId)
{
    BlazeRpcError rpcResult = ERR_OK;
    Franchise::GetRegularScheduleRequest  req;
    Franchise::GetRegularScheduleResponse resp;

    req.setTeamId (mTeamId);
    req.setFranchiseId (franId);

    BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] Start of getRegularSchedule test, fid=%d", mId, franId);
    TimeValue startTime = TimeValue::getTimeOfDay();
    rpcResult = mProxy->getRegularSchedule (&req, &resp);
    TimeValue endTime = TimeValue::getTimeOfDay();
    mOwner->addMetric(FranchiseModule::METRIC_GET_REGULAR_SCHEDULE, endTime - startTime);
    PROCESS_ERROR (rpcResult, "getRegularScheduleLocal", FranchiseModule::METRIC_GET_REGULAR_SCHEDULE);
    gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs

    return rpcResult;
}

BlazeRpcError FranchiseInstance::getRegularSchedule (Franchise::GetRegularScheduleRequest& req, Franchise::GetRegularScheduleResponse& resp)
{
    BlazeRpcError rpcResult = ERR_OK;

    BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] Start of getRegularSchedule test, fid=%d", mId, req.getFranchiseId());
    TimeValue startTime = TimeValue::getTimeOfDay();
    rpcResult = mProxy->getRegularSchedule (&req, &resp);
    TimeValue endTime = TimeValue::getTimeOfDay();
    mOwner->addMetric(FranchiseModule::METRIC_GET_REGULAR_SCHEDULE, endTime - startTime);
    PROCESS_ERROR (rpcResult, "getRegularSchedule", FranchiseModule::METRIC_GET_REGULAR_SCHEDULE);
    gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs

    return rpcResult;
}

BlazeRpcError FranchiseInstance::getPlayoffScheduleLocal(FranchiseId franId)
{
    BlazeRpcError rpcResult = ERR_OK;
    Franchise::GetPlayoffScheduleRequest  req;
    Franchise::GetPlayoffScheduleResponse resp;

    req.setSeasonId (0); // ??
    req.setFranchiseId (franId);

    BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] Start of getPlayoffSchedule test, fid=%d", mId, franId);
    TimeValue startTime = TimeValue::getTimeOfDay();
    rpcResult = mProxy->getPlayoffSchedule (&req, &resp);
    TimeValue endTime = TimeValue::getTimeOfDay();
    mOwner->addMetric(FranchiseModule::METRIC_GET_PLAYOFF_SCHEDULE, endTime - startTime);
    PROCESS_ERROR (rpcResult, "getPlayoffScheduleLocal", FranchiseModule::METRIC_GET_PLAYOFF_SCHEDULE);
    gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs

    return rpcResult;
}

// Get athlete data for those athletes on the specified team in the specified franchise.
BlazeRpcError FranchiseInstance::getDisplayRosterLocal(FranchiseId franId)
{
    BlazeRpcError rpcResult = ERR_OK;
    Franchise::GetDisplayRosterRequest  req;
    Franchise::GetDisplayRosterResponse resp;

    req.setTeamId (mTeamId);
    req.setFranchiseId (franId);

    BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] Start of getDisplayRoster test, TEAM_ID=%d, FRAN_ID=%d", mId, mTeamId, franId);
    TimeValue startTime = TimeValue::getTimeOfDay();
    rpcResult = mProxy->getDisplayRoster(&req, &resp);
    TimeValue endTime = TimeValue::getTimeOfDay();
    mOwner->addMetric(FranchiseModule::METRIC_GET_DISPLAY_ROSTER, endTime - startTime);
    PROCESS_ERROR (rpcResult, "getDisplayRosterLocal", FranchiseModule::METRIC_GET_DISPLAY_ROSTER);
    gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs

    return rpcResult;
}

// Get athlete data for those athletes on the specified team in the specified franchise.
BlazeRpcError FranchiseInstance::getAthletesOnTeamInFranchise (FranchiseId franId, TeamId teamId, bool sleepOk)
{
    BlazeRpcError rpcResult = ERR_OK;
    Franchise::GetDisplayRosterRequest  req;
    Franchise::GetDisplayRosterResponse resp;
    //BlazeId blazeId = getCreatorBlazeIdFromFranId (franId);

    req.setTeamId (teamId);
    req.setFranchiseId (franId);

    BLAZE_INFO (BlazeRpcLog::franchise, "[%d] Start of getAthletesOnTeamInFranchise, TEAM_ID=%d, FRAN_ID=%d", mId, teamId, franId);
    TimeValue startTime = TimeValue::getTimeOfDay();
    rpcResult = mProxy->getDisplayRoster(&req, &resp);
    TimeValue endTime = TimeValue::getTimeOfDay();
    mOwner->addMetric(FranchiseModule::METRIC_GET_DISPLAY_ROSTER, endTime - startTime);
    PROCESS_ERROR (rpcResult, "getAthletesOnTeamInFranchise", FranchiseModule::METRIC_GET_DISPLAY_ROSTER);

    // Put the returned data into an athleteData struct and put that into a map.
    GetDisplayRosterResponse::DisplayRosterEntityList listResults;
    GetDisplayRosterResponse::DisplayRosterEntityList* results = resp.getRoster();
    if (!results->empty())
    {
        int32_t count, delCount; // drl debug only
        athleteIdAthleteDataMapType::insert_return_type rt;
        GetDisplayRosterResponse::DisplayRosterEntityList::iterator iter = results->begin();
        for (count = 0, delCount = 0; iter != results->end(); iter++, count++)
        {
            AthleteId aid = (*iter)->getAthleteId ();
            AthleteData* aData = new AthleteData;
            //aData->bzid = blazeId;
            aData->fid  = franId;
            aData->tid  = teamId;
            aData->aid  = aid;
            rt = mAthleteDataMap.insert (athleteIdAthleteDataMapType::value_type(aid, aData));
            if (!rt.second) // if the insert failed, probably because that aid is already in the map
            {
                delete aData;
                delCount++;
            }
        }
        BLAZE_INFO (BlazeRpcLog::franchise, "[%d] getAthletesOnTeamInFranchise - ADDED %d athletes to map (%d deled)(fid=%d, tm=%d)",
            mId, count, delCount, franId, teamId);
    }
    else // debug only
    {
        BLAZE_INFO (BlazeRpcLog::franchise, "[%d] getAthletesOnTeamInFranchise, getDisplayRoster gave me NOTHING", mId);
    }
    if (sleepOk)
    {
        gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs
    }

    return rpcResult;
}

// Get athlete data for those athletes on the specified team in the specified franchise.
AthleteId FranchiseInstance::getOneAthleteOnTeamInFranchise (FranchiseId franId, TeamId teamId, bool sleepOk)
{
    // First, check to see if we already have filled in the mAthleteDataMap map.
    // If so, use it.  If not, call the function to fill in all the athlete data in
    // that map, and then call the function again.
    BlazeRpcError rpcResult = ERR_OK;
    AthleteId aId = getAthleteIdFromFranTeamId (franId, teamId);
    if (aId == 0 || aId == 32767)
    {
        BLAZE_INFO (BlazeRpcLog::franchise, "[%d] getOneAthleteOnTeamInFranchise1 - found 0 athletes for fid=%d, tid=%d",
            mId, franId, teamId);
        rpcResult = getAthletesOnTeamInFranchise (franId, teamId, sleepOk);
        aId = getAthleteIdFromFranTeamId (franId, teamId);
        BLAZE_INFO (BlazeRpcLog::franchise, "[%d] getOneAthleteOnTeamInFranchise2 - found aid=%d for fid=%d, tid=%d",
            mId, aId, franId, teamId);
        if (aId == 0)
        {
            aId = ATHLETE_ID_INVALID;  // the server checks for this, but not for 0
        }
    }
    return aId;
}

BlazeRpcError FranchiseInstance::getGameplayRosterLocal(FranchiseId franId)
{
    BlazeRpcError rpcResult = ERR_OK;
    Franchise::GetGameplayRosterRequest req;
    Franchise::GameplayRoster resp;

    req.setTeamId (mTeamId);
    req.setFranchiseId (franId);

    BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] Start of getGameplayRoster test, fid=%d", mId, franId);
    TimeValue startTime = TimeValue::getTimeOfDay();
    rpcResult = mProxy->getGameplayRoster(&req, &resp);
    TimeValue endTime = TimeValue::getTimeOfDay();
    mOwner->addMetric(FranchiseModule::METRIC_GET_GAMEPLAY_ROSTER, endTime - startTime);
    PROCESS_ERROR (rpcResult, "getGameplayRosterLocal", FranchiseModule::METRIC_GET_GAMEPLAY_ROSTER);
    gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs

    return rpcResult;
}

BlazeRpcError FranchiseInstance::getNumTeamInjuriesLocal(FranchiseId franId)
{
    BlazeRpcError rpcResult = ERR_OK;
    Franchise::GetNumTeamInjuriesRequest  req;
    Franchise::GetNumTeamInjuriesResponse resp;

    req.setTeamId (mTeamId);
    req.setFranchiseId (franId);

    BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] Start of getNumTeamInjuries test, fid=%d", mId, franId);
    TimeValue startTime = TimeValue::getTimeOfDay();
    rpcResult = mProxy->getNumTeamInjuries(&req, &resp);
    TimeValue endTime = TimeValue::getTimeOfDay();
    mOwner->addMetric(FranchiseModule::METRIC_GET_NUM_TEAM_INJURIES, endTime - startTime);
    PROCESS_ERROR (rpcResult, "getNumTeamInjuriesLocal", FranchiseModule::METRIC_GET_NUM_TEAM_INJURIES);
    gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs

    return rpcResult;
}

BlazeRpcError FranchiseInstance::getInjuriesByTeamLocal(FranchiseId franId)
{
    BlazeRpcError rpcResult = ERR_OK;
    Franchise::GetInjuriesByTeamRequest req;
    Franchise::GetInjuriesByTeamResponse resp;

    req.setTeamId (mTeamId);
    req.setFranchiseId (franId);

    BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] Start of getInjuriesByTeam test, fid=%d", mId, franId);
    TimeValue startTime = TimeValue::getTimeOfDay();
    rpcResult = mProxy->getInjuriesByTeam(&req, &resp);
    TimeValue endTime = TimeValue::getTimeOfDay();
    mOwner->addMetric(FranchiseModule::METRIC_GET_INJURIES_BY_TEAM, endTime - startTime);
    PROCESS_ERROR (rpcResult, "getInjuriesByTeamLocal", FranchiseModule::METRIC_GET_INJURIES_BY_TEAM);
    gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs

    return rpcResult;
}

BlazeRpcError FranchiseInstance::getGameBoxScoreByGameLocal(FranchiseId franId)
{
    BlazeRpcError rpcResult = ERR_OK;
    Franchise::GetGameBoxScoreByGameRequest req;
    Franchise::GameBoxScore resp;

    franId = 1;                  // so we'll get a hit in our test database
    req.setGameId (5);           // ditto
    req.setSeasonId (1);         // ditto
    req.setFranchiseId (franId);

    BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] Start of getGameBoxScoreByGame test, fid=%d", mId, franId);
    TimeValue startTime = TimeValue::getTimeOfDay();
    rpcResult = mProxy->getGameBoxScoreByGame(&req, &resp);
    TimeValue endTime = TimeValue::getTimeOfDay();
    mOwner->addMetric(FranchiseModule::METRIC_GET_GAME_BOX_SCORE_BY_GAME, endTime - startTime);
    PROCESS_ERROR (rpcResult, "getGameBoxScoreByGameLocal", FranchiseModule::METRIC_GET_GAME_BOX_SCORE_BY_GAME);
    gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs

    return rpcResult;
}

BlazeRpcError FranchiseInstance::getGameBoxScoresByWeekLocal(FranchiseId franId)
{
    BlazeRpcError rpcResult = ERR_OK;
    Franchise::GetGameBoxScoresByWeekRequest  req;
    Franchise::GetGameBoxScoresByWeekResponse resp;

    franId = 1;                  // so we'll get a hit in our test database
    req.setWindowId (1);         // ditto
    req.setSeasonId (1);         // ditto
    req.setFranchiseId (franId);

    BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] Start of getGameBoxScoresByWeek test, fid=%d", mId, franId);
    TimeValue startTime = TimeValue::getTimeOfDay();
    rpcResult = mProxy->getGameBoxScoresByWeek(&req, &resp);
    TimeValue endTime = TimeValue::getTimeOfDay();
    mOwner->addMetric(FranchiseModule::METRIC_GET_GAME_BOX_SCORES_BY_WEEK, endTime - startTime);
    PROCESS_ERROR (rpcResult, "getGameBoxScoresByWeekLocal", FranchiseModule::METRIC_GET_GAME_BOX_SCORES_BY_WEEK);
    gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs

    return rpcResult;
}

BlazeRpcError FranchiseInstance::getGameBoxScoresByTeamLocal(FranchiseId franId)
{
    BlazeRpcError rpcResult = ERR_OK;
    Franchise::GetGameBoxScoresByTeamRequest  req;
    Franchise::GetGameBoxScoresByTeamResponse resp;

    franId = 1;                  // so we'll get a hit in our test database
    req.setTeamId (10);          // ditto
    req.setSeasonId (1);         // ditto
    req.setFranchiseId (franId);

    BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] Start of getGameBoxScoresByTeam test, fid=%d", mId, franId);
    TimeValue startTime = TimeValue::getTimeOfDay();
    rpcResult = mProxy->getGameBoxScoresByTeam(&req, &resp);
    TimeValue endTime = TimeValue::getTimeOfDay();
    mOwner->addMetric(FranchiseModule::METRIC_GET_GAME_BOX_SCORES_BY_TEAM, endTime - startTime);
    PROCESS_ERROR (rpcResult, "getGameBoxScoresByTeamLocal", FranchiseModule::METRIC_GET_GAME_BOX_SCORES_BY_TEAM);
    gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs

    return rpcResult;
}

BlazeRpcError FranchiseInstance::getScoringSummaryLocal(FranchiseId franId)
{
    BlazeRpcError rpcResult = ERR_OK;
    Franchise::GetScoringSummaryRequest  req;
    Franchise::GetScoringSummaryResponse resp;

    franId = 1;                  // this won't work unless we have stats in the db
    req.setGameId (2);
    req.setFranchiseId (franId);

    BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] Start of getScoringSummary test, fid=%d", mId, franId);
    TimeValue startTime = TimeValue::getTimeOfDay();
    rpcResult = mProxy->getScoringSummary(&req, &resp);
    TimeValue endTime = TimeValue::getTimeOfDay();
    mOwner->addMetric(FranchiseModule::METRIC_GET_SCORING_SUMMARY, endTime - startTime);
    PROCESS_ERROR (rpcResult, "getScoringSummaryLocal", FranchiseModule::METRIC_GET_SCORING_SUMMARY);
    gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs

    return rpcResult;
}

BlazeRpcError FranchiseInstance::getTeamStatsByGameLocal(FranchiseId franId)
{
    BlazeRpcError rpcResult = ERR_OK;
    Franchise::GetTeamStatsByGameRequest  req;
    Franchise::GetTeamStatsByGameResponse resp;

    req.setFranchiseId (1); // this won't work unless we have stats in the db
    req.setGameId (2);

    BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] Start of getTeamStatsByGame test, fid=%d", mId, franId);
    TimeValue startTime = TimeValue::getTimeOfDay();
    rpcResult = mProxy->getTeamStatsByGame(&req, &resp);
    TimeValue endTime = TimeValue::getTimeOfDay();
    mOwner->addMetric(FranchiseModule::METRIC_GET_TEAM_STATS_BY_GAME, endTime - startTime);
    PROCESS_ERROR (rpcResult, "getTeamStatsByGameLocal", FranchiseModule::METRIC_GET_TEAM_STATS_BY_GAME);
    gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs

    return rpcResult;
}

BlazeRpcError FranchiseInstance::getAthleteStatsByGameLocal(FranchiseId franId)
{
    BlazeRpcError rpcResult = ERR_OK;
    Franchise::GetAthleteStatsByGameRequest  req;
    Franchise::GetAthleteStatsByGameResponse resp;

    req.setFranchiseId (1); // this won't work unless we have stats in the db
    req.setGameId (2);

    BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] Start of getAthleteStatsByGame test, fid=%d", mId, franId);
    TimeValue startTime = TimeValue::getTimeOfDay();
    rpcResult = mProxy->getAthleteStatsByGame(&req, &resp);
    TimeValue endTime = TimeValue::getTimeOfDay();
    mOwner->addMetric(FranchiseModule::METRIC_GET_ATHLETE_STATS_BY_GAME, endTime - startTime);
    PROCESS_ERROR (rpcResult, "getAthleteStatsByGameLocal", FranchiseModule::METRIC_GET_ATHLETE_STATS_BY_GAME);
    gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs

    return rpcResult;
}

BlazeRpcError FranchiseInstance::updateFranchiseSettingsLocal(FranchiseId franId)
{
    BlazeRpcError rpcResult = ERR_OK;
    Franchise::UpdateFranchiseSettingsRequest req;

    req.setQuarterLength(DEFAULT_QUARTER_LEN);
    req.setTradeType(FRANCHISE_TRADES_ENABLED_ALL);
    req.setSalaryCapEnabled(1);
    req.setInjuriesEnabled(1);
    req.setJoinsEnabled (1);
    req.setDescription ("Updated description");
    req.setFranchiseId (franId);

    BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] Start of updateFranchiseSettings test, fid=%d", mId, franId);
    TimeValue startTime = TimeValue::getTimeOfDay();
    rpcResult = mProxy->updateFranchiseSettings (&req);
    TimeValue endTime = TimeValue::getTimeOfDay();
    mOwner->addMetric(FranchiseModule::METRIC_UPDATE_FRANCHISE_SETTINGS, endTime - startTime);
    PROCESS_ERROR (rpcResult, "updateFranchiseSettingsLocal", FranchiseModule::METRIC_UPDATE_FRANCHISE_SETTINGS);
    gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs

    return rpcResult;
}

BlazeRpcError FranchiseInstance::getDisplayRosterInfo (TeamId tid, FranchiseId fid, Franchise::GetDisplayRosterResponse& out)
{
    BlazeRpcError rpcResult = ERR_OK;
    Franchise::GetDisplayRosterRequest  req;
    Franchise::GetDisplayRosterResponse resp;

    req.setTeamId (tid);
    req.setFranchiseId (fid);

    BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] Calling getDisplayRoster, TEAM_ID=%d, FRAN_ID=%d", mId, tid, fid);
    TimeValue startTime = TimeValue::getTimeOfDay();
    rpcResult = mProxy->getDisplayRoster(&req, &resp);
    TimeValue endTime = TimeValue::getTimeOfDay();
    mOwner->addMetric(FranchiseModule::METRIC_GET_DISPLAY_ROSTER, endTime - startTime);
    PROCESS_ERROR (rpcResult, "getDisplayRosterInfo", FranchiseModule::METRIC_GET_DISPLAY_ROSTER);

    if (rpcResult == ERR_OK)
    {
        GetDisplayRosterResponse::DisplayRosterEntityList* listResult = resp.getRoster();
        if (!listResult->empty())
        {
            resp.copyInto (out);
        }
        else
        {
            rpcResult = FRANCHISE_ERR_ROSTER_ERROR;
        }
    }
    gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs
    return rpcResult;
}

// Here we want to return the franchise id to use when calling processTradeLocal
BlazeRpcError FranchiseInstance::proposeTradeLocal (FranchiseId& franId)
{
    BlazeRpcError rpcResult = ERR_OK;
    Franchise::ProposeTradeRequest  req;
    Franchise::GetDisplayRosterResponse team1;
    Franchise::GetDisplayRosterResponse team2;

    // Get a franchise id for the current blazeid.
    franId = getFranchiseWithFantasyDraftSetting (0, true, mBlazeId);
    if (franId == 0)
    {
        franId = getFranchiseWithFantasyDraftSetting (0, true, 0);
        if (franId == 0)
        {
            franId = getGMFranchiseIdForUser ();
            if (franId == 0)
            {
                return rpcResult; // todo - figure out good error code; code not used currently anyway
            }
        }
    }

    // We need to be in the regular season in order for trades to work.
    rpcResult = advanceFranchiseLocal (franId, FRANCHISE_STATE_REGULAR_SEASON);
    BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] (proposeTrade) advanceFranchiseLocal result=%d, fid=%d", mId, rpcResult, franId); // drl debug only

    // Call getDisplayRoster to get all athlete ids for the given franId and blaze id
    //TeamId myTeamId = getMyTeamId (franId, mBlazeId);
    BlazeRpcError rpcResult1 = getDisplayRosterInfo (5, franId, team1);
    BlazeRpcError rpcResult2 = getDisplayRosterInfo (6, franId, team2);
    BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] (proposeTrade) display roster errs are %d, %d; fid=%d", mId, rpcResult1, rpcResult2, franId);

    if (rpcResult1 == ERR_OK && rpcResult2 == ERR_OK)
    {
        // We need to grab the results from the team1 struct.
        GetDisplayRosterResponse::DisplayRosterEntityList* data1 = team1.getRoster(); // vector of DisplayRosterEntity structs
        GetDisplayRosterResponse::DisplayRosterEntityList* data2 = team2.getRoster(); // ditto
        BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] (proposeTrade) roster results are %p, %p", mId, data1, data2);
        if (!data1->empty() && !data2->empty())             // we need at least 1 element per result
        {
            AthleteId aid1;
            AthleteId aid2;
            DisplayRosterEntity* item1 = data1->front();
            DisplayRosterEntity* item2 = data2->front();
            aid1 = item1->getAthleteId ();
            aid2 = item2->getAthleteId ();
            req.setFranchiseId (franId);
            req.setSourceTeamId (5); // the athletes below have to be on these teams for this to work
            req.setTargetTeamId (6);
            req.setSourceAthlete1 (aid1);
            req.setSourceAthlete2 (ATHLETE_ID_INVALID);
            req.setSourceAthlete3 (ATHLETE_ID_INVALID);
            req.setTargetAthlete1 (aid2);
            req.setTargetAthlete2 (ATHLETE_ID_INVALID);
            req.setTargetAthlete3 (ATHLETE_ID_INVALID);

            BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] Start of proposeTrade test; fid=%d, aid1=%d, aid2=%d", mId, franId, aid1, aid2);
            TimeValue startTime = TimeValue::getTimeOfDay();
            rpcResult = mProxy->proposeTrade (&req);
            TimeValue endTime = TimeValue::getTimeOfDay();
            mOwner->addMetric(FranchiseModule::METRIC_PROPOSE_TRADE, endTime - startTime);
            PROCESS_ERROR (rpcResult, "proposeTradeLocal", FranchiseModule::METRIC_PROPOSE_TRADE);
            gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs
        }
    }

    return rpcResult;
}

BlazeRpcError FranchiseInstance::processTradeLocal (FranchiseId franId, TradeId tradeId)
{
    BlazeRpcError rpcResult = ERR_OK;
    Franchise::ProcessTradeRequest  req;

    req.setFranchiseId (franId);
    req.setTradeId (tradeId); // grabbed from output of getTrades
    req.setOperation (TRADE_ACCEPT);
    req.setBlazeId (mBlazeId);

    BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] Start of processTrade test, fid=%d", mId, franId);
    TimeValue startTime = TimeValue::getTimeOfDay();
    rpcResult = mProxy->processTrade (&req);
    TimeValue endTime = TimeValue::getTimeOfDay();
    mOwner->addMetric(FranchiseModule::METRIC_PROCESS_TRADE, endTime - startTime);
    PROCESS_ERROR (rpcResult, "processTradeLocal", FranchiseModule::METRIC_PROCESS_TRADE);
    gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs

    return rpcResult;
}

BlazeRpcError FranchiseInstance::getTradesLocal (FranchiseId franId, TradeId& tradeId)
{
    BlazeRpcError rpcResult = ERR_OK;
    Franchise::GetTradesRequest  req;
    Franchise::GetTradesResponse resp;

    tradeId = 0;       // init
    req.setFranchiseId (franId);
    req.setTeamId (5); // because we used team 5 when we 'proposed' the trade
    req.setRosterId (mRosterId); 

    BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] Start of getTrades test, fid=%d", mId, franId);
    TimeValue startTime = TimeValue::getTimeOfDay();
    rpcResult = mProxy->getTrades (&req, &resp);
    TimeValue endTime = TimeValue::getTimeOfDay();
    mOwner->addMetric(FranchiseModule::METRIC_GET_TRADES, endTime - startTime);
    PROCESS_ERROR (rpcResult, "getTradesLocal", FranchiseModule::METRIC_GET_TRADES);

    // Lets get a trade id from the response data.
    Franchise::GetTradesResponse::TradeList* trades = resp.getTrades();
    if (!trades->empty())
    {
        Franchise::GetTradesResponse::TradeList::iterator iter = trades->begin();
        if (iter != trades->end())
        {
            tradeId = (*iter)->getTradeId ();
            BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] getTrades - found a trade = %d", mId, tradeId);
        }
    }
    BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] getTrades - done, trade = %d", mId, tradeId);
    gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs

    return rpcResult;
}

BlazeRpcError FranchiseInstance::getSuperbowlsLocal (FranchiseId franId)
{
    BlazeRpcError rpcResult = ERR_OK;
    Franchise::GetSuperbowlsRequest  req;
    Franchise::GetSuperbowlsResponse resp;

    franId = 1;                  // so it will work with our pre-built database
    req.setFranchiseId (franId);

    BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] Start of getSuperbowls test, fid=%d", mId, franId);
    TimeValue startTime = TimeValue::getTimeOfDay();
    rpcResult = mProxy->getSuperbowls (&req, &resp);
    TimeValue endTime = TimeValue::getTimeOfDay();
    mOwner->addMetric(FranchiseModule::METRIC_GET_SUPER_BOWLS, endTime - startTime);
    PROCESS_ERROR (rpcResult, "getSuperbowlsLocal", FranchiseModule::METRIC_GET_SUPER_BOWLS);
    gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs

    return rpcResult;
}

BlazeRpcError FranchiseInstance::getDisplayAthleteLocal (FranchiseId franId)
{
    BlazeRpcError rpcResult = ERR_OK;
    Franchise::GetDisplayAthleteRequest  req;
    Franchise::GetDisplayAthleteResponse resp;
    AthleteId aId = getOneAthleteOnTeamInFranchise (franId, mTeamId, false);

    req.setFranchiseId (franId);
    req.setAthleteId (aId);

    BLAZE_INFO (BlazeRpcLog::franchise, "[%d] Start of getDisplayAthlete test, fid=%d, aid=%d, tmid=%d", mId, franId, aId, mTeamId);
    TimeValue startTime = TimeValue::getTimeOfDay();
    rpcResult = mProxy->getDisplayAthlete (&req, &resp);
    TimeValue endTime = TimeValue::getTimeOfDay();
    mOwner->addMetric(FranchiseModule::METRIC_GET_DISPLAY_ATHLETE, endTime - startTime);
    PROCESS_ERROR (rpcResult, "getDisplayAthleteLocal", FranchiseModule::METRIC_GET_DISPLAY_ATHLETE);
    gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs

    return rpcResult;
}

BlazeRpcError FranchiseInstance::getRawDataLocal (FranchiseId franId)
{
    BlazeRpcError rpcResult = ERR_OK;
    Franchise::GetRawDataRequest  req;
    Franchise::GetRawDataResponse resp;
    RawDataFilter *filter1 = new RawDataFilter;
    RawDataFilter *filter2 = new RawDataFilter;
    RawDataFilter *filter3 = new RawDataFilter;
    GetRawDataRequest::RawDataFilterList* filterList = req.getFilters();

    // Data source - SCHEDULE.
    req.setFranchiseId (franId);
    req.setStartOffset (0); // offset of the first record to return
    req.setDataSource (FRANCHISE_RAWDATA_SOURCE_SCHEDULE); // see enum in franchise.tdf (comp/fr/gen)
    req.setNumRecords (0);

    BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] Start of getRawData test #1, fid=%d", mId, franId);
    TimeValue startTime = TimeValue::getTimeOfDay();
    rpcResult = mProxy->getRawData (&req, &resp);
    TimeValue endTime = TimeValue::getTimeOfDay();
    mOwner->addMetric(FranchiseModule::METRIC_GET_RAW_DATA, endTime - startTime);
    PROCESS_ERROR (rpcResult, "getRawDataLocal1", FranchiseModule::METRIC_GET_RAW_DATA);
    gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs

    // Data source - SCHEDULE - Run test again with a filter.
    req.setFranchiseId (franId);
    req.setStartOffset (0); // offset of the first record to return
    req.setDataSource (FRANCHISE_RAWDATA_SOURCE_SCHEDULE);
    req.setNumRecords (0);
    filter1->setType (FRANCHISE_RAWDATA_FILTER_TEAM);
    filter1->setData (5);    // team id
    filterList->push_back (filter1);

    BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] Start of getRawData test #2, fid=%d", mId, franId);
    startTime = TimeValue::getTimeOfDay();
    rpcResult = mProxy->getRawData (&req, &resp);
    endTime = TimeValue::getTimeOfDay();
    mOwner->addMetric(FranchiseModule::METRIC_GET_RAW_DATA, endTime - startTime);
    PROCESS_ERROR (rpcResult, "getRawDataLocal2", FranchiseModule::METRIC_GET_RAW_DATA);
    gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs

    // Data source - SCHEDULE - Run test again with another filter.
    req.setFranchiseId (franId);
    req.setStartOffset (0); // offset of the first record to return
    req.setDataSource (FRANCHISE_RAWDATA_SOURCE_SCHEDULE);
    req.setNumRecords (0);
    filterList->clear ();
    filter1->setType (FRANCHISE_RAWDATA_FILTER_TIMEPERIOD);
    filter1->setData (5);    // week num
    filterList->push_back (filter1);

    BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] Start of getRawData test #3, fid=%d", mId, franId);
    startTime = TimeValue::getTimeOfDay();
    rpcResult = mProxy->getRawData (&req, &resp);
    endTime = TimeValue::getTimeOfDay();
    mOwner->addMetric(FranchiseModule::METRIC_GET_RAW_DATA, endTime - startTime);
    PROCESS_ERROR (rpcResult, "getRawDataLocal3", FranchiseModule::METRIC_GET_RAW_DATA);
    gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs

    // New data source - PLAYER.
    req.setFranchiseId (franId);
    req.setStartOffset (0); // offset of the first record to return
    req.setDataSource (FRANCHISE_RAWDATA_SOURCE_PLAYER);
    req.setNumRecords (50);
    filterList->clear ();

    BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] Start of getRawData test #4, fid=%d", mId, franId);
    startTime = TimeValue::getTimeOfDay();
    rpcResult = mProxy->getRawData (&req, &resp);
    endTime = TimeValue::getTimeOfDay();
    mOwner->addMetric(FranchiseModule::METRIC_GET_RAW_DATA, endTime - startTime);
    PROCESS_ERROR (rpcResult, "getRawDataLocal4", FranchiseModule::METRIC_GET_RAW_DATA);
    gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs

    // New data source - PLAYER - Run test again with a filter.
    req.setFranchiseId (franId);
    req.setStartOffset (0); // offset of the first record to return
    req.setDataSource (FRANCHISE_RAWDATA_SOURCE_PLAYER);
    req.setNumRecords (50);
    filterList->clear ();
    filter1->setType (FRANCHISE_RAWDATA_FILTER_TEAM);
    filter1->setData (5);    // team id
    filterList->push_back (filter1);

    BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] Start of getRawData test #5, fid=%d", mId, franId);
    startTime = TimeValue::getTimeOfDay();
    rpcResult = mProxy->getRawData (&req, &resp);
    endTime = TimeValue::getTimeOfDay();
    mOwner->addMetric(FranchiseModule::METRIC_GET_RAW_DATA, endTime - startTime);
    PROCESS_ERROR (rpcResult, "getRawDataLocal5", FranchiseModule::METRIC_GET_RAW_DATA);
    gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs

    // New data source - STANDINGS.
    req.setFranchiseId (franId);
    req.setStartOffset (0); // offset of the first record to return
    req.setDataSource (FRANCHISE_RAWDATA_SOURCE_STANDINGS);
    req.setNumRecords (0);
    filterList->clear ();

    BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] Start of getRawData test #6, fid=%d", mId, franId);
    startTime = TimeValue::getTimeOfDay();
    rpcResult = mProxy->getRawData (&req, &resp);
    endTime = TimeValue::getTimeOfDay();
    mOwner->addMetric(FranchiseModule::METRIC_GET_RAW_DATA, endTime - startTime);
    PROCESS_ERROR (rpcResult, "getRawDataLocal6", FranchiseModule::METRIC_GET_RAW_DATA);
    gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs

    // New data source - GAMEROSTEREXT.
    req.setFranchiseId (franId);
    req.setStartOffset (0); // offset of the first record to return
    req.setDataSource (FRANCHISE_RAWDATA_SOURCE_GAMEROSTEREXT);
    req.setNumRecords (0);
    filterList->clear ();
    filter1->setType (FRANCHISE_RAWDATA_FILTER_TEAM);
    filter1->setData (5);    // team id
    filterList->push_back (filter1);

    BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] Start of getRawData test #7, fid=%d", mId, franId);
    startTime = TimeValue::getTimeOfDay();
    rpcResult = mProxy->getRawData (&req, &resp);
    endTime = TimeValue::getTimeOfDay();
    mOwner->addMetric(FranchiseModule::METRIC_GET_RAW_DATA, endTime - startTime);
    PROCESS_ERROR (rpcResult, "getRawDataLocal7", FranchiseModule::METRIC_GET_RAW_DATA);
    gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs

    // New data source - PLAYERSEASONSTATS.
    req.setFranchiseId (franId);
    req.setStartOffset (0); // offset of the first record to return
    req.setDataSource (FRANCHISE_RAWDATA_SOURCE_PLAYERSEASONSTATS);
    req.setNumRecords (0);
    filterList->clear ();
    filter1->setType (FRANCHISE_RAWDATA_FILTER_TEAM);
    filter1->setData (5);    // team id
    filterList->push_back (filter1);
    filter2->setType (FRANCHISE_RAWDATA_FILTER_TIMEPERIOD);
    filter2->setData (0);    // year - ?? should this be 0 based, or a real year?
    filterList->push_back (filter2);
    filter3->setType (FRANCHISE_RAWDATA_FILTER_STATTYPE);
    filter3->setData (FRANCHISE_PLAYER_STAT_DEFENSIVE); // stat type
    filterList->push_back (filter3);

    BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] Start of getRawData test #8, fid=%d", mId, franId);
    startTime = TimeValue::getTimeOfDay();
    rpcResult = mProxy->getRawData (&req, &resp);
    endTime = TimeValue::getTimeOfDay();
    mOwner->addMetric(FranchiseModule::METRIC_GET_RAW_DATA, endTime - startTime);
    PROCESS_ERROR (rpcResult, "getRawDataLocal8", FranchiseModule::METRIC_GET_RAW_DATA);
    gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs

    // New data source - PLAYERSEASONSTATS - Run test again with another filter.
    req.setFranchiseId (franId);
    req.setStartOffset (0); // offset of the first record to return
    req.setDataSource (FRANCHISE_RAWDATA_SOURCE_PLAYERSEASONSTATS);
    req.setNumRecords (0);
    filterList->clear ();
    filter1->setType (FRANCHISE_RAWDATA_FILTER_TIMEPERIOD);
    filter1->setData (0);    // year - ?? should this be 0 based, or a real year?
    filterList->push_back (filter1);
    filter2->setType (FRANCHISE_RAWDATA_FILTER_STATTYPE);
    filter2->setData (FRANCHISE_PLAYER_STAT_OFFENSIVE); // stat type
    filterList->push_back (filter2);
    filter3->setType (FRANCHISE_RAWDATA_FILTER_ATHLETE);
    filter3->setData (57);   // aid
    filterList->push_back (filter3);

    BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] Start of getRawData test #9, fid=%d", mId, franId);
    startTime = TimeValue::getTimeOfDay();
    rpcResult = mProxy->getRawData (&req, &resp);
    endTime = TimeValue::getTimeOfDay();
    mOwner->addMetric(FranchiseModule::METRIC_GET_RAW_DATA, endTime - startTime);
    PROCESS_ERROR (rpcResult, "getRawDataLocal9", FranchiseModule::METRIC_GET_RAW_DATA);
    gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs

    // New data source - STATLEADERS.
    req.setFranchiseId (franId);
    req.setStartOffset (0); // offset of the first record to return
    req.setDataSource (FRANCHISE_RAWDATA_SOURCE_STATLEADERS);
    req.setNumRecords (0);
    filterList->clear ();
    filter1->setType (FRANCHISE_RAWDATA_FILTER_TIMEPERIOD);
    filter1->setData (0);    // year - ?? should this be 0 based, or a real year?
    filterList->push_back (filter1);
    filter2->setType (FRANCHISE_RAWDATA_FILTER_STATLEADER);
    filter2->setData (FRANCHISE_STAT_LEADER_DEFENSIVE); // stat type
    filterList->push_back (filter2);

    BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] Start of getRawData test #10, fid=%d", mId, franId);
    startTime = TimeValue::getTimeOfDay();
    rpcResult = mProxy->getRawData (&req, &resp);
    endTime = TimeValue::getTimeOfDay();
    mOwner->addMetric(FranchiseModule::METRIC_GET_RAW_DATA, endTime - startTime);
    PROCESS_ERROR (rpcResult, "getRawDataLocal10", FranchiseModule::METRIC_GET_RAW_DATA);
    gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs

    return rpcResult;
}

BlazeRpcError FranchiseInstance::getDepthChartLocal (FranchiseId franId)
{
    BlazeRpcError rpcResult = ERR_OK;
    Franchise::GetDepthChartRequest  req;
    Franchise::GetDepthChartResponse resp;

    req.setFranchiseId (franId);
    req.setTeamId (mTeamId);

    BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] Start of getDepthChart test, fid=%d", mId, franId);
    TimeValue startTime = TimeValue::getTimeOfDay();
    rpcResult = mProxy->getDepthChart (&req, &resp);
    TimeValue endTime = TimeValue::getTimeOfDay();
    mOwner->addMetric(FranchiseModule::METRIC_GET_DEPTH_CHART, endTime - startTime);
    PROCESS_ERROR (rpcResult, "getDepthChartLocal", FranchiseModule::METRIC_GET_DEPTH_CHART);
    gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs

    return rpcResult;
}

BlazeRpcError FranchiseInstance::setDepthChartLocal (FranchiseId franId)
{
    BlazeRpcError rpcResult = ERR_OK;
    Franchise::SetDepthChartRequest  req;
    SetDepthChartRequest::DepthChartUploadElementList* dclist;
    DepthChartUploadElement* dcEle = new DepthChartUploadElement;

    req.setFranchiseId (franId);
    req.setTeamId (mTeamId);
    dclist = req.getDepthChartUpload ();
    dcEle->setAthleteId (getOneAthleteOnTeamInFranchise (franId, mTeamId, false));
    dcEle->setPosition (5);
    dcEle->setDepth (5);
    dclist->push_back (dcEle);

    BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] Start of setDepthChart test, fid=%d, tm=%d", mId, franId, mTeamId);
    TimeValue startTime = TimeValue::getTimeOfDay();
    rpcResult = mProxy->setDepthChart (&req);
    TimeValue endTime = TimeValue::getTimeOfDay();
    mOwner->addMetric(FranchiseModule::METRIC_SET_DEPTH_CHART, endTime - startTime);
    PROCESS_ERROR (rpcResult, "setDepthChartLocal", FranchiseModule::METRIC_SET_DEPTH_CHART);
    gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs

    return rpcResult;
}

BlazeRpcError FranchiseInstance::generateDepthChartLocal (FranchiseId franId)
{
    BlazeRpcError rpcResult = ERR_OK;
    Franchise::GenerateDepthChartRequest  req;

    req.setFranchiseId (franId);
    req.setTeamId (mTeamId);

    BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] Start of generateDepthChart test, fid=%d", mId, franId);
    TimeValue startTime = TimeValue::getTimeOfDay();
    rpcResult = mProxy->generateDepthChart (&req);
    TimeValue endTime = TimeValue::getTimeOfDay();
    mOwner->addMetric(FranchiseModule::METRIC_GENERATE_DEPTH_CHART, endTime - startTime);
    PROCESS_ERROR (rpcResult, "generateDepthChartLocal", FranchiseModule::METRIC_GENERATE_DEPTH_CHART);
    gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs

    return rpcResult;
}

BlazeRpcError FranchiseInstance::getNewsLocal (FranchiseId franId)
{
    BlazeRpcError rpcResult = ERR_OK;
    Franchise::GetNewsRequest  req;
    Franchise::GetNewsResponse resp;

    req.setFranchiseId (franId);
    req.setMsgType (NEWS_MSG_TYPE_ALL);
    req.setFirstItem (1); // ?? 1st item to get?
    req.setNumItems (1);

    BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] Start of getNews test, fid=%d", mId, franId);
    TimeValue startTime = TimeValue::getTimeOfDay();
    rpcResult = mProxy->getNews (&req, &resp);
    TimeValue endTime = TimeValue::getTimeOfDay();
    mOwner->addMetric(FranchiseModule::METRIC_GET_NEWS, endTime - startTime);
    PROCESS_ERROR (rpcResult, "getNewsLocal", FranchiseModule::METRIC_GET_NEWS);
    gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs

    return rpcResult;
}

BlazeRpcError FranchiseInstance::getInvitationsLocal (FranchiseId franId)
{
    BlazeRpcError rpcResult = ERR_OK;
    Franchise::GetInvitationsRequest  req;
    Franchise::GetInvitationsResponse resp;

    req.setFranchiseId (franId);
    req.setInvitationsToGet (FRANCHISE_INVITATIONS_ALL);

    BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] Start of getInvitations test, fid=%d", mId, franId);
    TimeValue startTime = TimeValue::getTimeOfDay();
    rpcResult = mProxy->getInvitations (&req, &resp);
    TimeValue endTime = TimeValue::getTimeOfDay();
    mOwner->addMetric(FranchiseModule::METRIC_GET_INVITATIONS, endTime - startTime);
    PROCESS_ERROR (rpcResult, "getInvitationsLocal", FranchiseModule::METRIC_GET_INVITATIONS);
    gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs

    return rpcResult;
}

// Send a message to a persona to join the given franchise.
BlazeRpcError FranchiseInstance::sendInvitationLocal (FranchiseId franId, bool useOurFranchisesOnly)
{
    BlazeRpcError rpcResult = ERR_OK;
    Franchise::SendInvitationRequest  req;
    char8_t* personaString = NULL;
    mInviteeBlazeId = 0;

    // First find all members who are NOT in this franchise.
    MemberDataVectorType membersVec;
    findMembersInOtherFranchises (franId, membersVec, useOurFranchisesOnly);

    // Next loop thru them until we find one with a persona.
    MemberDataVectorType::iterator iterMembers = membersVec.begin();
    MemberDataVectorType::iterator iterMembersEnd = membersVec.end();
    for (; iterMembers != iterMembersEnd; iterMembers++)
    {
        if ((*iterMembers)->persona != NULL)
        {
            personaString = (*iterMembers)->persona;
            mInviteeBlazeId = (*iterMembers)->bzid;
            break;
        }
    }
    BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] sendInvitation - found persona = %s, bzid=%d",
        mId, (personaString && *personaString) ? personaString : "not found", mInviteeBlazeId);

    if (personaString && *personaString)
    {
        req.setFranchiseId (franId);
        req.setPersona(personaString);

        BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] Start of sendInvitation test, fid=%d, persona=%s", mId, franId, personaString);
        TimeValue startTime = TimeValue::getTimeOfDay();
        rpcResult = mProxy->sendInvitation (&req);
        TimeValue endTime = TimeValue::getTimeOfDay();
        mOwner->addMetric(FranchiseModule::METRIC_SEND_INVITATION, endTime - startTime);
        PROCESS_ERROR (rpcResult, "sendInvitationLocal", FranchiseModule::METRIC_SEND_INVITATION);
        gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs
    }
    return rpcResult;
}

BlazeRpcError FranchiseInstance::processInvitationLocal (FranchiseId franId)
{
    BlazeRpcError rpcResult = ERR_OK;
    Franchise::ProcessInvitationRequest  req;

    req.setFranchiseId (franId);
    req.setInviterId (mBlazeId);          // Blaze id of the user who created the invitation
    req.setInviteeId (mInviteeBlazeId);   // Blaze id of the user who will accept/reject the invitation
    req.setOperation ((InvitationOp)0);   // 0=INVITATION_ACCEPT (but not defined yet), or REVOKE would work as well

    BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] Start of processInvitation test, fid=%d", mId, franId);
    TimeValue startTime = TimeValue::getTimeOfDay();
    rpcResult = mProxy->processInvitation (&req);
    TimeValue endTime = TimeValue::getTimeOfDay();
    mOwner->addMetric(FranchiseModule::METRIC_PROCESS_INVITATION, endTime - startTime);
    PROCESS_ERROR (rpcResult, "processInvitationLocal", FranchiseModule::METRIC_PROCESS_INVITATION);
    gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs

    // If this worked and the invitation was to accept, then this is equivalent to a join franchise.
    // So, patch up our internal data structures.
    if (rpcResult == ERR_OK)
    {
        // Update member data, now that we've altered it.
        getMembersLocal (franId);
    }

    return rpcResult;
}

BlazeRpcError FranchiseInstance::getDraftProfileLocal (FranchiseId franId)
{
    BlazeRpcError rpcResult = ERR_OK;
    Franchise::GetDraftProfileRequest  req;
    Franchise::DraftProfile resp;

    req.setFranchiseId (franId);

    BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] Start of getDraftProfile test, fid=%d", mId, franId);
    TimeValue startTime = TimeValue::getTimeOfDay();
    rpcResult = mProxy->getDraftProfile (&req, &resp);
    TimeValue endTime = TimeValue::getTimeOfDay();
    mOwner->addMetric(FranchiseModule::METRIC_GET_DRAFT_PROFILE, endTime - startTime);
    PROCESS_ERROR (rpcResult, "getDraftProfileLocal", FranchiseModule::METRIC_GET_DRAFT_PROFILE);
    gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs

    return rpcResult;
}

BlazeRpcError FranchiseInstance::getDraftHistoryLocal (FranchiseId franId)
{
    BlazeRpcError rpcResult = ERR_OK;
    Franchise::GetDraftHistoryRequest  req;
    Franchise::GetDraftHistoryResponse resp;

    req.setFranchiseId (franId);
    req.setSeasonId (0); // season to fetch draft results for (0 = 1st season)
    req.setPick (1);     // the 1st pick to start fetching from (1 is the 1st pick overall)
    req.setCount (10);   // number of results desired in returned data; max = 100

    BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] Start of getDraftHistory test, fid=%d", mId, franId);
    TimeValue startTime = TimeValue::getTimeOfDay();
    rpcResult = mProxy->getDraftHistory (&req, &resp);
    TimeValue endTime = TimeValue::getTimeOfDay();
    mOwner->addMetric(FranchiseModule::METRIC_GET_DRAFT_HISTORY, endTime - startTime);
    PROCESS_ERROR (rpcResult, "getDraftHistoryLocal", FranchiseModule::METRIC_GET_DRAFT_HISTORY);
    gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs

    return rpcResult;
}

BlazeRpcError FranchiseInstance::getDraftableAthletesLocal (FranchiseId franId)
{
    BlazeRpcError rpcResult = ERR_OK;
    Franchise::GetDraftableAthletesRequest  req;
    Franchise::GetDraftableAthletesResponse resp;

    req.setFranchiseId (franId);
    req.setFirstAthlete (1); // must be 1 or more or server fn could fail
    req.setNumAthletes (30); // return this many
    req.setPositionFilter (DRAFTABLE_POSITION_ALL);
    req.setSearchByColumn (DRAFTABLE_SEARCH_SPEED);
    req.setSearchText ("");  // ?? only need this if searchbycolumn = DRAFTABLE_SEARCH_LASTNAME

    BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] Start of getDraftableAthletes test, fid=%d", mId, franId);
    TimeValue startTime = TimeValue::getTimeOfDay();
    rpcResult = mProxy->getDraftableAthletes (&req, &resp);
    TimeValue endTime = TimeValue::getTimeOfDay();
    mOwner->addMetric(FranchiseModule::METRIC_GET_DRAFTABLE_ATHLETES, endTime - startTime);

    // Let's store all the draftable athletes away into a list we can get at later.
    GetDraftableAthletesResponse::DraftableAthleteList* respList = resp.getDraftableList();
    if (!respList->empty())
    {
        GetDraftableAthletesResponse::DraftableAthleteList::iterator iter = respList->begin();
        for (; iter != respList->end(); iter++)
        {
            mDraftableAthletesList.push_back (*iter);
        }
        //mDraftableAthletesList = *respList;  // oper= doesn't work here
    }

    //uint16_t numFound = resp.getNumAthletes ();// drl debug only
    //uint16_t totalFound = resp.getTotalAthletes ();// drl debug only
    //size_t numEles = respList->size (); // drl debug only
    //size_t numEles2 = mDraftableAthletesList.size (); // drl debug only
    //BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] numFound = %d, total=%d, size1 = %d, size2 = %d",
    //    mId, numFound, totalFound, numEles, numEles2); // drl debug only

    PROCESS_ERROR (rpcResult, "getDraftableAthletesLocal", FranchiseModule::METRIC_GET_DRAFTABLE_ATHLETES);
    gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs

    return rpcResult;
}

BlazeRpcError FranchiseInstance::getDraftOrderLocal (FranchiseId franId)
{
    BlazeRpcError rpcResult = ERR_OK;
    Franchise::GetDraftOrderRequest  req;
    Franchise::GetDraftOrderResponse resp;

    req.setFranchiseId (franId);

    BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] Start of getDraftOrder test, fid=%d", mId, franId);
    TimeValue startTime = TimeValue::getTimeOfDay();
    rpcResult = mProxy->getDraftOrder (&req, &resp);
    TimeValue endTime = TimeValue::getTimeOfDay();
    mOwner->addMetric(FranchiseModule::METRIC_GET_DRAFT_ORDER, endTime - startTime);
    PROCESS_ERROR (rpcResult, "getDraftOrderLocal", FranchiseModule::METRIC_GET_DRAFT_ORDER);
    gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs

    return rpcResult;
}

BlazeRpcError FranchiseInstance::joinDraft (Franchise::JoinDraftRequest& req, Franchise::JoinDraftResponse& resp)
{
    BlazeRpcError rpcResult = ERR_OK;

    BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] Start of joinDraft test, fid=%d", mId, req.getFranchiseId());
    TimeValue startTime = TimeValue::getTimeOfDay();
    rpcResult = mProxy->joinDraft (&req, &resp);
    TimeValue endTime = TimeValue::getTimeOfDay();
    mOwner->addMetric(FranchiseModule::METRIC_JOIN_DRAFT, endTime - startTime);

    // This cmd gives us back some values we'll need for other rpcs.
    if (rpcResult == ERR_OK)
    {
        mDraftMgrId  = resp.getDraftManagerId ();
        mDraftRound  = resp.getRound ();
        mDraftSeqNum = resp.getSeqNum ();
        BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] joinDraft results:  mgrId=%d, round=%d, seqNum=%d, fid=%d",
            mId, mDraftMgrId, mDraftRound, mDraftSeqNum, req.getFranchiseId());
    }

    PROCESS_ERROR (rpcResult, "joinDraft", FranchiseModule::METRIC_JOIN_DRAFT);
    gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs

    return rpcResult;
}


BlazeRpcError FranchiseInstance::joinDraftLocal (FranchiseId franId)
{
    BlazeRpcError rpcResult = ERR_OK;
    Franchise::JoinDraftRequest  req;
    Franchise::JoinDraftResponse resp;

    mDraftFranId = getFranchiseWithFantasyDraftSetting (1, true, mBlazeId); //getGMFranchiseIdForUser ();
    if (mDraftFranId == 0)
    {
        mDraftFranId = franId; // default back to this one if above calls fall
    }
    req.setFranchiseId (mDraftFranId);

    BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] Start of joinDraft test, fid=%d", mId, mDraftFranId);
    TimeValue startTime = TimeValue::getTimeOfDay();
    rpcResult = mProxy->joinDraft (&req, &resp);
    TimeValue endTime = TimeValue::getTimeOfDay();
    mOwner->addMetric(FranchiseModule::METRIC_JOIN_DRAFT, endTime - startTime);

    // This cmd gives us back some values we'll need for other rpcs.
    if (rpcResult == ERR_OK)
    {
        mDraftMgrId  = resp.getDraftManagerId ();
        mDraftRound  = resp.getRound ();
        mDraftSeqNum = resp.getSeqNum ();
        BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] joinDraft results:  mgrId=%d, round=%d, seqNum=%d, fid=%d",
            mId, mDraftMgrId, mDraftRound, mDraftSeqNum, mDraftFranId);
    }

    PROCESS_ERROR (rpcResult, "joinDraftLocal", FranchiseModule::METRIC_JOIN_DRAFT);
    gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs

    return rpcResult;
}

BlazeRpcError FranchiseInstance::leaveDraftLocal (FranchiseId franId)
{
    BlazeRpcError rpcResult = ERR_OK;
    Franchise::LeaveDraftRequest  req;

    req.setFranchiseId (franId);

    BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] Start of leaveDraft test, fid=%d", mId, franId);
    TimeValue startTime = TimeValue::getTimeOfDay();
    rpcResult = mProxy->leaveDraft (&req);
    TimeValue endTime = TimeValue::getTimeOfDay();
    mOwner->addMetric(FranchiseModule::METRIC_LEAVE_DRAFT, endTime - startTime);
    PROCESS_ERROR (rpcResult, "leaveDraftLocal", FranchiseModule::METRIC_LEAVE_DRAFT);
    gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs

    return rpcResult;
}

BlazeRpcError FranchiseInstance::beginDraft(Franchise::BeginDraftRequest& req)
{
    BlazeRpcError rpcResult = ERR_OK;

    BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] Start of beginDraft test, fid=%d", mId, req.getFranchiseId());
    TimeValue startTime = TimeValue::getTimeOfDay();
    rpcResult = mProxy->beginDraft (&req);
    TimeValue endTime = TimeValue::getTimeOfDay();
    mOwner->addMetric(FranchiseModule::METRIC_BEGIN_DRAFT, endTime - startTime);
    PROCESS_ERROR (rpcResult, "beginDraft", FranchiseModule::METRIC_BEGIN_DRAFT);
    gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs

    return rpcResult;
}


BlazeRpcError FranchiseInstance::beginDraftLocal (FranchiseId franId)
{
    BlazeRpcError rpcResult = ERR_OK;
    Franchise::BeginDraftRequest  req;

    req.setFranchiseId (franId);

    BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] Start of beginDraft test, fid=%d", mId, franId);
    TimeValue startTime = TimeValue::getTimeOfDay();
    rpcResult = mProxy->beginDraft (&req);
    TimeValue endTime = TimeValue::getTimeOfDay();
    mOwner->addMetric(FranchiseModule::METRIC_BEGIN_DRAFT, endTime - startTime);
    PROCESS_ERROR (rpcResult, "beginDraftLocal", FranchiseModule::METRIC_BEGIN_DRAFT);
    gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs

    return rpcResult;
}

// Can only pause a 'live' draft.  beginDraft will un-pause.
BlazeRpcError FranchiseInstance::pauseDraftLocal (FranchiseId franId)
{
    BlazeRpcError rpcResult = ERR_OK;
    Franchise::PauseDraftRequest  req;

    req.setFranchiseId (franId);

    BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] Start of pauseDraft test, fid=%d", mId, franId);
    TimeValue startTime = TimeValue::getTimeOfDay();
    rpcResult = mProxy->pauseDraft (&req);
    TimeValue endTime = TimeValue::getTimeOfDay();
    mOwner->addMetric(FranchiseModule::METRIC_PAUSE_DRAFT, endTime - startTime);
    PROCESS_ERROR (rpcResult, "pauseDraftLocal", FranchiseModule::METRIC_PAUSE_DRAFT);
    gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs

    return rpcResult;
}

// It must be your 'turn' to draft for this to work, plus the athlete must be
// draftable.
BlazeRpcError FranchiseInstance::draftAthleteLocal (FranchiseId franId)
{
    BlazeRpcError rpcResult = ERR_OK;
    Franchise::DraftAthleteRequest  req;

    if (mDraftableAthletesList.empty())
        return FRANCHISE_ERR_ATHLETE_NOT_ELIGIBLE;

    // Lets get an athlete id from the list data.
    Franchise::GetDraftableAthletesResponse::DraftableAthleteList::iterator iter = mDraftableAthletesList.begin();
    if (iter == mDraftableAthletesList.end())
    {
        return FRANCHISE_ERR_ATHLETE_NOT_ELIGIBLE;
    }

    AthleteId athleteId = (*iter)->getAthleteId ();
    if (athleteId == 0)
    {
        athleteId = ATHLETE_ID_INVALID; // the server checks for this but not for 0
    }

    req.setFranchiseId (franId);
    req.setAthleteId (athleteId); // pick one draftable athlete

    BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] Start of draftAthlete test, fid=%d", mId, franId);
    TimeValue startTime = TimeValue::getTimeOfDay();
    rpcResult = mProxy->draftAthlete (&req);
    TimeValue endTime = TimeValue::getTimeOfDay();
    mOwner->addMetric(FranchiseModule::METRIC_DRAFT_ATHLETE, endTime - startTime);
    PROCESS_ERROR (rpcResult, "draftAthleteLocal", FranchiseModule::METRIC_DRAFT_ATHLETE);
    gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs

    return rpcResult;
}

BlazeRpcError FranchiseInstance::sendDraftChatLocal (FranchiseId franId)
{
    BlazeRpcError rpcResult = ERR_OK;
    Franchise::SendDraftChatRequest  req;

    req.setFranchiseId (franId);
    req.setChatText ("Hi there from the stress app.  I'm chatty.");

    BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] Start of sendDraftChat test, fid=%d", mId, franId);
    TimeValue startTime = TimeValue::getTimeOfDay();
    rpcResult = mProxy->sendDraftChat (&req);
    TimeValue endTime = TimeValue::getTimeOfDay();
    mOwner->addMetric(FranchiseModule::METRIC_SEND_DRAFT_CHAT, endTime - startTime);
    PROCESS_ERROR (rpcResult, "sendDraftChatLocal", FranchiseModule::METRIC_SEND_DRAFT_CHAT);
    gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs

    return rpcResult;
}

BlazeRpcError FranchiseInstance::pollDraftLocal (FranchiseId franId)
{
    BlazeRpcError rpcResult = ERR_OK;
    Franchise::PollDraftRequest  req;
    Franchise::PollDraftResponse resp;
    bool done = false;

    while (!done)
    {
        req.setFranchiseId (franId);
        req.setDraftManagerId (mDraftMgrId); // grabbed from joinDraft
        req.setSeqNum (mDraftSeqNum);        // sequence # of the 1st desired notification; use 1st seq
                                             // # from joinDraft response; subsequent calls use # 1 gtr
                                             // then the most recently received notification
        req.setCount (20);                   // the number of notifications to retrieve; max = 100

        BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] Start of pollDraft test, fid=%d", mId, franId);
        TimeValue startTime = TimeValue::getTimeOfDay();
        rpcResult = mProxy->pollDraft (&req, &resp);
        TimeValue endTime = TimeValue::getTimeOfDay();
        mOwner->addMetric(FranchiseModule::METRIC_POLL_DRAFT, endTime - startTime);
        PROCESS_ERROR (rpcResult, "pollDraftLocal", FranchiseModule::METRIC_POLL_DRAFT);

        if (rpcResult != ERR_OK) // bail out if we get a hard error
        {
            done = true;
        }

        // Let's see what the response has in it.
        PollDraftResponse::DraftNotificationList* notList = resp.getNotifications();
        if (notList != NULL && rpcResult == ERR_OK)
        {
            //BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] got the notList", mId); // drl debug
            DraftStatus status = DRAFT_STATUS_INITIALIZING;
            uint32_t seqNum = 0;
            PollDraftResponse::DraftNotificationList::iterator iter = notList->begin();
            for (; iter != notList->end(); iter++)
            {
                seqNum = (*iter)->getSeqNum();
                //BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] inside loop, seqNum=%d", mId, seqNum); // drl debug
                DraftNotificationUnion* draftNotUnion = (*iter)->getNotification();

                if (draftNotUnion)
                {
                    //BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] inside draftNotUnion, seqnum=%d", mId, seqNum); // drl debug
                    DraftStatusNotification* statusNotification = draftNotUnion->getStatus();
                    if (statusNotification)
                    {
                        status = statusNotification->getStatus();
                        BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] pollDraft results: fid=%d, status=%d, seqNum=%d, timeLimit=%d",
                            mId, statusNotification->getFranchiseId(), status,
                            seqNum, statusNotification->getDraftTimeLimit());
                    }
                }
            }
            if (status != DRAFT_STATUS_COMPLETED)
            {
                BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] we're LIVE, going to sleep...", mId); // drl debug
                mDraftSeqNum = seqNum + 1;
                gSelector->sleep (5000000); // wait 5 secs and poll again; arg is in microsecs
            }
            else
            {
                BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] we're done, status = %d", mId, status); // drl debug
                done = true;
            }
        }
    }
    //BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] totally done", mId); // drl debug

    gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs

    return rpcResult;
}

BlazeRpcError FranchiseInstance::checkDraftLocal (FranchiseId franId)
{
    BlazeRpcError rpcResult = ERR_OK;
    Franchise::CheckDraftRequest  req;
    Franchise::CheckDraftResponse resp;

    req.setFranchiseId (franId);

    BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] Start of checkDraft test, fid=%d", mId, franId);
    TimeValue startTime = TimeValue::getTimeOfDay();
    rpcResult = mProxy->checkDraft (&req, &resp);
    TimeValue endTime = TimeValue::getTimeOfDay();
    mOwner->addMetric(FranchiseModule::METRIC_CHECK_DRAFT, endTime - startTime);
    PROCESS_ERROR (rpcResult, "checkDraftLocal", FranchiseModule::METRIC_CHECK_DRAFT);

    uint8_t activeState = resp.getIsDraftActive ();
    if (activeState == 0)
    {
        rpcResult = FRANCHISE_ERR_DRAFT_NOT_LIVE;
    }
    BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] checkDraft - DRAFT IS %s", mId, (activeState ? "LIVE" : "NOT LIVE"));

    gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs
    return rpcResult;
}

BlazeRpcError FranchiseInstance::getTopFranchisesLocal (FranchiseId franId)
{
    BlazeRpcError rpcResult = ERR_OK;
    Franchise::GetTopFranchisesRequest  req;
    Franchise::GetTopFranchisesResponse resp;

    req.setMaxResults (5);

    BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] Start of getTopFranchises test, fid=%d", mId, franId);
    TimeValue startTime = TimeValue::getTimeOfDay();
    rpcResult = mProxy->getTopFranchises (&req, &resp);
    TimeValue endTime = TimeValue::getTimeOfDay();
    mOwner->addMetric(FranchiseModule::METRIC_GET_TOP_FRANCHISES, endTime - startTime);
    PROCESS_ERROR (rpcResult, "getTopFranchisesLocal", FranchiseModule::METRIC_GET_TOP_FRANCHISES);
    gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs

    return rpcResult;
}

BlazeRpcError FranchiseInstance::getFranchiseRankLocal (FranchiseId franId)
{
    BlazeRpcError rpcResult = ERR_OK;
    Franchise::GetFranchiseRankRequest  req;
    Franchise::FranchiseRank resp;

    req.setFranchiseId (franId);

    BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] Start of getFranchiseRank test, fid=%d", mId, franId);
    TimeValue startTime = TimeValue::getTimeOfDay();
    rpcResult = mProxy->getFranchiseRank (&req, &resp);
    TimeValue endTime = TimeValue::getTimeOfDay();
    mOwner->addMetric(FranchiseModule::METRIC_GET_FRANCHISE_RANK, endTime - startTime);
    PROCESS_ERROR (rpcResult, "getFranchiseRankLocal", FranchiseModule::METRIC_GET_FRANCHISE_RANK);
    gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs

    return rpcResult;
}

BlazeRpcError FranchiseInstance::validateRostersLocal (FranchiseId franId)
{
    BlazeRpcError rpcResult = ERR_OK;
    Franchise::ValidateRostersRequest  req;

    req.setFranchiseId (franId);

    BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] Start of validateRosters test, fid=%d", mId, franId);
    TimeValue startTime = TimeValue::getTimeOfDay();
    rpcResult = mProxy->validateRosters (&req);
    TimeValue endTime = TimeValue::getTimeOfDay();
    mOwner->addMetric(FranchiseModule::METRIC_VALIDATE_ROSTERS, endTime - startTime);
    PROCESS_ERROR (rpcResult, "validateRostersLocal", FranchiseModule::METRIC_VALIDATE_ROSTERS);
    gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs

    return rpcResult;
}

BlazeRpcError FranchiseInstance::validateRosters (Franchise::ValidateRostersRequest& req)
{
    BlazeRpcError rpcResult = ERR_OK;

    BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] Start of validateRosters test, fid=%d", mId, req.getFranchiseId());
    TimeValue startTime = TimeValue::getTimeOfDay();
    rpcResult = mProxy->validateRosters (&req);
    TimeValue endTime = TimeValue::getTimeOfDay();
    mOwner->addMetric(FranchiseModule::METRIC_VALIDATE_ROSTERS, endTime - startTime);
    PROCESS_ERROR (rpcResult, "validateRosters", FranchiseModule::METRIC_VALIDATE_ROSTERS);
    gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs

    return rpcResult;
}

BlazeRpcError FranchiseInstance::validateDepthChartsLocal (FranchiseId franId)
{
    BlazeRpcError rpcResult = ERR_OK;
    Franchise::ValidateDepthChartsRequest  req;

    req.setFranchiseId (franId);

    BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] Start of validateDepthCharts test, fid=%d", mId, franId);
    TimeValue startTime = TimeValue::getTimeOfDay();
    rpcResult = mProxy->validateDepthCharts (&req);
    TimeValue endTime = TimeValue::getTimeOfDay();
    mOwner->addMetric(FranchiseModule::METRIC_VALIDATE_DEPTH_CHARTS, endTime - startTime);
    PROCESS_ERROR (rpcResult, "validateDepthChartsLocal", FranchiseModule::METRIC_VALIDATE_DEPTH_CHARTS);
    gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs

    return rpcResult;
}

BlazeRpcError FranchiseInstance::submitFranchiseGameStatsLocal (FranchiseId franId)
{
    BlazeRpcError rpcResult = ERR_OK;
    Franchise::SubmitFranchiseGameStatsRequest  req;
    Franchise::SubmitFranchiseGameStatsRequest::Uint32_tList* entList  = req.getEntities ();
    Franchise::SubmitFranchiseGameStatsRequest::Int32_tList*  statList = req.getStats ();

    req.setFranchiseId (franId);
    req.setCategory ("FranchiseTeamStats"); // ??
    req.setNumEntities (2);
    req.setNumStats (12); // must have at least this amt if using category = FranchiseTeamStats
    req.setGameId (1);
    req.setState (FRANCHISE_STATE_REGULAR_SEASON); // use this normally
    entList->push_back (5);
    entList->push_back (6);
    statList->push_back (7);
    statList->push_back (7);
    statList->push_back (10);
    statList->push_back (3);
    statList->push_back (0);
    statList->push_back (0);
    statList->push_back (3);
    statList->push_back (7);
    statList->push_back (10);
    statList->push_back (3);
    statList->push_back (0);
    statList->push_back (0);

    BLAZE_INFO (BlazeRpcLog::franchise, "[%d] Start of submitFranchiseGameStats test, fid=%d, bzid=%d", mId, franId, mBlazeId);
    TimeValue startTime = TimeValue::getTimeOfDay();
    rpcResult = mProxy->submitFranchiseGameStats (&req);
    TimeValue endTime = TimeValue::getTimeOfDay();
    mOwner->addMetric(FranchiseModule::METRIC_SUBMIT_FRANCHISE_GAME_STATS, endTime - startTime);
    PROCESS_ERROR (rpcResult, "submitFranchiseGameStatsLocal", FranchiseModule::METRIC_SUBMIT_FRANCHISE_GAME_STATS);
    gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs

    return rpcResult;
}


void FranchiseInstance::fillFranchiseTeamStats(Franchise::SubmitFranchiseGameStatsRequest& req, Franchise::GetGameplayInformationResponse& ggiResp)
{
    Franchise::SubmitFranchiseGameStatsRequest::Int32_tList* statsList = req.getStats();
    StatsTables* statsTable = mOwner->getStatsTable();
    if (statsTable != NULL && !statsTable->GetRegularSchedule().empty())
    {
        const IntArray &stats = statsTable->GetRegularSchedule()[rand() % statsTable->GetRegularSchedule().size()];
        // We are sending (stats.size() - 1) because
        // we are ignoring the last column (athleteId) of the csv file
        req.setNumStats((int32_t)stats.size()-1);
        for (int32_t i = 0; i< (int32_t)stats.size()-1; i++)
        {
            statsList->push_back(stats[i]);
        }
    }
}


void FranchiseInstance::internalFillPlayerStats(Franchise::SubmitFranchiseGameStatsRequest& req, 
                                                Franchise::GetGameplayInformationResponse& ggiResp,
                                                int randomAthleteCount1,
                                                int randomAthleteCount2,
                                                const eastl::vector<IntArray> &statArray)
{
    Franchise::SubmitFranchiseGameStatsRequest::Int32_tList* statsList = req.getStats();
    Franchise::SubmitFranchiseGameStatsRequest::Uint32_tList* entitiesList = req.getEntities();

    const IntArray &stats = statArray[0];

    req.setNumEntities(randomAthleteCount1 + randomAthleteCount2);
    req.setNumStats((int32_t)stats.size() );

    Franchise::TeamGameplayInformation* team1GamePlayInfo = ggiResp.getTeam1();
    Franchise::TeamGameplayInformation::AthletePropertiesList* team1AthleteProperties = team1GamePlayInfo->getAthletePropertyList();
    Franchise::TeamGameplayInformation::AthletePropertiesList::iterator team1Iter = team1AthleteProperties->begin();

    for (; team1Iter !=  team1AthleteProperties->end(); team1Iter++)
    {
        entitiesList->push_back((*team1Iter)->getAthleteId());

        const IntArray &stats1 = statArray[rand() % statArray.size()];
        for (int32_t i = 0; i< (int32_t)stats1.size(); i++)
        {
            statsList->push_back(stats1[i]);
        }

        randomAthleteCount1--;
        if (randomAthleteCount1 == 0)
        {
            break;
        }
    }

    Franchise::TeamGameplayInformation* team2GamePlayInfo = ggiResp.getTeam2();
    Franchise::TeamGameplayInformation::AthletePropertiesList* team2AthleteProperties = team2GamePlayInfo->getAthletePropertyList();
    Franchise::TeamGameplayInformation::AthletePropertiesList::iterator team2Iter = team2AthleteProperties->begin();

    for (; team2Iter !=  team2AthleteProperties->end(); team2Iter++)
    {
        entitiesList->push_back((*team2Iter)->getAthleteId());

        const IntArray &stats1 = statArray[rand() % statArray.size()];
        for (int32_t i = 0; i< (int32_t)stats1.size(); i++)
        {
            statsList->push_back(stats1[i]);
        }

        randomAthleteCount2--;
        if (randomAthleteCount2 == 0)
        {
            break;
        }
    }
}


void FranchiseInstance::fillInjuryPlayerStats(Franchise::SubmitFranchiseGameStatsRequest& req, Franchise::GetGameplayInformationResponse& ggiResp)
{
    int32_t randomAthleteCount1 = (rand() % 2) + 1;
    int32_t randomAthleteCount2 = (rand() % 2) + 1;

    StatsTables* statsTable = mOwner->getStatsTable();
    if (statsTable != NULL && !statsTable->GetInjury().empty())
    {
        Franchise::SubmitFranchiseGameStatsRequest::Int32_tList* statsList = req.getStats();
        Franchise::SubmitFranchiseGameStatsRequest::Uint32_tList* entitiesList = req.getEntities();

        const IntArray &stats = statsTable->GetInjury()[0];

        req.setNumEntities(randomAthleteCount1 + randomAthleteCount2);
        req.setNumStats((int32_t)stats.size() );

        Franchise::TeamGameplayInformation* team1GamePlayInfo = ggiResp.getTeam1();
        Franchise::TeamGameplayInformation::AthletePropertiesList* team1AthleteProperties = team1GamePlayInfo->getAthletePropertyList();
        Franchise::TeamGameplayInformation::AthletePropertiesList::iterator team1Iter = team1AthleteProperties->begin();

        for (; team1Iter !=  team1AthleteProperties->end(); team1Iter++)
        {
            entitiesList->push_back((*team1Iter)->getAthleteId());
            statsList->push_back(team1GamePlayInfo->getTeamId());
            const IntArray &stats1 = statsTable->GetInjury()[rand() % statsTable->GetInjury().size()];
            for (int32_t i = 0; i< (int32_t)stats1.size(); i++)
            {
                statsList->push_back(stats1[i]);
            }

            randomAthleteCount1--;
            if (randomAthleteCount1 == 0)
            {
                break;
            }
        }

        Franchise::TeamGameplayInformation* team2GamePlayInfo = ggiResp.getTeam2();
        Franchise::TeamGameplayInformation::AthletePropertiesList* team2AthleteProperties = team2GamePlayInfo->getAthletePropertyList();
        Franchise::TeamGameplayInformation::AthletePropertiesList::iterator team2Iter = team2AthleteProperties->begin();

        for (; team2Iter !=  team2AthleteProperties->end(); team2Iter++)
        {
            entitiesList->push_back((*team2Iter)->getAthleteId());
            statsList->push_back(team2GamePlayInfo->getTeamId());
            const IntArray &stats1 = statsTable->GetInjury()[rand() % statsTable->GetInjury().size()];
            for (int32_t i = 0; i< (int32_t)stats1.size(); i++)
            {
                statsList->push_back(stats1[i]);
            }

            randomAthleteCount2--;
            if (randomAthleteCount2 == 0)
            {
                break;
            }
        }
    }
}


void FranchiseInstance::fillOffensivePlayerStat(Franchise::SubmitFranchiseGameStatsRequest& req, Franchise::GetGameplayInformationResponse& ggiResp)
{
    int32_t randomAthleteCount1 = (rand() % 8) + 7;
    int32_t randomAthleteCount2 = (rand() % 8) + 7;

    StatsTables* statsTable = mOwner->getStatsTable();
    if (statsTable != NULL && !statsTable->GetOffensivePlayer().empty())
    {
        internalFillPlayerStats(req, ggiResp, randomAthleteCount1, randomAthleteCount2, statsTable->GetOffensivePlayer());
    }
}

void FranchiseInstance::fillOffensiveLinePlayerStat(Franchise::SubmitFranchiseGameStatsRequest& req, Franchise::GetGameplayInformationResponse& ggiResp)
{
    int32_t randomAthleteCount1 = (rand() % 7) + 4;
    int32_t randomAthleteCount2 = (rand() % 7) + 4;

    StatsTables* statsTable = mOwner->getStatsTable();
    if (statsTable != NULL && !statsTable->GetOffensiveLinePlayer().empty())
    {
        internalFillPlayerStats(req, ggiResp, randomAthleteCount1, randomAthleteCount2, statsTable->GetOffensiveLinePlayer());
    }
}

void FranchiseInstance::fillDefensivePlayerStats(Franchise::SubmitFranchiseGameStatsRequest& req, Franchise::GetGameplayInformationResponse& ggiResp)
{
    int32_t randomAthleteCount1 = (rand() % 5) + 10;
    int32_t randomAthleteCount2 = (rand() % 5) + 10;

    StatsTables* statsTable = mOwner->getStatsTable();
    if (statsTable != NULL && !statsTable->GetDefensivePlayer().empty())
    {
        internalFillPlayerStats(req, ggiResp, randomAthleteCount1, randomAthleteCount2, statsTable->GetDefensivePlayer());
    }
}
void FranchiseInstance::fillKickPlayerStats(Franchise::SubmitFranchiseGameStatsRequest& req, Franchise::GetGameplayInformationResponse& ggiResp)
{
    int32_t randomAthleteCount1 = (rand() % 2) + 2;
    int32_t randomAthleteCount2 = (rand() % 2) + 2;

    StatsTables* statsTable = mOwner->getStatsTable();
    if (statsTable != NULL && !statsTable->GetKickPlayer().empty())
    {
        internalFillPlayerStats(req, ggiResp, randomAthleteCount1, randomAthleteCount2, statsTable->GetKickPlayer());
    }
}
void FranchiseInstance::fillKickReturnPlayerStats(Franchise::SubmitFranchiseGameStatsRequest& req, Franchise::GetGameplayInformationResponse& ggiResp)
{
    int32_t randomAthleteCount1 = (rand() % 2) + 2;
    int32_t randomAthleteCount2 = (rand() % 2) + 2;

    StatsTables* statsTable = mOwner->getStatsTable();
    if (statsTable != NULL && !statsTable->GetKickReturnPlayer().empty())
    {
        internalFillPlayerStats(req, ggiResp, randomAthleteCount1, randomAthleteCount2, statsTable->GetKickReturnPlayer());
    }
}
void FranchiseInstance::fillFieldPlayerStat(Franchise::SubmitFranchiseGameStatsRequest& req, Franchise::GetGameplayInformationResponse& ggiResp)
{
    int32_t randomAthleteCount1 = (rand() % 30) + 15;
    int32_t randomAthleteCount2 = (rand() % 30) + 15;

    StatsTables* statsTable = mOwner->getStatsTable();
    if (statsTable != NULL && !statsTable->GetFieldPlayer().empty())
    {
        internalFillPlayerStats(req, ggiResp, randomAthleteCount1, randomAthleteCount2, statsTable->GetFieldPlayer());
    }
}


void FranchiseInstance::fillSubmitFranchiseGameStatsRequest(Franchise::SubmitFranchiseGameStatsRequest& req, Franchise::GetGameplayInformationResponse& ggiResp)
{
    const char8_t* category = req.getCategory();
    if (blaze_stricmp(category, "FranchiseTeamStats") == 0)
    {
        fillFranchiseTeamStats(req, ggiResp);
    }
    else if (blaze_stricmp(category, "InjuryPlayerStats") == 0)
    {
        fillInjuryPlayerStats(req, ggiResp);
    }
    else if (blaze_stricmp(category, "OffensivePlayerStats") == 0)
    {
        fillOffensivePlayerStat(req, ggiResp);
    }
    else if (blaze_stricmp(category, "OffensiveLinePlayerStats") == 0)
    {
        fillOffensiveLinePlayerStat(req, ggiResp);
    }
    else if (blaze_stricmp(category, "DefensivePlayerStats") == 0)
    {
        fillDefensivePlayerStats(req, ggiResp);
    }
    else if (blaze_stricmp(category, "KickPlayerStats") == 0)
    {
        fillKickPlayerStats(req, ggiResp);
    }
    else if (blaze_stricmp(category, "KickReturnPlayerStats") == 0)
    {
        fillKickReturnPlayerStats(req, ggiResp);
    }
    else if (blaze_stricmp(category, "FieldPlayerStats") == 0)
    {
        fillFieldPlayerStat(req, ggiResp);
    }

}


BlazeRpcError FranchiseInstance::submitFranchiseGameStats (Franchise::SubmitFranchiseGameStatsRequest& req, Franchise::GetGameplayInformationResponse& ggiResp)
{
    BlazeRpcError rpcResult = ERR_OK;

    fillSubmitFranchiseGameStatsRequest(req, ggiResp);

    BLAZE_INFO (BlazeRpcLog::franchise, "[%d] Start of submitFranchiseGameStats test, fid=%d, bzid=%d, category=%s, seasonid=%d, weekid=%d, gameid=%d", 
                        mId, req.getFranchiseId(), mBlazeId, req.getCategory(), req.getSeasonId(), req.getWindowId(), req.getGameId());
    TimeValue startTime = TimeValue::getTimeOfDay();
    rpcResult = mProxy->submitFranchiseGameStats (&req);
    TimeValue endTime = TimeValue::getTimeOfDay();
    mOwner->addMetric(FranchiseModule::METRIC_SUBMIT_FRANCHISE_GAME_STATS, endTime - startTime);
    PROCESS_ERROR (rpcResult, "submitFranchiseGameStats", FranchiseModule::METRIC_SUBMIT_FRANCHISE_GAME_STATS);
    gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs

    return rpcResult;
}

BlazeRpcError FranchiseInstance::submitScoringSummaryLocal (FranchiseId franId)
{
    BlazeRpcError rpcResult = ERR_OK;
    Franchise::SubmitScoringSummaryRequest  req;

    req.setFranchiseId (franId);
    req.setGameId (2); // ??

    BLAZE_INFO (BlazeRpcLog::franchise, "[%d] Start of submitScoringSummary test, fid=%d, bzid=%d", mId, franId, mBlazeId);
    TimeValue startTime = TimeValue::getTimeOfDay();
    rpcResult = mProxy->submitScoringSummary (&req);
    TimeValue endTime = TimeValue::getTimeOfDay();
    mOwner->addMetric(FranchiseModule::METRIC_SUBMIT_SCORING_SUMMARY, endTime - startTime);
    PROCESS_ERROR (rpcResult, "submitScoringSummaryLocal", FranchiseModule::METRIC_SUBMIT_SCORING_SUMMARY);
    gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs

    return rpcResult;
}

BlazeRpcError FranchiseInstance::submitScoringSummary (Franchise::SubmitScoringSummaryRequest& req, Franchise::TeamId homeTeamId, Franchise::TeamId awayTeamId)
{
    BlazeRpcError rpcResult = ERR_OK;

    enum { MAX_SCORES = 10 };
    StatsTables &stats = StatsTables::GetInstance();

    for (int i = 0, sz = (rand() % MAX_SCORES) + 1; i < sz; i++)
    {
        const ScoringSummary &summary = stats.GetScoringSummary()[rand() % stats.GetScoringSummary().size()];
        ScoreDescription *score = new ScoreDescription;
        score->setOrder(i);
        score->setPeriod((Franchise::PeriodType)summary.mStat0);
        score->setDescription(summary.mStat1.c_str());
        score->setHomeScore((uint16_t)summary.mStat2);
        score->setAwayScore((uint16_t)summary.mStat3);
        score->setGameId(req.getGameId());
        score->setTeamId(rand() % 2 ? homeTeamId : awayTeamId);
        printf("Score: Order %d, Period %d, HomeScore %d, AwayScore %d, GameId %d, Description '%s'\n", 
             score->getOrder(), score->getPeriod(), score->getHomeScore(), score->getAwayScore(), score->getGameId(), score->getDescription());
        req.getScoreDescriptionList()->push_back(score);
    }

    BLAZE_INFO (BlazeRpcLog::franchise, "[%d] Start of submitScoringSummary test, fid=%d, bzid=%d, weekid=%d, seasonid=%d, gameid=%d",
        mId, req.getFranchiseId(), mBlazeId, req.getWindowId(), req.getSeasonId(), req.getGameId());

    TimeValue startTime = TimeValue::getTimeOfDay();
    rpcResult = mProxy->submitScoringSummary (&req);
    TimeValue endTime = TimeValue::getTimeOfDay();
    mOwner->addMetric(FranchiseModule::METRIC_SUBMIT_SCORING_SUMMARY, endTime - startTime);
    PROCESS_ERROR (rpcResult, "submitScoringSummary", FranchiseModule::METRIC_SUBMIT_SCORING_SUMMARY);
    gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs

    return rpcResult;
}

BlazeRpcError FranchiseInstance::submitAthleteGameStatsLocal (FranchiseId franId)
{
    BlazeRpcError rpcResult = ERR_OK;
    Franchise::SubmitAthleteGameStatsRequest  req;

    req.setFranchiseId (franId);
    req.setGameId (2); // ??

    BLAZE_INFO (BlazeRpcLog::franchise, "[%d] Start of submitAthleteGameStats test, fid=%d, bzid=%d", mId, franId, mBlazeId);
    TimeValue startTime = TimeValue::getTimeOfDay();
    rpcResult = mProxy->submitAthleteGameStats (&req);
    TimeValue endTime = TimeValue::getTimeOfDay();
    mOwner->addMetric(FranchiseModule::METRIC_SUBMIT_ATHLETE_GAME_STATS, endTime - startTime);
    PROCESS_ERROR (rpcResult, "submitAthleteGameStatsLocal", FranchiseModule::METRIC_SUBMIT_ATHLETE_GAME_STATS);
    gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs

    return rpcResult;
}

void FranchiseInstance::fillAthleteStatsByTeam(AthleteStatsByTeam* athleteStats, Franchise::AthleteId athleteId)
{
    StatsTables* statsTable = mOwner->getStatsTable();
    if (statsTable != NULL && !statsTable->GetAthleteStatsByGame().empty())
    {
        const IntArray &stats = statsTable->GetAthleteStatsByGame()[rand() % statsTable->GetAthleteStatsByGame().size()];
        Franchise::AthleteId randomAthleyeId = (Franchise::AthleteId)(rand() % 45) ;    // 45 players in a team
        int32_t statsType = stats[0];

        // you cannot have a duplicate entry for frachiseid+gameid+athleteid+categoryid
        // need some logic to prevent that
        // for time being we just use unique athelete id passed in by caller
        randomAthleyeId = athleteId;

        // its ok even if one of the stats type list does not have any entry in them
        switch(statsType)
        {
        case ATHLETE_GAME_STAT_TYPE_PASSING:
            {
                Franchise::AthleteStatsByTeam::PassingStatsList *passingStatList = athleteStats->getPassing();
                Franchise::PassingStats* passingStat = new Franchise::PassingStats();
                passingStat->setAthleteId(randomAthleyeId);
                passingStat->setCompletes((uint8_t)stats[1]);
                passingStat->setAttempts((uint8_t)stats[2]);
                passingStat->setYards((int16_t)stats[3]);
                passingStat->setTds((uint8_t)stats[4]);
                passingStat->setInts((uint8_t)stats[5]);
                passingStatList->push_back(passingStat);

                break;
            }
        case ATHLETE_GAME_STAT_TYPE_RUSHING:
            {
                Franchise::AthleteStatsByTeam::RushingStatsList *rushingStatList = athleteStats->getRushing();
                Franchise::RushingStats* rushingStat = new Franchise::RushingStats();
                rushingStat->setAthleteId(randomAthleyeId);
                rushingStat->setAttempts((uint8_t)stats[1]);
                rushingStat->setYards((int16_t)stats[2]);
                rushingStat->setTds((uint8_t)stats[3]);
                rushingStat->setLong((int8_t)stats[4]);
                rushingStatList->push_back(rushingStat);

                break;
            }
        case ATHLETE_GAME_STAT_TYPE_RECEIVING:
            {
                Franchise::AthleteStatsByTeam::ReceivingStatsList *receivingStatList = athleteStats->getReceiving();
                Franchise::ReceivingStats* receivingStat = new Franchise::ReceivingStats();
                receivingStat->setAthleteId(randomAthleyeId);
                receivingStat->setReceptions((uint8_t)stats[1]);
                receivingStat->setYards((int16_t)stats[2]);
                receivingStat->setTds((uint8_t)stats[3]);
                receivingStat->setLong((int8_t)stats[4]);
                receivingStatList->push_back(receivingStat);
                break;
            }
        case ATHLETE_GAME_STAT_TYPE_BLOCKING:
            {
                Franchise::AthleteStatsByTeam::BlockingStatsList *blockingStatList = athleteStats->getBlocking();
                Franchise::BlockingStats* blockingStat = new Franchise::BlockingStats();
                blockingStat->setAthleteId(randomAthleyeId);
                blockingStat->setPancakes((uint8_t)stats[1]);
                blockingStat->setSacksAllowed((uint8_t)stats[2]);
                blockingStatList->push_back(blockingStat);
                break;
            }
        case ATHLETE_GAME_STAT_TYPE_KICKING:
            {
                Franchise::AthleteStatsByTeam::KickingStatsList *kickingStatList = athleteStats->getKicking();
                Franchise::KickingStats* kickingStat = new Franchise::KickingStats();
                kickingStat->setAthleteId(randomAthleyeId);
                kickingStat->setFGAttempts((uint8_t)stats[1]);
                kickingStat->setFGMade((uint8_t)stats[2]);
                kickingStat->setLong((uint8_t)stats[3]);
                kickingStat->setXAttempts((uint8_t)stats[4]);
                kickingStat->setXMade((uint8_t)stats[5]);
                kickingStat->setPoints((uint8_t)stats[6]);
                kickingStatList->push_back(kickingStat);
                break;
            }
        case ATHLETE_GAME_STAT_TYPE_PUNTING:
            {
                Franchise::AthleteStatsByTeam::PuntingStatsList *puntingStatList = athleteStats->getPunting();
                Franchise::PuntingStats* puntingStat = new Franchise::PuntingStats();
                puntingStat->setAthleteId(randomAthleyeId);
                puntingStat->setPunts((uint8_t)stats[1]);
                puntingStat->setAverage((uint8_t)stats[2]);
                puntingStat->setPuntsIn20((uint8_t)stats[3]);
                puntingStat->setLong((uint8_t)stats[4]);
                puntingStatList->push_back(puntingStat);
                break;
            }
        case ATHLETE_GAME_STAT_TYPE_KICKOFFRETURN:
            {
                Franchise::AthleteStatsByTeam::KickoffReturnStatsList * kickoffReturnStatList = athleteStats->getKickoffReturn();
                Franchise::KickoffReturnStats* kickoffReturnStat = new Franchise::KickoffReturnStats();
                kickoffReturnStat->setAthleteId(randomAthleyeId);
                kickoffReturnStat->setReturns((uint8_t)stats[1]);
                kickoffReturnStat->setAverage((int8_t)stats[2]);
                kickoffReturnStat->setTds((uint8_t)stats[3]);
                kickoffReturnStat->setLong((int8_t)stats[4]);
                kickoffReturnStatList->push_back(kickoffReturnStat);

                break;
            }
        case ATHLETE_GAME_STAT_TYPE_PUNTRETURN:
            {
                Franchise::AthleteStatsByTeam::PuntReturnStatsList * puntReturnStatList = athleteStats->getPuntReturn();
                Franchise::PuntReturnStats* puntReturnStat = new Franchise::PuntReturnStats();
                puntReturnStat->setAthleteId(randomAthleyeId);
                puntReturnStat->setReturns((uint8_t)stats[1]);
                puntReturnStat->setAverage((int8_t)stats[2]);
                puntReturnStat->setTds((uint8_t)stats[3]);
                puntReturnStat->setLong((int8_t)stats[4]);
                puntReturnStatList->push_back(puntReturnStat);
                break;
            }
        case ATHLETE_GAME_STAT_TYPE_DEFENSE:
            {
                Franchise::AthleteStatsByTeam::DefenseStatsList * defenceStatList = athleteStats->getDefense();
                Franchise::DefenseStats* defenceStat = new Franchise::DefenseStats();
                defenceStat->setAthleteId(randomAthleyeId);
                defenceStat->setTackles((uint8_t)stats[1]);
                defenceStat->setSacks((uint8_t)stats[2]);
                defenceStat->setInts((uint8_t)stats[3]);
                defenceStat->setForceFumble((uint8_t)stats[4]);
                defenceStatList->push_back(defenceStat);
                break;
            }
        default:
            {
                BLAZE_INFO (BlazeRpcLog::franchise, "[%d] unkmown statsType=%d", mId, statsType);
                break;
            }
        }
    }
}

void FranchiseInstance::fillSubmitAthleteGameStatsRequest(Franchise::SubmitAthleteGameStatsRequest& req)
{
    AthleteStatsByTeam* athleteStatsT1 = req.getAthleteStatsT1();
    int32_t randomNumEntries1 = (rand() % 40) + 1 ;    // 40 is just a random number
    for (int32_t i=0; i < randomNumEntries1; i++)      
    {
        fillAthleteStatsByTeam(athleteStatsT1, (Franchise::AthleteId)i);
    }

    int32_t randomNumEntries2 = (rand() % 40) + 1 ;    // 40 is just a random number
    AthleteStatsByTeam* athleteStatsT2 = req.getAthleteStatsT2();
    for (int32_t i=0; i < randomNumEntries2; i++)      // 40 is just a random number
    {
        fillAthleteStatsByTeam(athleteStatsT2, (Franchise::AthleteId) (randomNumEntries1 + i));
    }
}

BlazeRpcError FranchiseInstance::submitAthleteGameStats (Franchise::SubmitAthleteGameStatsRequest& req)
{
    BlazeRpcError rpcResult = ERR_OK;

    fillSubmitAthleteGameStatsRequest(req);

    BLAZE_INFO (BlazeRpcLog::franchise, "[%d] Start of submitAthleteGameStats test, fid=%d, bzid=%d, weekid=%d, seasonid=%d, gameid=%d",
        mId, req.getFranchiseId(), mBlazeId, req.getWindowId(), req.getSeasonId(), req.getGameId());
    TimeValue startTime = TimeValue::getTimeOfDay();
    rpcResult = mProxy->submitAthleteGameStats (&req);
    TimeValue endTime = TimeValue::getTimeOfDay();
    mOwner->addMetric(FranchiseModule::METRIC_SUBMIT_ATHLETE_GAME_STATS, endTime - startTime);
    PROCESS_ERROR (rpcResult, "submitAthleteGameStats", FranchiseModule::METRIC_SUBMIT_ATHLETE_GAME_STATS);
    gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs

    return rpcResult;
}

BlazeRpcError FranchiseInstance::submitTeamGameStatsLocal (FranchiseId franId)
{
    BlazeRpcError rpcResult = ERR_OK;
    Franchise::SubmitTeamGameStatsRequest  req;
    TeamStats* stats1;
    TeamStats* stats2;

    req.setFranchiseId (franId);
    req.setGameId (2);
    stats1 = req.getTeam1GameStats ();
    stats1->setTeamId (5);
    stats1->setScore (7);
    stats1->setTotalOffense (20);
    stats1->setRushYards (30);
    stats1->setPassYards (40);
    stats1->setFirstDowns (15);
    stats1->setPuntReturnYards (60);
    stats1->setKickReturnYards (70);
    stats1->setTotalYards (80);
    stats1->setThirdDownConv (9);
    stats1->setFourthDownConv (10);
    stats1->setTwoPointConv (11);
    stats1->setRedZoneTds (12);
    stats1->setRedZoneFgs (13);
    stats1->setPenaltyCount (14);
    stats1->setTimeOfPossession (15);
    stats1->setTwoPointConvAtt (16);
    stats1->setRedZoneAtt (17);
    stats1->setPenaltyYards (18);
    stats1->setThirdDownConvAtt (19);
    stats1->setFourthDownConvAtt (20);
    stats1->setPassInts (21);
    stats1->setFumblesLost (22);

    stats2 = req.getTeam2GameStats ();
    stats2->setTeamId (6);
    stats2->setScore (14);
    stats2->setTotalOffense (20);
    stats2->setRushYards (30);
    stats2->setPassYards (40);
    stats2->setFirstDowns (15);
    stats2->setPuntReturnYards (60);
    stats2->setKickReturnYards (70);
    stats2->setTotalYards (80);
    stats2->setThirdDownConv (9);
    stats2->setFourthDownConv (10);
    stats2->setTwoPointConv (11);
    stats2->setRedZoneTds (12);
    stats2->setRedZoneFgs (13);
    stats2->setPenaltyCount (14);
    stats2->setTimeOfPossession (15);
    stats2->setTwoPointConvAtt (16);
    stats2->setRedZoneAtt (17);
    stats2->setPenaltyYards (18);
    stats2->setThirdDownConvAtt (19);
    stats2->setFourthDownConvAtt (20);
    stats2->setPassInts (21);
    stats2->setFumblesLost (22);

    // this rpc fails because the franid/gameid pair isn't in the table (it wants to delete
    // it before inserting new data).  the only way to get this to work is to operate off
    // of my db-filled info, but that raises another point of - how do I modify franchises
    // if I don't know if they are real or if my db-filler python app added them?
    BLAZE_INFO (BlazeRpcLog::franchise, "[%d] Start of submitTeamGameStats test, fid=%d, bzid=%d", mId, franId, mBlazeId);
    TimeValue startTime = TimeValue::getTimeOfDay();
    rpcResult = mProxy->submitTeamGameStats (&req);
    TimeValue endTime = TimeValue::getTimeOfDay();
    mOwner->addMetric(FranchiseModule::METRIC_SUBMIT_TEAM_GAME_STATS, endTime - startTime);
    PROCESS_ERROR (rpcResult, "submitTeamGameStatsLocal", FranchiseModule::METRIC_SUBMIT_TEAM_GAME_STATS);
    gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs

    return rpcResult;
}

BlazeRpcError FranchiseInstance::submitTeamGameStats (Franchise::SubmitTeamGameStatsRequest& req)
{
    BlazeRpcError rpcResult = ERR_OK;

    StatsTables &stats = StatsTables::GetInstance();

    TeamStats *teams[] = { req.getTeam1GameStats(), req.getTeam2GameStats() };

    for (int i = 0; i < 2; i++)
    {
        TeamStats *teamStats = teams[i];
        const eastl::vector<int> &row = stats.GetTeamStatsByGame()[rand() % stats.GetTeamStatsByGame().size()];

        // teamids are already set outside the function.
        teamStats->setScore((uint16_t)row[1]);
        teamStats->setTotalOffense((int16_t)row[2]);
        teamStats->setRushYards((int16_t)row[3]);
        teamStats->setPassYards((int16_t)row[4]);
        teamStats->setFirstDowns((uint8_t)row[5]);
        teamStats->setPuntReturnYards((int16_t)row[6]);
        teamStats->setKickReturnYards((int16_t)row[7]);
        teamStats->setTotalYards((int16_t)row[8]);
        teamStats->setThirdDownConv((uint8_t)row[9]);
        teamStats->setFourthDownConv((uint8_t)row[10]);
        teamStats->setTwoPointConv((uint8_t)row[11]);
        teamStats->setRedZoneTds((uint8_t)row[12]);
        teamStats->setRedZoneFgs((uint8_t)row[13]);
        teamStats->setPenaltyCount((uint8_t)row[14]);
        teamStats->setTimeOfPossession((uint16_t)row[15]);
        teamStats->setTwoPointConvAtt((uint8_t)row[16]);
        teamStats->setRedZoneAtt((uint8_t)row[17]);
        teamStats->setPenaltyYards((uint16_t)row[18]);
        teamStats->setThirdDownConvAtt((uint8_t)row[19]);
        teamStats->setFourthDownConvAtt((uint8_t)row[20]);
        teamStats->setPassInts((uint8_t)row[21]);
        teamStats->setFumblesLost((uint8_t)row[22]);
    }

    BLAZE_INFO (BlazeRpcLog::franchise, "[%d] Start of submitTeamGameStats test, fid=%d, bzid=%d, weekid=%d, seasonid=%d, gameid=%d",
        mId, req.getFranchiseId(), mBlazeId, req.getWindowId(), req.getSeasonId(), req.getGameId());
    TimeValue startTime = TimeValue::getTimeOfDay();
    rpcResult = mProxy->submitTeamGameStats (&req);
    TimeValue endTime = TimeValue::getTimeOfDay();
    mOwner->addMetric(FranchiseModule::METRIC_SUBMIT_TEAM_GAME_STATS, endTime - startTime);
    PROCESS_ERROR (rpcResult, "submitTeamGameStats", FranchiseModule::METRIC_SUBMIT_TEAM_GAME_STATS);
    gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs

    return rpcResult;
}

BlazeRpcError FranchiseInstance::setDraftProfileLocal (FranchiseId franId)
{
    BlazeRpcError rpcResult = ERR_OK;
    Franchise::SetDraftProfileRequest  req;

    // We need to add in all the athletes we want in the draft.
    req.setFranchiseId (franId);
    DraftProfile* dpList = req.getDraftProfile ();
    DraftProfile::AthleteIdList* aidList = dpList->getDraftBoard ();
    aidList->push_back (57);
    aidList->push_back (125);
    aidList->push_back (171);
    aidList->push_back (213);
    aidList->push_back (218);

    BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] Start of setDraftProfile test, fid=%d", mId, franId);
    TimeValue startTime = TimeValue::getTimeOfDay();
    rpcResult = mProxy->setDraftProfile (&req);
    TimeValue endTime = TimeValue::getTimeOfDay();
    mOwner->addMetric(FranchiseModule::METRIC_SET_DRAFT_PROFILE, endTime - startTime);
    PROCESS_ERROR (rpcResult, "setDraftProfileLocal", FranchiseModule::METRIC_SET_DRAFT_PROFILE);
    gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs

    return rpcResult;
}

BlazeRpcError FranchiseInstance::findFranchisesAsyncLocal ()
{
    BlazeRpcError rpcResult = ERR_OK;
    Franchise::FindFranchisesRequest  req;

    req.setMaxResults (10);
    req.setName ("Fran00");
    req.setAbbrev ("Fr");
    req.setPreferredTeamId (1);
    req.setMaxHumans (32);
    req.setMaxDNF (50);
    req.setFantasyDraftEnabled (0);
    req.setTradeType (FRANCHISE_TRADES_ENABLED_ALL);
    req.setSalaryCapEnabled (1);
    req.setInjuriesEnabled (1);
    req.setQuarterLength (DEFAULT_QUARTER_LEN);
    req.setSkillLevel (DEFAULT_SKILL_LEVEL);

    BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] Start of findFranchisesAsync test", mId);
    TimeValue startTime = TimeValue::getTimeOfDay();
    rpcResult = mProxy->findFranchisesAsync (&req);
    TimeValue endTime = TimeValue::getTimeOfDay();
    mOwner->addMetric(FranchiseModule::METRIC_FIND_FRANCHISES_ASYNC, endTime - startTime);
    PROCESS_ERROR (rpcResult, "findFranchisesAsyncLocal", FranchiseModule::METRIC_FIND_FRANCHISES_ASYNC);
    gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs

    return rpcResult;
}

BlazeRpcError FranchiseInstance::postNewsLocal (FranchiseId franId)
{
    BlazeRpcError rpcResult = ERR_OK;
    Franchise::PostNewsRequest  req;

    req.setFranchiseId (franId);
    req.setNews ("This is a dummy news report from the stress tester.  Enjoy.");
    req.setMsgType (NEWS_MSG_TYPE_ALL);

    BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] Start of postNews test, fid=%d", mId, franId);
    TimeValue startTime = TimeValue::getTimeOfDay();
    rpcResult = mProxy->postNews (&req);
    TimeValue endTime = TimeValue::getTimeOfDay();
    mOwner->addMetric(FranchiseModule::METRIC_POST_NEWS, endTime - startTime);
    PROCESS_ERROR (rpcResult, "postNewsLocal", FranchiseModule::METRIC_POST_NEWS);
    gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs

    return rpcResult;
}

// Need to be a member of franchise and GM also for this to work.
BlazeRpcError FranchiseInstance::advanceFranchiseLocal (FranchiseId franId, FranchiseState nextState)
{
    BlazeRpcError rpcResult = ERR_OK;
    Franchise::AdvanceFranchiseRequest  req;

    franId = getFranchiseWithFantasyDraftSetting (0, true, mBlazeId);
    if (franId == 0)
    {
        franId = getFranchiseWithFantasyDraftSetting (0, true, 0);
        if (franId == 0)
        {
            franId = getGMFranchiseIdForUser ();
        }
    }

    req.setFranchiseId (franId);
    req.setNextState (nextState);

    BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] Start of advanceFranchise test, next state = %d, fid=%d", mId, nextState, franId);
    TimeValue startTime = TimeValue::getTimeOfDay();
    rpcResult = mProxy->AdvanceFranchise (&req);
    TimeValue endTime = TimeValue::getTimeOfDay();
    mOwner->addMetric(FranchiseModule::METRIC_ADVANCE_FRANCHISE, endTime - startTime);
    PROCESS_ERROR (rpcResult, "advanceFranchiseLocal", FranchiseModule::METRIC_ADVANCE_FRANCHISE);
    gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs
    BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] End of advanceFranchise test, next state = %d, fid=%d", mId, nextState, franId);

    return rpcResult;
}

// Need to be a member of franchise and GM also for this to work.
BlazeRpcError FranchiseInstance::advanceFranchise (Franchise::AdvanceFranchiseRequest& req)
{
    BlazeRpcError rpcResult = ERR_OK;

    BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] Start of advanceFranchise test, next state = %d, fid=%d", mId, req.getNextState(), req.getFranchiseId());
    TimeValue startTime = TimeValue::getTimeOfDay();
    rpcResult = mProxy->AdvanceFranchise (&req);
    TimeValue endTime = TimeValue::getTimeOfDay();
    mOwner->addMetric(FranchiseModule::METRIC_ADVANCE_FRANCHISE, endTime - startTime);
    PROCESS_ERROR (rpcResult, "advanceFranchise", FranchiseModule::METRIC_ADVANCE_FRANCHISE);
    gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs
    BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] End of advanceFranchise test, next state = %d, fid=%d", mId, req.getNextState(), req.getFranchiseId());

    return rpcResult;
}

BlazeRpcError FranchiseInstance::simSeasonLocal (FranchiseId franId)
{
    BlazeRpcError rpcResult = ERR_OK;
    Franchise::SimSeasonRequest  req;

    req.setFranchiseId (franId);
    req.setRequester (mBlazeId); // ok for blaze id?

    BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] Start of simSeason test, fid=%d", mId, franId);
    TimeValue startTime = TimeValue::getTimeOfDay();
    rpcResult = mProxy->SimSeason (&req);
    TimeValue endTime = TimeValue::getTimeOfDay();
    mOwner->addMetric(FranchiseModule::METRIC_SIM_SEASON, endTime - startTime);
    PROCESS_ERROR (rpcResult, "simSeasonLocal", FranchiseModule::METRIC_SIM_SEASON);
    gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs

    return rpcResult;
}

BlazeRpcError FranchiseInstance::athleteProgressLocal (FranchiseId franId)
{
    BlazeRpcError rpcResult = ERR_OK;
    Franchise::AthleteProgressRequest  req;

    req.setFranchiseId (franId);
    req.setRequester (mBlazeId); // ok for blaze id?

    BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] Start of athleteProgress test, fid=%d", mId, franId);
    TimeValue startTime = TimeValue::getTimeOfDay();
    rpcResult = mProxy->athleteProgress (&req);
    TimeValue endTime = TimeValue::getTimeOfDay();
    mOwner->addMetric(FranchiseModule::METRIC_ATHLETE_PROGRESS, endTime - startTime);
    PROCESS_ERROR (rpcResult, "athleteProgressLocal", FranchiseModule::METRIC_ATHLETE_PROGRESS);
    gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs

    return rpcResult;
}

// Can only sign a free agent.
BlazeRpcError FranchiseInstance::signAthleteLocal (FranchiseId franId)
{
    BlazeRpcError rpcResult = ERR_OK;
    Franchise::SignAthleteRequest  req;
    AthleteId aId = getOneAthleteOnTeamInFranchise (franId, 1009, false); // we pick an athlete from the free agent team to sign

    if (aId != 0 && aId != 32767)
    {
        req.setFranchiseId (franId);
        req.setTeamId (mTeamId);
        req.setAthleteId (aId);

        BLAZE_INFO (BlazeRpcLog::franchise, "[%d] Start of signAthlete test, fid=%d, aid=%d, tm=%d", mId, franId, aId, mTeamId);
        TimeValue startTime = TimeValue::getTimeOfDay();
        rpcResult = mProxy->signAthlete (&req);
        TimeValue endTime = TimeValue::getTimeOfDay();
        mOwner->addMetric(FranchiseModule::METRIC_SIGN_ATHLETE, endTime - startTime);
        PROCESS_ERROR (rpcResult, "signAthleteLocal", FranchiseModule::METRIC_SIGN_ATHLETE);
        gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs
    }
    else
    {
        BLAZE_INFO (BlazeRpcLog::franchise, "[%d] Skipping signAthlete: could not find a valid athleteid; fid=%d, tm=%d", mId, franId, mTeamId);
    }
    return rpcResult;
}

BlazeRpcError FranchiseInstance::releaseAthleteLocal (FranchiseId franId)
{
    BlazeRpcError rpcResult = ERR_OK;
    Franchise::ReleaseAthleteRequest  req;
    AthleteId aId = getOneAthleteOnTeamInFranchise (franId, mTeamId, false);

    if (aId != 0 && aId != 32767)
    {
        req.setFranchiseId (franId);
        req.setTeamId (mTeamId);
        req.setAthleteId (aId);

        BLAZE_INFO (BlazeRpcLog::franchise, "[%d] Start of releaseAthlete test, aid=%d, fid=%d, tid=%d", mId, aId, franId, mTeamId);
        TimeValue startTime = TimeValue::getTimeOfDay();
        rpcResult = mProxy->releaseAthlete (&req);
        TimeValue endTime = TimeValue::getTimeOfDay();
        mOwner->addMetric(FranchiseModule::METRIC_RELEASE_ATHLETE, endTime - startTime);
        PROCESS_ERROR (rpcResult, "releaseAthleteLocal", FranchiseModule::METRIC_RELEASE_ATHLETE);
        gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs
    }
    else
    {
        BLAZE_INFO (BlazeRpcLog::franchise, "[%d] Skipping releaseAthlete: could not find a valid athleteid; fid=%d, tm=%d", mId, franId, mTeamId);
    }
    return rpcResult;
}

BlazeRpcError FranchiseInstance::getCpuTradesLocal(FranchiseId franId)
{
    BlazeRpcError rpcResult = ERR_OK;
    Franchise::GetCpuTradesRequest  req;
    Franchise::GetTradesResponse    resp;

    req.setFranchiseId (franId);
    req.setRosterId (mRosterId); // ??

    BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] Start of getCpuTrades test, fid=%d", mId, franId);
    TimeValue startTime = TimeValue::getTimeOfDay();
    rpcResult = mProxy->getCpuTrades(&req, &resp);
    TimeValue endTime = TimeValue::getTimeOfDay();
    mOwner->addMetric(FranchiseModule::METRIC_GET_CPU_TRADES, endTime - startTime);
    PROCESS_ERROR (rpcResult, "getCpuTradesLocal", FranchiseModule::METRIC_GET_CPU_TRADES);
    gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs

    return rpcResult;
}

BlazeRpcError FranchiseInstance::getDisplayRosterByAthleteLocal(FranchiseId franId)
{
    BlazeRpcError rpcResult = ERR_OK;
    Franchise::GetDisplayRosterByAthleteRequest  req;
    Franchise::GetDisplayRosterResponse          resp;

    req.setFranchiseId (franId);
    req.setAthleteId (getOneAthleteOnTeamInFranchise (franId, mTeamId, false));
    req.setRosterId (mRosterId); // ??

    BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] Start of getDisplayRosterByAthlete test, fid=%d", mId, franId);
    TimeValue startTime = TimeValue::getTimeOfDay();
    rpcResult = mProxy->getDisplayRosterByAthlete(&req, &resp);
    TimeValue endTime = TimeValue::getTimeOfDay();
    mOwner->addMetric(FranchiseModule::METRIC_GET_DISPLAY_ROSTER_BY_ATHLETE, endTime - startTime);
    PROCESS_ERROR (rpcResult, "getDisplayRosterByAthleteLocal", FranchiseModule::METRIC_GET_DISPLAY_ROSTER_BY_ATHLETE);
    gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs

    return rpcResult;
}

BlazeRpcError FranchiseInstance::submitAntiCheatInfoLocal(FranchiseId franId)
{
    BlazeRpcError rpcResult = ERR_OK;
    Franchise::SubmitAntiCheatInfoRequest  req;

    req.setFranchiseId (franId);
    req.setGameId (15); // ??
    req.setState (FRANCHISE_STATE_REGULAR_SEASON); // ??
    req.setSeasonId (0); // ??

    BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] Start of submitAntiCheatInfo test, fid=%d", mId, franId);
    TimeValue startTime = TimeValue::getTimeOfDay();
    rpcResult = mProxy->submitAntiCheatInfo(&req);
    TimeValue endTime = TimeValue::getTimeOfDay();
    mOwner->addMetric(FranchiseModule::METRIC_SUBMIT_ANTI_CHEAT_INFO, endTime - startTime);
    PROCESS_ERROR (rpcResult, "submitAntiCheatInfoLocal", FranchiseModule::METRIC_SUBMIT_ANTI_CHEAT_INFO);
    gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs

    return rpcResult;
}


BlazeRpcError FranchiseInstance::getGameplayInformationLocal (FranchiseId franId)
{
    BlazeRpcError rpcResult = ERR_OK;
    Franchise::GetGameplayInformationRequest  req;
    Franchise::GetGameplayInformationResponse resp;

    req.setFranchiseId (franId);
    req.setTeam1Id (1);   // ??
    req.setTeam2Id (2);   // ??
    req.setRosterId (mRosterId); // ??

    BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] Start of getGameplayInformation test, fid=%d", mId, franId);
    TimeValue startTime = TimeValue::getTimeOfDay();
    rpcResult = mProxy->getGameplayInformation(&req, &resp);
    TimeValue endTime = TimeValue::getTimeOfDay();
    mOwner->addMetric(FranchiseModule::METRIC_GET_GAMEPLAY_INFORMATION, endTime - startTime);
    PROCESS_ERROR (rpcResult, "getGameplayInformationLocal", FranchiseModule::METRIC_GET_GAMEPLAY_INFORMATION);
    gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs

    return rpcResult;
}

BlazeRpcError FranchiseInstance::getGameplayInformation (Franchise::GetGameplayInformationRequest& req, Franchise::GetGameplayInformationResponse& resp)
{
    BlazeRpcError rpcResult = ERR_OK;

    BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] Start of getGameplayInformation test, fid=%d", mId, req.getFranchiseId());
    TimeValue startTime = TimeValue::getTimeOfDay();
    rpcResult = mProxy->getGameplayInformation(&req, &resp);
    TimeValue endTime = TimeValue::getTimeOfDay();
    mOwner->addMetric(FranchiseModule::METRIC_GET_GAMEPLAY_INFORMATION, endTime - startTime);
    PROCESS_ERROR (rpcResult, "getGameplayInformationLocal", FranchiseModule::METRIC_GET_GAMEPLAY_INFORMATION);
    gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs

    return rpcResult;
}

BlazeRpcError FranchiseInstance::setMemberStatusLocal (FranchiseId franId)
{
    BlazeRpcError rpcResult = ERR_OK;
    Franchise::SetMemberStatusRequest  req;

    req.setFranchiseId (franId);
    req.setStatus (MEMBER_READY_FOR_ADVANCE);

    BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] Start of setMemberStatus test, fid=%d", mId, franId);
    TimeValue startTime = TimeValue::getTimeOfDay();
    rpcResult = mProxy->setMemberStatus(&req);
    TimeValue endTime = TimeValue::getTimeOfDay();
    mOwner->addMetric(FranchiseModule::METRIC_SET_MEMBER_STATUS, endTime - startTime);
    PROCESS_ERROR (rpcResult, "setMemberStatusLocal", FranchiseModule::METRIC_SET_MEMBER_STATUS);
    gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs

    return rpcResult;
}

BlazeRpcError FranchiseInstance::getRetirementsLocal (FranchiseId franId)
{
    BlazeRpcError rpcResult = ERR_OK;
    Franchise::GetRetirementsRequest  req;
    Franchise::GetRetirementsResponse resp;

    req.setFranchiseId (franId);
    req.setRosterId (mRosterId);

    BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] Start of getRetirements test, fid=%d", mId, franId);
    TimeValue startTime = TimeValue::getTimeOfDay();
    rpcResult = mProxy->getRetirements(&req, &resp);
    TimeValue endTime = TimeValue::getTimeOfDay();
    mOwner->addMetric(FranchiseModule::METRIC_GET_RETIREMENTS, endTime - startTime);
    PROCESS_ERROR (rpcResult, "getRetirementsLocal", FranchiseModule::METRIC_GET_RETIREMENTS);
    gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs

    return rpcResult;
}

BlazeRpcError FranchiseInstance::getGameStatReportLocal (FranchiseId franId)
{
    BlazeRpcError rpcResult = ERR_OK;
    Franchise::GetGameStatReportRequest  req;
    Franchise::GetGameStatReportResponse resp;

    req.setFranchiseId (1); // this fid is already set up in the db w/ game stats
    req.setGameId (2);      // ditto for this game
    req.setSeasonId (0);    // ??

    BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] Start of getGameStatReport test, fid=%d", mId, franId);
    TimeValue startTime = TimeValue::getTimeOfDay();
    rpcResult = mProxy->getGameStatReport(&req, &resp);
    TimeValue endTime = TimeValue::getTimeOfDay();
    mOwner->addMetric(FranchiseModule::METRIC_GET_GAME_STAT_REPORT, endTime - startTime);
    PROCESS_ERROR (rpcResult, "getGameStatReportLocal", FranchiseModule::METRIC_GET_GAME_STAT_REPORT);
    gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs

    return rpcResult;
}

BlazeRpcError FranchiseInstance::validateMatchLocal(FranchiseId franId)
{
    BlazeRpcError rpcResult = ERR_OK;
    Franchise::ValidateMatchInfoRequest req;

    BlazeId bzid1 = findAnyMember (franId, 1);
    BlazeId bzid2 = findAnyMember (franId, 2);
    TeamId tid1 = getMyTeamId (franId, bzid1);
    TeamId tid2 = getMyTeamId (franId, bzid2);

    req.setFranchiseId (franId);
    req.setHomeBlazeId (bzid1);
    req.setHomeTeamId (tid1);
    req.setAwayBlazeId (bzid2);
    req.setAwayTeamId (tid2);
    req.setWindowId (0);
    req.setSeasonId (0);

    BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] Start of validateMatch test, fid=%d", mId, franId);
    TimeValue startTime = TimeValue::getTimeOfDay();
    rpcResult = mProxy->validateMatch (&req);
    TimeValue endTime = TimeValue::getTimeOfDay();
    mOwner->addMetric(FranchiseModule::METRIC_VALIDATE_MATCH, endTime - startTime);
    PROCESS_ERROR (rpcResult, "validateMatchLocal", FranchiseModule::METRIC_VALIDATE_MATCH);
    gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs

    return rpcResult;
}

BlazeRpcError FranchiseInstance::getServerSettingsLocal (FranchiseId franId)
{
    BlazeRpcError rpcResult = ERR_OK;
    Franchise::GetServerSettingsResponse resp;

    BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] Start of getServerSettings test, fid=%d", mId, franId);
    TimeValue startTime = TimeValue::getTimeOfDay();
    rpcResult = mProxy->getServerSettings (&resp);
    TimeValue endTime = TimeValue::getTimeOfDay();
    mOwner->addMetric(FranchiseModule::METRIC_GET_SERVER_SETTINGS, endTime - startTime);
    PROCESS_ERROR (rpcResult, "getServerSettingsLocal", FranchiseModule::METRIC_GET_SERVER_SETTINGS);
    gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs

    return rpcResult;
}

BlazeRpcError FranchiseInstance::setJerseyNumberLocal (FranchiseId franId)
{
    BlazeRpcError rpcResult = ERR_OK;
    Franchise::SetJerseyNumberRequest req;

    req.setAthleteId (getOneAthleteOnTeamInFranchise (franId, mTeamId));
    req.setFranchiseId (franId);
    req.setJerseyNum ((uint8_t)Blaze::Random::getRandomNumber(150));

    BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] Start of setJerseyNumber test, fid=%d", mId, franId);
    TimeValue startTime = TimeValue::getTimeOfDay();
    rpcResult = mProxy->setJerseyNumber (&req);
    TimeValue endTime = TimeValue::getTimeOfDay();
    mOwner->addMetric(FranchiseModule::METRIC_SET_JERSEY_NUMBER, endTime - startTime);
    PROCESS_ERROR (rpcResult, "setJerseyNumberLocal", FranchiseModule::METRIC_SET_JERSEY_NUMBER);
    gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs

    return rpcResult;
}

BlazeRpcError FranchiseInstance::advanceWeekTest ()
{
    BlazeRpcError rpcResult = ERR_OK;
    FranchiseId fid = getGMFranchiseIdForUser (true);

    FranchiseState curState = FRANCHISE_STATE_NONE;
    if (mCurrentFranData != NULL)
    {
        curState = mCurrentFranData->state;
    }
    FranchiseState nextState = getNextFranchiseState (fid); // call getFranchise and grab state
    
    if (fid && (curState == FRANCHISE_STATE_REGULAR_SEASON || curState == FRANCHISE_STATE_PLAYOFFS))
    {
        Franchise::GetRegularScheduleRequest  grsReq;
        Franchise::GetRegularScheduleResponse grsResp;

        // set season and window (week) ids, and fid
        grsReq.setFranchiseId (fid);
        grsReq.setSeasonId (mCurrentFranData->seasonId);
        grsReq.setWindowId (mCurrentFranData->windowId); // -1 means get whole season schedule; we want one week only
        grsReq.setTeamId (0); // set to 0 means ignore this param

        rpcResult = getRegularSchedule (grsReq, grsResp); // need to grab result data and use for next call
        if (rpcResult != ERR_OK)
        {
            return rpcResult;
        }

        GetRegularScheduleResponse::RegularGameInfoList* gameList = grsResp.getGameList ();
        if (gameList != NULL && !gameList->empty())
        {
            GetRegularScheduleResponse::RegularGameInfoList::iterator iter = gameList->begin();
            ValidateRostersRequest vrReq;
            vrReq.setFranchiseId (fid);

            for (; iter != gameList->end(); iter++)
            {
                uint8_t state = (*iter)->getState ();
                if (state == 1) // unplayed
                {
                    ValidateRostersRequest::TeamIdList* teamList = vrReq.getTeamIds ();
                    TeamId home = (*iter)->getHomeId ();
                    TeamId away = (*iter)->getAwayId ();
                    teamList->push_back (home);
                    teamList->push_back (away);
                }
            }
            rpcResult = validateRosters (vrReq); // need to fill in team list before calling rpc only for teams for games not played
            if (rpcResult != ERR_OK)
            {
                return rpcResult;
            }
        }

        // for each game not played from the schedule response, up to 16 max
        if (gameList != NULL && !gameList->empty())
        {
            GetRegularScheduleResponse::RegularGameInfoList::iterator iter = gameList->begin();

            for (; iter != gameList->end(); iter++)
            {
                uint8_t state = (*iter)->getState ();
                if (state == 1) // unplayed
                {
                    Franchise::GetGameplayInformationRequest ggiReq;
                    Franchise::GetGameplayInformationResponse ggiResp;

                    ggiReq.setFranchiseId (fid);
                    ggiReq.setTeam1Id ((*iter)->getHomeId ());
                    ggiReq.setTeam2Id ((*iter)->getAwayId ());
                    ggiReq.setRosterId (mRosterId);
                    rpcResult = getGameplayInformation (ggiReq, ggiResp);
                    if (rpcResult != ERR_OK)
                    {
                        return rpcResult;
                    }
                    
                    StatsTables* statsTable = NULL;
                    if (mOwner->getStatsEnabled())
                    {

                        Franchise::SubmitFranchiseGameStatsRequest sfsReq;
                        sfsReq.setFranchiseId (fid);
                        sfsReq.setGameId ((*iter)->getGameId ());
                        sfsReq.setSeasonId (mCurrentFranData->seasonId);
                        sfsReq.setVersusType (VERSUS_TYPE_CPU_VS_CPU);
                        sfsReq.setWindowId (mCurrentFranData->windowId);
                        sfsReq.setState(mCurrentFranData->state);

                        statsTable = mOwner->getStatsTable();
                        if (statsTable != NULL && !statsTable->GetTeamStatsByGame().empty())
                        {
                            Franchise::SubmitFranchiseGameStatsRequest sfsFranchiseTeamStatsReq;
                            sfsReq.copyInto(sfsFranchiseTeamStatsReq);
                            sfsFranchiseTeamStatsReq.setCategory("FranchiseTeamStats");
                            rpcResult = submitFranchiseGameStats (sfsFranchiseTeamStatsReq, ggiResp);
                            if (rpcResult != ERR_OK)
                            {
                                return rpcResult;
                            }
                        }

                        if (statsTable != NULL && !statsTable->GetInjury().empty())
                        {
                            Franchise::SubmitFranchiseGameStatsRequest sfsInjuryPlayerStatsReq;
                            sfsReq.copyInto(sfsInjuryPlayerStatsReq);
                            sfsInjuryPlayerStatsReq.setCategory("InjuryPlayerStats");
                            rpcResult = submitFranchiseGameStats (sfsInjuryPlayerStatsReq, ggiResp);
                            if (rpcResult != ERR_OK)
                            {
                                return rpcResult;
                            }
                        }

                        if (statsTable != NULL && !statsTable->GetOffensivePlayer().empty())
                        {
                            Franchise::SubmitFranchiseGameStatsRequest offensivePlayerStatReq;
                            sfsReq.copyInto(offensivePlayerStatReq);
                            offensivePlayerStatReq.setCategory("OffensivePlayerStats");
                            rpcResult = submitFranchiseGameStats (offensivePlayerStatReq, ggiResp);
                            if (rpcResult != ERR_OK)
                            {
                                return rpcResult;
                            }
                        }

                        if (statsTable != NULL && !statsTable->GetOffensiveLinePlayer().empty())
                        {
                            Franchise::SubmitFranchiseGameStatsRequest offensiveLinePlayerStatReq;
                            sfsReq.copyInto(offensiveLinePlayerStatReq);
                            offensiveLinePlayerStatReq.setCategory("OffensiveLinePlayerStats");
                            rpcResult = submitFranchiseGameStats (offensiveLinePlayerStatReq, ggiResp);
                            if (rpcResult != ERR_OK)
                            {
                                return rpcResult;
                            }
                        }

                        if (statsTable != NULL && !statsTable->GetDefensivePlayer().empty())
                        {
                            Franchise::SubmitFranchiseGameStatsRequest defensivePlayerStatsReq;
                            sfsReq.copyInto(defensivePlayerStatsReq);
                            defensivePlayerStatsReq.setCategory("DefensivePlayerStats");
                            rpcResult = submitFranchiseGameStats (defensivePlayerStatsReq, ggiResp);
                            if (rpcResult != ERR_OK)
                            {
                                return rpcResult;
                            }
                        }

                        if (statsTable != NULL && !statsTable->GetKickPlayer().empty())
                        {
                            Franchise::SubmitFranchiseGameStatsRequest kickPlayerStatsReq;
                            sfsReq.copyInto(kickPlayerStatsReq);
                            kickPlayerStatsReq.setCategory("KickPlayerStats");
                            rpcResult = submitFranchiseGameStats (kickPlayerStatsReq, ggiResp);
                            if (rpcResult != ERR_OK)
                            {
                                return rpcResult;
                            }
                        }

                        if (statsTable != NULL && !statsTable->GetKickReturnPlayer().empty())
                        {
                            Franchise::SubmitFranchiseGameStatsRequest kickReturnPlayerStatsReq;
                            sfsReq.copyInto(kickReturnPlayerStatsReq);
                            kickReturnPlayerStatsReq.setCategory("KickReturnPlayerStats");
                            rpcResult = submitFranchiseGameStats (kickReturnPlayerStatsReq, ggiResp);
                            if (rpcResult != ERR_OK)
                            {
                                return rpcResult;
                            }
                        }

                        if (statsTable != NULL && !statsTable->GetFieldPlayer().empty())
                        {
                            Franchise::SubmitFranchiseGameStatsRequest fieldPlayerStatReq;
                            sfsReq.copyInto(fieldPlayerStatReq);
                            fieldPlayerStatReq.setCategory("FieldPlayerStats");
                            rpcResult = submitFranchiseGameStats (fieldPlayerStatReq, ggiResp);
                            if (rpcResult != ERR_OK)
                            {
                                return rpcResult;
                            }
                        }

                        Franchise::SubmitScoringSummaryRequest sssReq;
                        sssReq.setFranchiseId (fid);
                        sssReq.setGameId ((*iter)->getGameId ());
                        sssReq.setIsSimGame (1);
                        sssReq.setRosterId (mRosterId);
                        sssReq.setSeasonId (mCurrentFranData->seasonId);
                        sssReq.setVersusType (VERSUS_TYPE_CPU_VS_CPU);
                        sssReq.setWindowId (mCurrentFranData->windowId);
                        rpcResult = submitScoringSummary (sssReq, (*iter)->getHomeId(), (*iter)->getAwayId());
                        if (rpcResult != ERR_OK)
                        {
                            return rpcResult;
                        }

                        Franchise::SubmitAthleteGameStatsRequest sagsReq;
                        sagsReq.setFranchiseId (fid);
                        sagsReq.setGameId ((*iter)->getGameId ());
                        sagsReq.setSeasonId (mCurrentFranData->seasonId);
                        sagsReq.setVersusType (VERSUS_TYPE_CPU_VS_CPU);
                        sagsReq.setWindowId (mCurrentFranData->windowId);
                        Franchise::AthleteStatsByTeam* athleteTeamStat1 = sagsReq.getAthleteStatsT1();
                        athleteTeamStat1->setTeamId ((*iter)->getHomeId ());
                        Franchise::AthleteStatsByTeam* athleteTeamStat2 = sagsReq.getAthleteStatsT2();
                        athleteTeamStat2->setTeamId ((*iter)->getAwayId ());

                        rpcResult = submitAthleteGameStats (sagsReq);
                        if (rpcResult != ERR_OK)
                        {
                            return rpcResult;
                        }

                        Franchise::SubmitTeamGameStatsRequest stgsReq;
                        stgsReq.setFranchiseId (fid);
                        stgsReq.setGameId ((*iter)->getGameId ());
                        stgsReq.setSeasonId (mCurrentFranData->seasonId);
                        stgsReq.setVersusType (VERSUS_TYPE_CPU_VS_CPU);
                        stgsReq.setWindowId (mCurrentFranData->windowId);
                        TeamStats* teamStat1 = stgsReq.getTeam1GameStats();
                        teamStat1->setTeamId ((*iter)->getHomeId ());
                        TeamStats* teamStat2 = stgsReq.getTeam2GameStats();
                        teamStat2->setTeamId ((*iter)->getAwayId ());
                        rpcResult = submitTeamGameStats (stgsReq);
                        if (rpcResult != ERR_OK)
                        {
                            return rpcResult;
                        }
                    }
                }
            }
        }
        Franchise::AdvanceFranchiseRequest afReq;
        afReq.setFranchiseId (fid);
        afReq.setNextState (nextState);
        rpcResult = advanceFranchise (afReq);
        if (rpcResult != ERR_OK)
        {
            return rpcResult;
        }
    }
    else if (fid != 0 && curState == FRANCHISE_STATE_DRAFT_SETUP || curState == FRANCHISE_STATE_DRAFT)
    {
        // start the draft process to advance the franchise to regular season.
        Franchise::JoinDraftRequest jdReq;
        Franchise::JoinDraftResponse jdResp;
        jdReq.setFranchiseId(fid);
        rpcResult = joinDraft(jdReq, jdResp);

        if (rpcResult == ERR_OK)
        {
            Franchise::BeginDraftRequest bdReq;
            bdReq.setFranchiseId(fid);
            bdReq.setTime(2);
            rpcResult = beginDraft(bdReq);

            if (rpcResult == ERR_OK)
            {
                //now poll on the draft process, and come out only when draft is complete
                rpcResult = pollDraftLocal(fid);
                if (rpcResult != ERR_OK)
                {
                    return rpcResult;
                }
            }
        }
    }
    else
    {
        if (mCurrentFranData != NULL)
        {
            BLAZE_INFO (BlazeRpcLog::franchise, "[%d] Cannot advance week as franchise=%d is in state=%d, weekid=%d, seasonid=%d",
                mId, fid, mCurrentFranData->state, mCurrentFranData->windowId, mCurrentFranData->seasonId);
        }
        else
        {
            BLAZE_INFO (BlazeRpcLog::franchise, "[%d] Cannot advance week as franchise=%d",mId, fid);
        }
    }

    if (fid != 0)
    {
        Franchise::GetFranchiseRequest gfReq;
        Franchise::Franchise gfResp;
        gfReq.setFranchiseId (fid);
        rpcResult = getFranchise (gfReq, gfResp);
        if (rpcResult == ERR_OK)
        {
            mCurrentFranData->seasonId = gfResp.getCurrSeason();
            mCurrentFranData->windowId = gfResp.getCurrWindow();
            mCurrentFranData->state = gfResp.getState();
        }
    }

    return rpcResult;
}


///////////////////////////////////////////////////////////////////////////////

FranchiseState FranchiseInstance::getNextFranchiseState (FranchiseId franId)
{
    FranchiseState nextState = FRANCHISE_STATE_NONE;

    nextState = FRANCHISE_STATE_NONE;

    return nextState;
}

// Find the current GM for a franchise (it can change).
BlazeId FranchiseInstance::getGMForFranchise (FranchiseId franId)
{
    FranchiseIdDataVectorType::iterator iterFrans = mFranchiseIdData.begin();

    // Find the franchise id corresponding to the franchise id passed in.
    for (; iterFrans != mFranchiseIdData.end(); iterFrans++)
    {
        if ((*iterFrans)->fid == franId)
        {
            // Iterate thru all members to find the GM.
            FranchiseIdData* franData = *iterFrans;
            MemberDataVectorType::iterator iterMembers = franData->memberData.begin();
            for (; iterMembers != franData->memberData.end(); iterMembers++)
            {
                if ((*iterMembers)->isGM)
                {
                    return (*iterMembers)->bzid;
                }
            }
        }
    }
    MYASSERT (!"Could not find GM for franchise!");
    return 0;
}

// Find the first member of a franchise that is not the GM.
BlazeId FranchiseInstance::findNonGMMember (FranchiseId franId)
{
    FranchiseIdDataVectorType::iterator iterFrans = mFranchiseIdData.begin();

    // Find the franchise id corresponding to the franchise id passed in.
    for (; iterFrans != mFranchiseIdData.end(); iterFrans++)
    {
        if ((*iterFrans)->fid == franId)
        {
            // Iterate thru all members to find someone NOT the GM.
            FranchiseIdData* franData = *iterFrans;
            MemberDataVectorType::iterator iterMembers = franData->memberData.begin();
            for (; iterMembers != franData->memberData.end(); iterMembers++)
            {
                if (!(*iterMembers)->isGM)
                {
                    return (*iterMembers)->bzid;
                }
            }
            break;
        }
    }
    return 0; // this is ok; we may not have any non-gm members sometimes
}


// Find a member of a franchise.
BlazeId FranchiseInstance::findAnyMember (FranchiseId franId, int32_t which)
{
    FranchiseIdDataVectorType::iterator iterFrans = mFranchiseIdData.begin();

    // Find the franchise id corresponding to the franchise id passed in.
    for (; iterFrans != mFranchiseIdData.end(); iterFrans++)
    {
        if ((*iterFrans)->fid == franId)
        {
            // Iterate thru all members to find someone.
            FranchiseIdData* franData = *iterFrans;
            MemberDataVectorType::iterator iterMembers = franData->memberData.begin();
            for (int32_t count=1; iterMembers != franData->memberData.end(); iterMembers++, count++)
            {
                if (count == which || which == 0)
                {
                    return (*iterMembers)->bzid;
                }
            }
            break;
        }
    }
    return 0; // is this ok?
}

// Find the first member of a franchise that is banned.
BlazeId FranchiseInstance::findBannedMember (FranchiseId franId)
{
    FranchiseIdDataVectorType::iterator iterFrans = mFranchiseIdData.begin();

    // Find the franchise id corresponding to the franchise id passed in.
    for (; iterFrans != mFranchiseIdData.end(); iterFrans++)
    {
        if ((*iterFrans)->fid == franId)
        {
            // Iterate thru all members to find someone who is banned.
            FranchiseIdData* franData = *iterFrans;
            MemberDataVectorType::iterator iterMembers = franData->memberData.begin();
            for (; iterMembers != franData->memberData.end(); iterMembers++)
            {
                if ((*iterMembers)->isBanned)
                {
                    return (*iterMembers)->bzid;
                }
            }
        }
    }
    return 0; // this is ok; we may not have any non-gm members sometimes
}

// Change the 'isBanned' flag of a member in a franchise.
void FranchiseInstance::changeBannedFlag (FranchiseId franId, BlazeId bzId, bool newState)
{
    FranchiseIdDataVectorType::iterator iterFrans = mFranchiseIdData.begin();

    // Find the franchise id corresponding to the franchise id passed in.
    for (; iterFrans != mFranchiseIdData.end(); iterFrans++)
    {
        if ((*iterFrans)->fid == franId)
        {
            // Iterate thru all members to find the passed member.
            FranchiseIdData* franData = *iterFrans;
            MemberDataVectorType::iterator iterMembers = franData->memberData.begin();
            for (; iterMembers != franData->memberData.end(); iterMembers++)
            {
                if ((*iterMembers)->bzid == bzId)
                {
                    (*iterMembers)->isBanned = newState;
                    break;
                }
            }
            break;
        }
    }
}

// Change the 'isGM' flag of a member in a franchise.
void FranchiseInstance::changeGMFlag (FranchiseId franId, BlazeId bzId, bool newState)
{
    FranchiseIdDataVectorType::iterator iterFrans = mFranchiseIdData.begin();

    // Find the franchise id corresponding to the franchise id passed in.
    for (; iterFrans != mFranchiseIdData.end(); iterFrans++)
    {
        if ((*iterFrans)->fid == franId)
        {
            // Iterate thru all members to find the passed member.
            FranchiseIdData* franData = *iterFrans;
            MemberDataVectorType::iterator iterMembers = franData->memberData.begin();
            for (; iterMembers != franData->memberData.end(); iterMembers++)
            {
                if ((*iterMembers)->bzid == bzId)
                {
                    (*iterMembers)->isGM = newState;
                    break;
                }
            }
            break;
        }
    }
}

AthleteId FranchiseInstance::getAthleteIdFromFranTeamId (FranchiseId franId, TeamId teamId)
{
    // Iterate thru the map of all athlete ids to athlete data and find a match for the passed params.
    athleteIdAthleteDataMapType::iterator iter = mAthleteDataMap.begin();
    for (; iter != mAthleteDataMap.end(); iter++)
    {
        AthleteData* aData = iter->second;
        if (aData->fid == franId && aData->tid == teamId)
        {
            return iter->first; // found it, return now
        }
    }
    //MYASSERT (!"Could not find athlete");
    BLAZE_INFO (BlazeRpcLog::franchise, "[%d] Could not find an athlete on team %d for franchise %d", mId, teamId, franId);
    return ATHLETE_ID_INVALID;
}

// Find and return the franchise id corresponding to the current user's blaze ID.
// Return 0 if none found.
FranchiseId FranchiseInstance::getGMFranchiseIdForUser (bool needFantasyDraftOff)
{
    FranchiseIdDataVectorType::iterator iterFrans = mFranchiseIdData.begin();
    eastl::vector<FranchiseIdData*> franchiseVector;

    // Loop thru all franchises until we find a GM member = mBlazeId.
    for (; iterFrans != mFranchiseIdData.end(); iterFrans++)
    {
        // Sometimes we want fantasy draft OFF, so if it's on, continue to the next franchise.
        if (needFantasyDraftOff && (*iterFrans)->fantasyDraftIsOn)
        {
                continue;
        }

        // Iterate thru all members until we find the passed blazeId.
        FranchiseIdData* franData = *iterFrans;
        MemberDataVectorType::iterator iterMembers = franData->memberData.begin();
        for (; iterMembers != franData->memberData.end(); iterMembers++)
        {
            if ((*iterMembers)->bzid == mBlazeId && (*iterMembers)->isGM)
            {
                franchiseVector.push_back (franData);
            }
        }
    }

    eastl::vector<FranchiseIdData*>::size_type size = franchiseVector.size();

    if (size > 1)
    {
        int32_t indexIntoVector = Blaze::Random::getRandomNumber((int32_t)size-1);   // get random index into the vector
        if (indexIntoVector >= 0)
        {
            mCurrentFranData = franchiseVector[indexIntoVector];
            return mCurrentFranData->fid;
            }
        }
    else if (size == 1)
    {
        mCurrentFranData = franchiseVector[0];
        return mCurrentFranData->fid;
    }

    return 0;
}

// Find and return the franchise id corresponding to the current user's blaze ID.
// Return 0 if none found.
FranchiseId FranchiseInstance::getAnyFranchiseIdForUser (FranchiseId defaultFranId)
{
    FranchiseIdDataVectorType::iterator iterFrans = mFranchiseIdData.begin();

    // Loop thru all franchises until we find a member = mBlazeId.
    for (; iterFrans != mFranchiseIdData.end(); iterFrans++)
    {
        // Iterate thru all members until we find the passed blazeId.
        FranchiseIdData* franData = *iterFrans;
        MemberDataVectorType::iterator iterMembers = franData->memberData.begin();
        for (; iterMembers != franData->memberData.end(); iterMembers++)
        {
            if ((*iterMembers)->bzid == mBlazeId)
            {
                return franData->fid;
            }
        }
    }
    //MYASSERT (!"Could not find franchise id for blaze id"); // blaze id has no franchises?  all deleted?
    return defaultFranId;
}

const char8_t *FranchiseInstance::getName() const
{
    return ACTION_STRINGS[mOwner->getAction()];
}

// Get the user's team id in a specified franchise.
TeamId FranchiseInstance::getMyTeamId (FranchiseId franId, BlazeId blazeId)
{
    FranchiseIdDataVectorType::iterator iterFrans = mFranchiseIdData.begin();

    // Loop thru all franchises until we find the right one.
    for (; iterFrans != mFranchiseIdData.end(); iterFrans++)
    {
        if ((*iterFrans)->fid == franId)
        {
            // Iterate thru all members until we find the passed blazeId.
            FranchiseIdData* franData = *iterFrans;
            MemberDataVectorType::iterator iterMembers = franData->memberData.begin();
            for (; iterMembers != franData->memberData.end(); iterMembers++)
            {
                if ((*iterMembers)->bzid == blazeId)
                {
                    return (*iterMembers)->tid;
                }
            }
            break;
        }
    }
    //MYASSERT (!"Could not find a team id for given blaze and franchise ids"); // blaze id has no franchises? all deleted? vector not filled up yet?
    return 0;
}

// Should we find the blaze id of another user in the same franchise?  Or just anyone in any franchise?
BlazeId FranchiseInstance::findBlazeIdOfSomeoneElse (FranchiseId franId)
{
    FranchiseIdDataVectorType::iterator iterFrans = mFranchiseIdData.begin();

    // Loop thru all franchises until we find the right one.
    for (; iterFrans != mFranchiseIdData.end(); iterFrans++)
    {
        if ((*iterFrans)->fid == franId)
        {
            // Iterate thru all members until we find the passed blazeId.
            FranchiseIdData* franData = *iterFrans;
            MemberDataVectorType::iterator iterMembers = franData->memberData.begin();
            for (; iterMembers != franData->memberData.end(); iterMembers++)
            {
                if ((*iterMembers)->bzid != mBlazeId)
                {
                    return (*iterMembers)->bzid;
                }
            }
            break;
        }
    }
    MYASSERT (!"Could not find another's blaze id"); // vector not filled up yet?
    return 0;
}

// 
FranchiseIdData* FranchiseInstance::findFidDataFromFranId (FranchiseId franId)
{
    FranchiseIdDataVectorType::iterator iter = mFranchiseIdData.begin();

    for (; iter != mFranchiseIdData.end(); iter++)
    {
        if ((*iter)->fid == franId)
        {
            return *iter;
        }
    }
    MYASSERT (!"Could not find element corresponding to fran id in vector"); // vector not filled up yet?
    return NULL;
}

void FranchiseInstance::findAllFranchiseIds ()
{
    BlazeRpcError rpcResult = ERR_OK;
    Franchise::FindFranchisesRequest  req;
    Franchise::FindFranchisesResponse resp;
    int32_t count = 0;

    // First get all franchises that *don't* belong to the current blaze id.
    req.setMaxResults (1000);
    req.setSkillLevel (OF_FRANCHISE_OPTION_ANY);
    req.setQuarterLength (OF_FRANCHISE_OPTION_ANY);
    req.setInjuriesEnabled (OF_FRANCHISE_OPTION_ANY);
    req.setSalaryCapEnabled (OF_FRANCHISE_OPTION_ANY);
    req.setTradeType (OF_FRANCHISE_OPTION_ANY);
    req.setFantasyDraftEnabled (OF_FRANCHISE_OPTION_ANY);
    req.setMaxHumans(OF_FRANCHISE_OPTION_ANY);
    req.setMaxDNF(OF_FRANCHISE_OPTION_ANY);

    BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] Start of findAllFranchiseIds for NOT bzid=%d", mId, mBlazeId);
    TimeValue startTime = TimeValue::getTimeOfDay();
    rpcResult = mProxy->findFranchises(&req, &resp);
    TimeValue endTime = TimeValue::getTimeOfDay();
    mOwner->addMetric(FranchiseModule::METRIC_FIND_FRANCHISES, endTime - startTime);

    if (rpcResult == ERR_OK)
    {
        // Iterate thru all franchises that aren't ours and add them to our vector of franchise data.
        FindFranchisesResponse::FranchisesList::const_iterator franchisesItr    = resp.getFranchises()->begin();
        FindFranchisesResponse::FranchisesList::const_iterator franchisesItrEnd = resp.getFranchises()->end();

        // Loop thru all franchises found.
        for (; franchisesItr != franchisesItrEnd; franchisesItr++)
        {
            FranchiseIdData* fidData  = new FranchiseIdData;
            fidData->fid              = (*franchisesItr)->getFranchiseId();
            fidData->gmId             = (*franchisesItr)->getGM()->getBlazeId();
            fidData->weCreatedIt      = false;   // only set to true in createFranchiseLocal
            fidData->weJoinedIt       = false;   // only set to true in joinFranchiseLocal
            fidData->fantasyDraftIsOn = (*franchisesItr)->getFantasyDraftEnabled() == 0 ? false : true;
            fidData->state            = (*franchisesItr)->getState ();
            fidData->seasonId         = (*franchisesItr)->getCurrSeason ();
            fidData->windowId         = (*franchisesItr)->getCurrWindow ();
            mRosterId                 = (*franchisesItr)->getRosterId(); // keep the last one we find
            mFranchiseIdData.push_back (fidData);
            count++;

            // Note - we can't ask for member data here because it is illegal to ask for member data in other franchises.
        }
    }
    gSelector->sleep (mRpcDelay * 1000); // arg is in microsecs; give findFranchises time to run
    BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] After 1st franchise loop, findAllFranchiseIds, found %d frans; rpcResult=0x%x, RosterID=%d",
        mId, count, rpcResult, mRosterId);

    // Next, get all franchises that belong to the current blaze id.
    // This also calls getMembersLocal to fill in member data for each franchise.
    getFranchisesByUserLocal ();
}

FranchiseId FranchiseInstance::getFranchiseWeCreated ()
{
    FranchiseIdDataVectorType::iterator iter = mFranchiseIdData.begin();
    for (; iter != mFranchiseIdData.end(); iter++)
    {
        if ((*iter)->weCreatedIt == true)
        {
            return (*iter)->fid;
        }
    }
    return 0;
}

// Make sure we don't return 0; the random number generator will give us a 0 every now and
// then, and if so, just ask again.
FranchiseId FranchiseInstance::getRandomFranchiseId ()
{
    eastl::vector<FranchiseId>::size_type size = mFranchiseIdData.size();
    if (size > 0)
    {
        int32_t indexIntoVector = 0;
        int32_t count = 0;
        while (indexIntoVector == 0 && count <= 5)
        {
            indexIntoVector = Blaze::Random::getRandomNumber((int32_t)size);   // get random index into the vector
            count++;
        }
        return indexIntoVector > 0 ? mFranchiseIdData[indexIntoVector]->fid : 0; // sub 1 to get real 0-based index
    }
    return 0;
}

// Simple function to get a team id that this user doesn't belong to.
// Just subtract one from the passed id, unless at the low end already.
TeamId FranchiseInstance::getDifferentTeamId (TeamId tId)
{
    if (tId == 1) return 32;
    else return (tId - 1);
}

// Given mBlazeId as the current user, find a franchise in the vector that doesn't contain mBlazeId.
// Find one at random, not just the 1st one in the list.
FranchiseId FranchiseInstance::getFranchiseWeAreNotAMemberOf ()
{
    FranchiseIdDataVectorType::iterator iterFrans;
    eastl::vector<FranchiseId>::size_type size = mFranchiseIdData.size();
    bool failed = false;
    bool done = false;
    int  numAttempts = 0;
    FranchiseId fid = 0;

    if (size > 0)
    {
        while (!done && numAttempts++ < 50)
        {
            // Pick a franchise id randomly from the number available.
            fid = getRandomFranchiseId ();

            // Loop thru all franchises until we find the franchise we just picked.
            for (iterFrans = mFranchiseIdData.begin(); iterFrans != mFranchiseIdData.end() && !failed; iterFrans++)
            {
                if (fid == (*iterFrans)->fid) // found it
                {
                    if ((*iterFrans)->gmId == mBlazeId)
                    {
                        failed = true;
                        break;  // we created this franchise, so we can't join it; try another time
                    }
        FranchiseIdData* franData = *iterFrans;
        MemberDataVectorType::iterator iterMembers = franData->memberData.begin();
        MemberDataVectorType::iterator iterMembersEnd = franData->memberData.end();
        for (; iterMembers != iterMembersEnd; iterMembers++)
        {
                        if ((*iterMembers)->bzid == mBlazeId) 
            {
                failed = true;
                            break; // this franchise is out; we're already a member
                        }
                    }
                break;
            }
        }
        if (!failed)
        {
                done = true;
        }
    }
    }

    return fid;
}

void FranchiseInstance::findMembersInOtherFranchises (FranchiseId franId, eastl::vector<MemberData*>& membersVec, bool fransWeCreatedOnly)
{
    FranchiseIdDataVectorType::iterator iterFrans = mFranchiseIdData.begin();

    // Loop thru all franchises until we find one we're not a member of.
    for (; iterFrans != mFranchiseIdData.end(); iterFrans++)
    {
        // If not this franchise, find all members and store them away in the passed vector.
        FranchiseIdData* franData = *iterFrans;
        if (franData != NULL && franData->fid != franId)
        {
            if (!fransWeCreatedOnly || fransWeCreatedOnly && franData->weCreatedIt)
            {
                MemberDataVectorType::iterator iterMembers = franData->memberData.begin();
                MemberDataVectorType::iterator iterMembersEnd = franData->memberData.end();
                for (; iterMembers != iterMembersEnd; iterMembers++)
                {
                   membersVec.push_back (*iterMembers);
                }
            }
        }
    }
}

// Delete member data for just the one franchise.
void FranchiseInstance::clearMemberDataForFranchise (FranchiseId franId)
{
    FranchiseIdDataVectorType::iterator iterFrans = mFranchiseIdData.begin();
    FranchiseIdDataVectorType::iterator iterFransEnd = mFranchiseIdData.end();
    for (; iterFrans != iterFransEnd; iterFrans++)
    {
        FranchiseIdData* data = *iterFrans;
        if (data != NULL && data->fid == franId)
        {
            MemberDataVectorType::reverse_iterator iterMembers = data->memberData.rbegin();
            for (; iterMembers != data->memberData.rend(); iterMembers++)
            {
                //printf ("[%d] fid=%d DELETING MEMBER=%s, tid=%d, bzid=%d\n\r",
                //    mId, franId, (*iterMembers)->persona, (*iterMembers)->tid, (*iterMembers)->bzid); // DRL DEBUG ONLY
                delete *iterMembers;
            }
            data->memberData.clear ();
            break;
        }
    }
}

bool FranchiseInstance::isFranchiseInFranchiseVector (FranchiseId franId)
{
    FranchiseIdDataVectorType::iterator iterFrans = mFranchiseIdData.begin();
    FranchiseIdDataVectorType::iterator iterFransEnd = mFranchiseIdData.end();
    for (; iterFrans != iterFransEnd; iterFrans++)
    {
        FranchiseIdData* data = *iterFrans;
        if (data != NULL && data->fid == franId)
        {
            return true;
        }
    }
    return false;
}

FranchiseId FranchiseInstance::getFranchiseWithFantasyDraftSetting (bool fantasyDraftDesired, bool needGM, BlazeId bzidDesired)
{
    FranchiseIdDataVectorType::iterator iterFrans = mFranchiseIdData.begin();
    FranchiseIdDataVectorType::iterator iterFransEnd = mFranchiseIdData.end();

    BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] getFranchiseWithFantasyDraftSetting - fant desired=%d, needGM=%d, bzidDesired=%d",
        mId, fantasyDraftDesired, needGM, bzidDesired); // drl debug only

    // Iterate thru all franchises known.
    for (; iterFrans != iterFransEnd; iterFrans++)
    {
        FranchiseIdData* data = *iterFrans;
        if (data != NULL && data->fantasyDraftIsOn == fantasyDraftDesired)
        {
            //BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] getFranchiseWithFantasyDraftSetting - FOUND fid=%d (needGM=%d, bzidDesired=%d)",
            //    mId, data->fid, needGM, bzidDesired); // drl debug only
            if (needGM || bzidDesired != 0)
            {
                MemberDataVectorType::iterator iterMembers = data->memberData.begin();
                MemberDataVectorType::iterator iterMembersEnd = data->memberData.end();
                // Iterate thru all members of this franchise.
                for (; iterMembers != iterMembersEnd; iterMembers++)
                {
                    if (needGM && !(*iterMembers)->isGM)
                    {
                        continue;
                    }
                    if (bzidDesired != 0 && bzidDesired != (*iterMembers)->bzid)
                    {
                        continue;
                    }
                    //BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] getFranchiseWithFantasyDraftSetting - FOUND MEMBER - fid=%d, needGM=%d, bzidDesired=%d",
                    //    mId, data->fid, needGM, bzidDesired); // drl debug only
                    break;
                }
                if (iterMembers == iterMembersEnd)
                {
                    //BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] getFranchiseWithFantasyDraftSetting - DIDN'T FIND MEMBER - fid=%d, needGM=%d, bzidDesired=%d",
                    //    mId, data->fid, needGM, bzidDesired); // drl debug only
                    continue;   // we didn't meet the condition of GM and/or bzid for this franchise
                }
            }
            //BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] getFranchiseWithFantasyDraftSetting - found fid=%d", mId, data->fid); // drl debug only
            return data->fid;
        }
    }
    //BLAZE_DEBUG (BlazeRpcLog::franchise, "[%d] getFranchiseWithFantasyDraftSetting - FAILURE FAILURE FAILURE -------", mId); // drl debug only
    return 0;
}

} // Stress
} // Blaze

