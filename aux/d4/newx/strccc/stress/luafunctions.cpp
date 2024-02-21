/*************************************************************************************************/
/*!
    \file   luafunctions.cpp

    $Header: //blaze/games/Madden/2010-NG/trunk/component/franchise/stress/luafunctions.cpp#1 $
    $Change: 44620 $
    $DateTime: 2009/06/04 08:15:11 $

    \attention
        (c) Electronic Arts Inc. 2009
*/
/*************************************************************************************************/

/*************************************************************************************************/
/*!
    luafunctions.cpp

    These Lua to C++ bindings are machine generated.  Take care when modifying by hand.

*/
/*************************************************************************************************/

#include "framework/blaze.h"
#include "franchisemodule.h"
#include "franchisestress.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

extern "C"
{
#include "external/lua/lua.h"
#include "external/lua/lualib.h"
#include "external/lua/lauxlib.h"
}

#include "luathread.h"
#include "luastats.h"

namespace Blaze
{
namespace Stress
{

namespace Lua
{

static int RpcOk(lua_State *lua)
{
    lua_pushinteger(lua, ERR_OK);
    return 1;
}

static int RpcFranchiseNameInUse(lua_State *lua)
{
    lua_pushinteger(lua, FRANCHISE_ERR_FRANCHISE_NAME_IN_USE);
    return 1;
};

static int GetLastRpcError(lua_State *lua)
{
    lua_pushinteger(lua, LuaStats::GetInstance()->GetLastRpcError());
    return 1;
}

static int SetYieldAll(lua_State *lua)
{
    // the second needs to be our argument
    luaL_checktype(lua, -1, LUA_TBOOLEAN);
    LuaStats::GetInstance()->SetYieldAll(lua_toboolean(lua, -1) != 0);
    return 0;
}

int CreateFranchise(lua_State *lua)
{
    static const eastl::string name("CreateFranchise");
    
    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);
    // the second needs to be our franchise id
    luaL_checktype(lua, -1, LUA_TNUMBER);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);
    int int0 = (int)lua_tointeger(lua, -1),
        result = 0;

    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());
    if (instance)
    {
		TimeValue start(TimeValue::getTimeOfDay());
        LuaStats::GetInstance()->ReportError(name, instance->createFranchiseLocal(int0, (FranchiseId *)&result));
		LuaStats::GetInstance()->ReportTime(name, start);
    }    

    printf("CreateFranchise instance %p, arg int 0 %d\n", instance, int0);

    lua_pushinteger(lua, (int)result);
    
    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 1) : 1;
}

int JoinFranchise(lua_State *lua)
{
    static const eastl::string name("JoinFranchise");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);
    // the second needs to be our franchise id
    luaL_checktype(lua, -1, LUA_TNUMBER);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);
    int int0 = (int)lua_tointeger(lua, -1);

    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());
    BlazeRpcError result = ERR_OK;
    if (instance)
    {
		TimeValue start(TimeValue::getTimeOfDay());
        result = LuaStats::GetInstance()->ReportError(name.c_str(), instance->joinFranchiseLocal(int0));
		LuaStats::GetInstance()->ReportTime(name, start);
    }

    printf("JoinFranchise instance %p, franchise id %d\n", instance, int0);

    lua_pushinteger(lua, (int)result);
    
    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 1) : 1;
}

int DeleteFranchise(lua_State *lua)
{
    static const eastl::string name("DeleteFranchise");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);
    // the second needs to be our franchise id
    luaL_checktype(lua, -1, LUA_TNUMBER);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);
    int int0 = (int)lua_tointeger(lua, -1);

    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());
    BlazeRpcError result = ERR_OK;
    if (instance)
    {
		TimeValue start(TimeValue::getTimeOfDay());
        result = LuaStats::GetInstance()->ReportError(name.c_str(), instance->deleteFranchiseLocal(int0));
		LuaStats::GetInstance()->ReportTime(name, start);
    }

    printf("DeleteFranchise instance %p, franchise id %d\n", instance, int0);

    lua_pushinteger(lua, (int)result);
    
    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 1) : 1;
}

int GetFranchise(lua_State *lua)
{
    static const eastl::string name("GetFranchise");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);
    // the second needs to be our franchise id
    luaL_checktype(lua, -1, LUA_TNUMBER);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);
    int int0 = (int)lua_tointeger(lua, -1);

    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());
    BlazeRpcError result = ERR_OK;
    if (instance)
    {
		TimeValue start(TimeValue::getTimeOfDay());
        result = LuaStats::GetInstance()->ReportError(name.c_str(), instance->getFranchiseLocal(int0));
		LuaStats::GetInstance()->ReportTime(name, start);
    }

    printf("GetFranchise instance %p, franchise id %d\n", instance, int0);

    lua_pushinteger(lua, (int)result);
    
    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 1) : 1;
}

int UpdateFranchiseSettings(lua_State *lua)
{
    static const eastl::string name("UpdateFranchiseSettings");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);
    // the second needs to be our franchise id
    luaL_checktype(lua, -1, LUA_TNUMBER);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);
    int int0 = (int)lua_tointeger(lua, -1);

    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());
    BlazeRpcError result = ERR_OK;
    if (instance)
    {
		TimeValue start(TimeValue::getTimeOfDay());
        result = LuaStats::GetInstance()->ReportError(name.c_str(), instance->updateFranchiseSettingsLocal(int0));
		LuaStats::GetInstance()->ReportTime(name, start);
    }

    printf("UpdateFranchiseSettings instance %p, franchise id %d\n", instance, int0);

    lua_pushinteger(lua, (int)result);
    
    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 1) : 1;
}

int GetMemberByBlazeId(lua_State *lua)
{
    static const eastl::string name("GetMemberByBlazeId");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);
    // the second needs to be our franchise id
    luaL_checktype(lua, -1, LUA_TNUMBER);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);
    int int0 = (int)lua_tointeger(lua, -1);

    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());
    BlazeRpcError result = ERR_OK;
    if (instance)
    {
		TimeValue start(TimeValue::getTimeOfDay());
        result = LuaStats::GetInstance()->ReportError(name.c_str(), instance->getMemberByBlazeIdLocal(int0));
		LuaStats::GetInstance()->ReportTime(name, start);
    }

    printf("GetMemberByBlazeId instance %p, franchise id %d\n", instance, int0);

    lua_pushinteger(lua, (int)result);
    
    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 1) : 1;
}

int GetMemberByTeamId(lua_State *lua)
{
    static const eastl::string name("GetMemberByTeamId");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);
    // the second needs to be our franchise id
    luaL_checktype(lua, -1, LUA_TNUMBER);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);
    int int0 = (int)lua_tointeger(lua, -1);

    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());
    BlazeRpcError result = ERR_OK;
    if (instance)
    {
		TimeValue start(TimeValue::getTimeOfDay());
        result = LuaStats::GetInstance()->ReportError(name.c_str(), instance->getMemberByTeamIdLocal(int0));
		LuaStats::GetInstance()->ReportTime(name, start);
    }

    printf("GetMemberByTeamId instance %p, franchise id %d\n", instance, int0);

    lua_pushinteger(lua, (int)result);
    
    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 1) : 1;
}

int RemoveMember(lua_State *lua)
{
    static const eastl::string name("RemoveMember");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);
    // the second needs to be our franchise id
    luaL_checktype(lua, -1, LUA_TNUMBER);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);
    int int0 = (int)lua_tointeger(lua, -1);

    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());
    BlazeRpcError result = ERR_OK;
    if (instance)
    {
		TimeValue start(TimeValue::getTimeOfDay());
        result = LuaStats::GetInstance()->ReportError(name.c_str(), instance->removeMemberLocal(int0));
		LuaStats::GetInstance()->ReportTime(name, start);
    }

    printf("RemoveMember instance %p, franchise id %d\n", instance, int0);

    lua_pushinteger(lua, (int)result);
    
    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 1) : 1;
}

int GetMembers(lua_State *lua)
{
    static const eastl::string name("GetMembers");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);
    // the second needs to be our franchise id
    luaL_checktype(lua, -1, LUA_TNUMBER);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);
    int int0 = (int)lua_tointeger(lua, -1);

    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());
    BlazeRpcError result = ERR_OK;
    if (instance)
    {
		TimeValue start(TimeValue::getTimeOfDay());
        result = LuaStats::GetInstance()->ReportError(name.c_str(), instance->getMembersLocal(int0));
		LuaStats::GetInstance()->ReportTime(name, start);
    }

    printf("GetMembers instance %p, franchise id %d\n", instance, int0);

    lua_pushinteger(lua, (int)result);
    
    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 1) : 1;
}

int PromoteToGM(lua_State *lua)
{
    static const eastl::string name("PromoteToGM");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);
    // the second needs to be our franchise id
    luaL_checktype(lua, -1, LUA_TNUMBER);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);
    int int0 = (int)lua_tointeger(lua, -1);

    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());
    BlazeRpcError result = ERR_OK;
    if (instance)
    {
		TimeValue start(TimeValue::getTimeOfDay());
        result = LuaStats::GetInstance()->ReportError(name.c_str(), instance->promoteToGMLocal(int0));
		LuaStats::GetInstance()->ReportTime(name, start);
    }

    printf("PromoteToGM instance %p, franchise id %d\n", instance, int0);

    lua_pushinteger(lua, (int)result);
    
    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 1) : 1;
}

int GetRegularSchedule(lua_State *lua)
{
    static const eastl::string name("GetRegularSchedule");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);
    // the second needs to be our franchise id
    luaL_checktype(lua, -1, LUA_TNUMBER);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);
    int int0 = (int)lua_tointeger(lua, -1);

    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());
    BlazeRpcError result = ERR_OK;
    if (instance)
    {
		TimeValue start(TimeValue::getTimeOfDay());
        result = LuaStats::GetInstance()->ReportError(name.c_str(), instance->getRegularScheduleLocal(int0));
		LuaStats::GetInstance()->ReportTime(name, start);
    }

    printf("GetRegularSchedule instance %p, franchise id %d\n", instance, int0);

    lua_pushinteger(lua, (int)result);
    
    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 1) : 1;
}

int GetPlayoffSchedule(lua_State *lua)
{
    static const eastl::string name("GetPlayoffSchedule");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);
    // the second needs to be our franchise id
    luaL_checktype(lua, -1, LUA_TNUMBER);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);
    int int0 = (int)lua_tointeger(lua, -1);

    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());
    BlazeRpcError result = ERR_OK;
    if (instance)
    {
		TimeValue start(TimeValue::getTimeOfDay());
        result = LuaStats::GetInstance()->ReportError(name.c_str(), instance->getPlayoffScheduleLocal(int0));
		LuaStats::GetInstance()->ReportTime(name, start);
    }

    printf("GetPlayoffSchedule instance %p, franchise id %d\n", instance, int0);

    lua_pushinteger(lua, (int)result);
    
    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 1) : 1;
}

int GetDisplayRoster(lua_State *lua)
{
    static const eastl::string name("GetDisplayRoster");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);
    // the second needs to be our franchise id
    luaL_checktype(lua, -1, LUA_TNUMBER);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);
    int int0 = (int)lua_tointeger(lua, -1);

    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());
    BlazeRpcError result = ERR_OK;
    if (instance)
    {
		TimeValue start(TimeValue::getTimeOfDay());
        result = LuaStats::GetInstance()->ReportError(name.c_str(), instance->getDisplayRosterLocal(int0));
		LuaStats::GetInstance()->ReportTime(name, start);
    }

    printf("GetDisplayRoster instance %p, franchise id %d\n", instance, int0);

    lua_pushinteger(lua, (int)result);
    
    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 1) : 1;
}

int GetGameplayRoster(lua_State *lua)
{
    static const eastl::string name("GetGameplayRoster");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);
    // the second needs to be our franchise id
    luaL_checktype(lua, -1, LUA_TNUMBER);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);
    int int0 = (int)lua_tointeger(lua, -1);

    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());
    BlazeRpcError result = ERR_OK;
    if (instance)
    {
		TimeValue start(TimeValue::getTimeOfDay());
        result = LuaStats::GetInstance()->ReportError(name.c_str(), instance->getGameplayRosterLocal(int0));
		LuaStats::GetInstance()->ReportTime(name, start);
    }

    printf("GetGameplayRoster instance %p, franchise id %d\n", instance, int0);

    lua_pushinteger(lua, (int)result);
    
    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 1) : 1;
}

int GetNumTeamInjuries(lua_State *lua)
{
    static const eastl::string name("GetNumTeamInjuries");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);
    // the second needs to be our franchise id
    luaL_checktype(lua, -1, LUA_TNUMBER);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);
    int int0 = (int)lua_tointeger(lua, -1);

    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());
    BlazeRpcError result = ERR_OK;
    if (instance)
    {
		TimeValue start(TimeValue::getTimeOfDay());
        result = LuaStats::GetInstance()->ReportError(name.c_str(), instance->getNumTeamInjuriesLocal(int0));
		LuaStats::GetInstance()->ReportTime(name, start);
    }

    printf("GetNumTeamInjuries instance %p, franchise id %d\n", instance, int0);

    lua_pushinteger(lua, (int)result);
    
    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 1) : 1;
}

int GetInjuriesByTeam(lua_State *lua)
{
    static const eastl::string name("GetInjuriesByTeam");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);
    // the second needs to be our franchise id
    luaL_checktype(lua, -1, LUA_TNUMBER);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);
    int int0 = (int)lua_tointeger(lua, -1);

    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());
    BlazeRpcError result = ERR_OK;
    if (instance)
    {
		TimeValue start(TimeValue::getTimeOfDay());
        result = LuaStats::GetInstance()->ReportError(name.c_str(), instance->getInjuriesByTeamLocal(int0));
		LuaStats::GetInstance()->ReportTime(name, start);
    }

    printf("GetInjuriesByTeam instance %p, franchise id %d\n", instance, int0);

    lua_pushinteger(lua, (int)result);
    
    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 1) : 1;
}

int GetGameBoxScoreByGame(lua_State *lua)
{
    static const eastl::string name("GetGameBoxScoreByGame");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);
    // the second needs to be our franchise id
    luaL_checktype(lua, -1, LUA_TNUMBER);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);
    int int0 = (int)lua_tointeger(lua, -1);

    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());
    BlazeRpcError result = ERR_OK;
    if (instance)
    {
		TimeValue start(TimeValue::getTimeOfDay());
        result = LuaStats::GetInstance()->ReportError(name.c_str(), instance->getGameBoxScoreByGameLocal(int0));
		LuaStats::GetInstance()->ReportTime(name, start);
    }

    printf("GetGameBoxScoreByGame instance %p, franchise id %d\n", instance, int0);

    lua_pushinteger(lua, (int)result);
    
    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 1) : 1;
}

int GetGameBoxScoresByWeek(lua_State *lua)
{
    static const eastl::string name("GetGameBoxScoresByWeek");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);
    // the second needs to be our franchise id
    luaL_checktype(lua, -1, LUA_TNUMBER);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);
    int int0 = (int)lua_tointeger(lua, -1);

    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());
    BlazeRpcError result = ERR_OK;
    if (instance)
    {
		TimeValue start(TimeValue::getTimeOfDay());
        result = LuaStats::GetInstance()->ReportError(name.c_str(), instance->getGameBoxScoresByWeekLocal(int0));
		LuaStats::GetInstance()->ReportTime(name, start);
    }

    printf("GetGameBoxScoresByWeek instance %p, franchise id %d\n", instance, int0);

    lua_pushinteger(lua, (int)result);
    
    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 1) : 1;
}

int GetGameBoxScoresByTeam(lua_State *lua)
{
    static const eastl::string name("GetGameBoxScoresByTeam");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);
    // the second needs to be our franchise id
    luaL_checktype(lua, -1, LUA_TNUMBER);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);
    int int0 = (int)lua_tointeger(lua, -1);

    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());
    BlazeRpcError result = ERR_OK;
    if (instance)
    {
		TimeValue start(TimeValue::getTimeOfDay());
        result = LuaStats::GetInstance()->ReportError(name.c_str(), instance->getGameBoxScoresByTeamLocal(int0));
		LuaStats::GetInstance()->ReportTime(name, start);
    }

    printf("GetGameBoxScoresByTeam instance %p, franchise id %d\n", instance, int0);

    lua_pushinteger(lua, (int)result);
    
    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 1) : 1;
}

int GetScoringSummary(lua_State *lua)
{
    static const eastl::string name("GetScoringSummary");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);
    // the second needs to be our franchise id
    luaL_checktype(lua, -1, LUA_TNUMBER);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);
    int int0 = (int)lua_tointeger(lua, -1);

    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());
    BlazeRpcError result = ERR_OK;
    if (instance)
    {
		TimeValue start(TimeValue::getTimeOfDay());
        result = LuaStats::GetInstance()->ReportError(name.c_str(), instance->getScoringSummaryLocal(int0));
		LuaStats::GetInstance()->ReportTime(name, start);
    }

    printf("GetScoringSummary instance %p, franchise id %d\n", instance, int0);

    lua_pushinteger(lua, (int)result);
    
    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 1) : 1;
}

int GetTeamStatsByGame(lua_State *lua)
{
    static const eastl::string name("GetTeamStatsByGame");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);
    // the second needs to be our franchise id
    luaL_checktype(lua, -1, LUA_TNUMBER);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);
    int int0 = (int)lua_tointeger(lua, -1);

    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());
    BlazeRpcError result = ERR_OK;
    if (instance)
    {
		TimeValue start(TimeValue::getTimeOfDay());
        result = LuaStats::GetInstance()->ReportError(name.c_str(), instance->getTeamStatsByGameLocal(int0));
		LuaStats::GetInstance()->ReportTime(name, start);
    }

    printf("GetTeamStatsByGame instance %p, franchise id %d\n", instance, int0);

    lua_pushinteger(lua, (int)result);
    
    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 1) : 1;
}

int GetAthleteStatsByGame(lua_State *lua)
{
    static const eastl::string name("GetAthleteStatsByGame");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);
    // the second needs to be our franchise id
    luaL_checktype(lua, -1, LUA_TNUMBER);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);
    int int0 = (int)lua_tointeger(lua, -1);

    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());
    BlazeRpcError result = ERR_OK;
    if (instance)
    {
		TimeValue start(TimeValue::getTimeOfDay());
        result = LuaStats::GetInstance()->ReportError(name.c_str(), instance->getAthleteStatsByGameLocal(int0));
		LuaStats::GetInstance()->ReportTime(name, start);
    }

    printf("GetAthleteStatsByGame instance %p, franchise id %d\n", instance, int0);

    lua_pushinteger(lua, (int)result);
    
    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 1) : 1;
}

int ClearBan(lua_State *lua)
{
    static const eastl::string name("ClearBan");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);
    // the second needs to be our franchise id
    luaL_checktype(lua, -1, LUA_TNUMBER);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);
    int int0 = (int)lua_tointeger(lua, -1);

    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());
    BlazeRpcError result = ERR_OK;
    if (instance)
    {
		TimeValue start(TimeValue::getTimeOfDay());
        result = LuaStats::GetInstance()->ReportError(name.c_str(), instance->clearBanLocal(int0));
		LuaStats::GetInstance()->ReportTime(name, start);
    }

    printf("ClearBan instance %p, franchise id %d\n", instance, int0);

    lua_pushinteger(lua, (int)result);
    
    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 1) : 1;
}

int GetSuperbowls(lua_State *lua)
{
    static const eastl::string name("GetSuperbowls");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);
    // the second needs to be our franchise id
    luaL_checktype(lua, -1, LUA_TNUMBER);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);
    int int0 = (int)lua_tointeger(lua, -1);

    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());
    BlazeRpcError result = ERR_OK;
    if (instance)
    {
		TimeValue start(TimeValue::getTimeOfDay());
        result = LuaStats::GetInstance()->ReportError(name.c_str(), instance->getSuperbowlsLocal(int0));
		LuaStats::GetInstance()->ReportTime(name, start);
    }

    printf("GetSuperbowls instance %p, franchise id %d\n", instance, int0);

    lua_pushinteger(lua, (int)result);
    
    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 1) : 1;
}

int GetDisplayAthlete(lua_State *lua)
{
    static const eastl::string name("GetDisplayAthlete");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);
    // the second needs to be our franchise id
    luaL_checktype(lua, -1, LUA_TNUMBER);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);
    int int0 = (int)lua_tointeger(lua, -1);

    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());
    BlazeRpcError result = ERR_OK;
    if (instance)
    {
		TimeValue start(TimeValue::getTimeOfDay());
        result = LuaStats::GetInstance()->ReportError(name.c_str(), instance->getDisplayAthleteLocal(int0));
		LuaStats::GetInstance()->ReportTime(name, start);
    }

    printf("GetDisplayAthlete instance %p, franchise id %d\n", instance, int0);

    lua_pushinteger(lua, (int)result);
    
    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 1) : 1;
}

int GetRawData(lua_State *lua)
{
    static const eastl::string name("GetRawData");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);
    // the second needs to be our franchise id
    luaL_checktype(lua, -1, LUA_TNUMBER);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);
    int int0 = (int)lua_tointeger(lua, -1);

    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());
    BlazeRpcError result = ERR_OK;
    if (instance)
    {
		TimeValue start(TimeValue::getTimeOfDay());
        result = LuaStats::GetInstance()->ReportError(name.c_str(), instance->getRawDataLocal(int0));
		LuaStats::GetInstance()->ReportTime(name, start);
    }

    printf("GetRawData instance %p, franchise id %d\n", instance, int0);

    lua_pushinteger(lua, (int)result);
    
    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 1) : 1;
}

int GetDepthChart(lua_State *lua)
{
    static const eastl::string name("GetDepthChart");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);
    // the second needs to be our franchise id
    luaL_checktype(lua, -1, LUA_TNUMBER);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);
    int int0 = (int)lua_tointeger(lua, -1);

    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());
    BlazeRpcError result = ERR_OK;
    if (instance)
    {
		TimeValue start(TimeValue::getTimeOfDay());
        result = LuaStats::GetInstance()->ReportError(name.c_str(), instance->getDepthChartLocal(int0));
		LuaStats::GetInstance()->ReportTime(name, start);
    }

    printf("GetDepthChart instance %p, franchise id %d\n", instance, int0);

    lua_pushinteger(lua, (int)result);
    
    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 1) : 1;
}

int GetNews(lua_State *lua)
{
    static const eastl::string name("GetNews");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);
    // the second needs to be our franchise id
    luaL_checktype(lua, -1, LUA_TNUMBER);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);
    int int0 = (int)lua_tointeger(lua, -1);

    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());
    BlazeRpcError result = ERR_OK;
    if (instance)
    {
		TimeValue start(TimeValue::getTimeOfDay());
        result = LuaStats::GetInstance()->ReportError(name.c_str(), instance->getNewsLocal(int0));
		LuaStats::GetInstance()->ReportTime(name, start);
    }

    printf("GetNews instance %p, franchise id %d\n", instance, int0);

    lua_pushinteger(lua, (int)result);
    
    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 1) : 1;
}

int GetInvitations(lua_State *lua)
{
    static const eastl::string name("GetInvitations");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);
    // the second needs to be our franchise id
    luaL_checktype(lua, -1, LUA_TNUMBER);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);
    int int0 = (int)lua_tointeger(lua, -1);

    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());
    BlazeRpcError result = ERR_OK;
    if (instance)
    {
		TimeValue start(TimeValue::getTimeOfDay());
        result = LuaStats::GetInstance()->ReportError(name.c_str(), instance->getInvitationsLocal(int0));
		LuaStats::GetInstance()->ReportTime(name, start);
    }

    printf("GetInvitations instance %p, franchise id %d\n", instance, int0);

    lua_pushinteger(lua, (int)result);
    
    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 1) : 1;
}

int GetDraftProfile(lua_State *lua)
{
    static const eastl::string name("GetDraftProfile");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);
    // the second needs to be our franchise id
    luaL_checktype(lua, -1, LUA_TNUMBER);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);
    int int0 = (int)lua_tointeger(lua, -1);

    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());
    BlazeRpcError result = ERR_OK;
    if (instance)
    {
		TimeValue start(TimeValue::getTimeOfDay());
        result = LuaStats::GetInstance()->ReportError(name.c_str(), instance->getDraftProfileLocal(int0));
		LuaStats::GetInstance()->ReportTime(name, start);
    }

    printf("GetDraftProfile instance %p, franchise id %d\n", instance, int0);

    lua_pushinteger(lua, (int)result);
    
    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 1) : 1;
}

int GetDraftHistory(lua_State *lua)
{
    static const eastl::string name("GetDraftHistory");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);
    // the second needs to be our franchise id
    luaL_checktype(lua, -1, LUA_TNUMBER);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);
    int int0 = (int)lua_tointeger(lua, -1);

    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());
    BlazeRpcError result = ERR_OK;
    if (instance)
    {
		TimeValue start(TimeValue::getTimeOfDay());
        result = LuaStats::GetInstance()->ReportError(name.c_str(), instance->getDraftHistoryLocal(int0));
		LuaStats::GetInstance()->ReportTime(name, start);
    }

    printf("GetDraftHistory instance %p, franchise id %d\n", instance, int0);

    lua_pushinteger(lua, (int)result);
    
    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 1) : 1;
}

int GetDraftableAthletes(lua_State *lua)
{
    static const eastl::string name("GetDraftableAthletes");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);
    // the second needs to be our franchise id
    luaL_checktype(lua, -1, LUA_TNUMBER);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);
    int int0 = (int)lua_tointeger(lua, -1);

    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());
    BlazeRpcError result = ERR_OK;
    if (instance)
    {
		TimeValue start(TimeValue::getTimeOfDay());
        result = LuaStats::GetInstance()->ReportError(name.c_str(), instance->getDraftableAthletesLocal(int0));
		LuaStats::GetInstance()->ReportTime(name, start);
    }

    printf("GetDraftableAthletes instance %p, franchise id %d\n", instance, int0);

    lua_pushinteger(lua, (int)result);
    
    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 1) : 1;
}

int GetDraftOrder(lua_State *lua)
{
    static const eastl::string name("GetDraftOrder");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);
    // the second needs to be our franchise id
    luaL_checktype(lua, -1, LUA_TNUMBER);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);
    int int0 = (int)lua_tointeger(lua, -1);

    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());
    BlazeRpcError result = ERR_OK;
    if (instance)
    {
		TimeValue start(TimeValue::getTimeOfDay());
        result = LuaStats::GetInstance()->ReportError(name.c_str(), instance->getDraftOrderLocal(int0));
		LuaStats::GetInstance()->ReportTime(name, start);
    }

    printf("GetDraftOrder instance %p, franchise id %d\n", instance, int0);

    lua_pushinteger(lua, (int)result);
    
    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 1) : 1;
}

int GetTopFranchises(lua_State *lua)
{
    static const eastl::string name("GetTopFranchises");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);
    // the second needs to be our franchise id
    luaL_checktype(lua, -1, LUA_TNUMBER);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);
    int int0 = (int)lua_tointeger(lua, -1);

    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());
    BlazeRpcError result = ERR_OK;
    if (instance)
    {
		TimeValue start(TimeValue::getTimeOfDay());
        result = LuaStats::GetInstance()->ReportError(name.c_str(), instance->getTopFranchisesLocal(int0));
		LuaStats::GetInstance()->ReportTime(name, start);
    }

    printf("GetTopFranchises instance %p, franchise id %d\n", instance, int0);

    lua_pushinteger(lua, (int)result);
    
    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 1) : 1;
}

int GetFranchiseRank(lua_State *lua)
{
    static const eastl::string name("GetFranchiseRank");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);
    // the second needs to be our franchise id
    luaL_checktype(lua, -1, LUA_TNUMBER);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);
    int int0 = (int)lua_tointeger(lua, -1);

    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());
    BlazeRpcError result = ERR_OK;
    if (instance)
    {
		TimeValue start(TimeValue::getTimeOfDay());
        result = LuaStats::GetInstance()->ReportError(name.c_str(), instance->getFranchiseRankLocal(int0));
		LuaStats::GetInstance()->ReportTime(name, start);
    }

    printf("GetFranchiseRank instance %p, franchise id %d\n", instance, int0);

    lua_pushinteger(lua, (int)result);
    
    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 1) : 1;
}

int ValidateRosters(lua_State *lua)
{
    static const eastl::string name("ValidateRosters");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);
    // the second needs to be our franchise id
    luaL_checktype(lua, -1, LUA_TNUMBER);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);
    int int0 = (int)lua_tointeger(lua, -1);

    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());
    BlazeRpcError result = ERR_OK;
    if (instance)
    {
		TimeValue start(TimeValue::getTimeOfDay());
        result = LuaStats::GetInstance()->ReportError(name.c_str(), instance->validateRostersLocal(int0));
		LuaStats::GetInstance()->ReportTime(name, start);
    }

    printf("ValidateRosters instance %p, franchise id %d\n", instance, int0);

    lua_pushinteger(lua, (int)result);
    
    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 1) : 1;
}

int ValidateDepthCharts(lua_State *lua)
{
    static const eastl::string name("ValidateDepthCharts");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);
    // the second needs to be our franchise id
    luaL_checktype(lua, -1, LUA_TNUMBER);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);
    int int0 = (int)lua_tointeger(lua, -1);

    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());
    BlazeRpcError result = ERR_OK;
    if (instance)
    {
		TimeValue start(TimeValue::getTimeOfDay());
        result = LuaStats::GetInstance()->ReportError(name.c_str(), instance->validateDepthChartsLocal(int0));
		LuaStats::GetInstance()->ReportTime(name, start);
    }

    printf("ValidateDepthCharts instance %p, franchise id %d\n", instance, int0);

    lua_pushinteger(lua, (int)result);
    
    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 1) : 1;
}

int SetDraftProfile(lua_State *lua)
{
    static const eastl::string name("SetDraftProfile");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);
    // the second needs to be our franchise id
    luaL_checktype(lua, -1, LUA_TNUMBER);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);
    int int0 = (int)lua_tointeger(lua, -1);

    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());
    BlazeRpcError result = ERR_OK;
    if (instance)
    {
		TimeValue start(TimeValue::getTimeOfDay());
        result = LuaStats::GetInstance()->ReportError(name.c_str(), instance->setDraftProfileLocal(int0));
		LuaStats::GetInstance()->ReportTime(name, start);
    }

    printf("SetDraftProfile instance %p, franchise id %d\n", instance, int0);

    lua_pushinteger(lua, (int)result);
    
    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 1) : 1;
}

int PostNews(lua_State *lua)
{
    static const eastl::string name("PostNews");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);
    // the second needs to be our franchise id
    luaL_checktype(lua, -1, LUA_TNUMBER);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);
    int int0 = (int)lua_tointeger(lua, -1);

    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());
    BlazeRpcError result = ERR_OK;
    if (instance)
    {
		TimeValue start(TimeValue::getTimeOfDay());
        result = LuaStats::GetInstance()->ReportError(name.c_str(), instance->postNewsLocal(int0));
		LuaStats::GetInstance()->ReportTime(name, start);
    }

    printf("PostNews instance %p, franchise id %d\n", instance, int0);

    lua_pushinteger(lua, (int)result);
    
    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 1) : 1;
}

int SendInvitation(lua_State *lua)
{
    static const eastl::string name("SendInvitation");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);
    // the second needs to be our franchise id
    luaL_checktype(lua, -1, LUA_TNUMBER);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);
    int int0 = (int)lua_tointeger(lua, -1);

    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());
    BlazeRpcError result = ERR_OK;
    if (instance)
    {
		TimeValue start(TimeValue::getTimeOfDay());
        result = LuaStats::GetInstance()->ReportError(name.c_str(), instance->sendInvitationLocal(int0));
		LuaStats::GetInstance()->ReportTime(name, start);
    }

    printf("SendInvitation instance %p, franchise id %d\n", instance, int0);

    lua_pushinteger(lua, (int)result);
    
    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 1) : 1;
}

int ProcessInvitation(lua_State *lua)
{
    static const eastl::string name("ProcessInvitation");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);
    // the second needs to be our franchise id
    luaL_checktype(lua, -1, LUA_TNUMBER);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);
    int int0 = (int)lua_tointeger(lua, -1);

    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());
    BlazeRpcError result = ERR_OK;
    if (instance)
    {
		TimeValue start(TimeValue::getTimeOfDay());
        result = LuaStats::GetInstance()->ReportError(name.c_str(), instance->processInvitationLocal(int0));
		LuaStats::GetInstance()->ReportTime(name, start);
    }

    printf("ProcessInvitation instance %p, franchise id %d\n", instance, int0);

    lua_pushinteger(lua, (int)result);
    
    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 1) : 1;
}

int JoinDraft(lua_State *lua)
{
    static const eastl::string name("JoinDraft");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);
    // the second needs to be our franchise id
    luaL_checktype(lua, -1, LUA_TNUMBER);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);
    int int0 = (int)lua_tointeger(lua, -1);

    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());
    BlazeRpcError result = ERR_OK;
    if (instance)
    {
		TimeValue start(TimeValue::getTimeOfDay());
        result = LuaStats::GetInstance()->ReportError(name.c_str(), instance->joinDraftLocal(int0));
		LuaStats::GetInstance()->ReportTime(name, start);
    }

    printf("JoinDraft instance %p, franchise id %d\n", instance, int0);

    lua_pushinteger(lua, (int)result);
    
    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 1) : 1;
}

int LeaveDraft(lua_State *lua)
{
    static const eastl::string name("LeaveDraft");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);
    // the second needs to be our franchise id
    luaL_checktype(lua, -1, LUA_TNUMBER);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);
    int int0 = (int)lua_tointeger(lua, -1);

    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());
    BlazeRpcError result = ERR_OK;
    if (instance)
    {
		TimeValue start(TimeValue::getTimeOfDay());
        result = LuaStats::GetInstance()->ReportError(name.c_str(), instance->leaveDraftLocal(int0));
		LuaStats::GetInstance()->ReportTime(name, start);
    }

    printf("LeaveDraft instance %p, franchise id %d\n", instance, int0);

    lua_pushinteger(lua, (int)result);
    
    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 1) : 1;
}

int BeginDraft(lua_State *lua)
{
    static const eastl::string name("BeginDraft");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);
    // the second needs to be our franchise id
    luaL_checktype(lua, -1, LUA_TNUMBER);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);
    int int0 = (int)lua_tointeger(lua, -1);

    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());
    BlazeRpcError result = ERR_OK;
    if (instance)
    {
		TimeValue start(TimeValue::getTimeOfDay());
        result = LuaStats::GetInstance()->ReportError(name.c_str(), instance->beginDraftLocal(int0));
		LuaStats::GetInstance()->ReportTime(name, start);
    }

    printf("BeginDraft instance %p, franchise id %d\n", instance, int0);

    lua_pushinteger(lua, (int)result);
    
    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 1) : 1;
}

int PauseDraft(lua_State *lua)
{
    static const eastl::string name("PauseDraft");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);
    // the second needs to be our franchise id
    luaL_checktype(lua, -1, LUA_TNUMBER);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);
    int int0 = (int)lua_tointeger(lua, -1);

    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());
    BlazeRpcError result = ERR_OK;
    if (instance)
    {
		TimeValue start(TimeValue::getTimeOfDay());
        result = LuaStats::GetInstance()->ReportError(name.c_str(), instance->pauseDraftLocal(int0));
		LuaStats::GetInstance()->ReportTime(name, start);
    }

    printf("PauseDraft instance %p, franchise id %d\n", instance, int0);

    lua_pushinteger(lua, (int)result);
    
    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 1) : 1;
}

int DraftAthlete(lua_State *lua)
{
    static const eastl::string name("DraftAthlete");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);
    // the second needs to be our franchise id
    luaL_checktype(lua, -1, LUA_TNUMBER);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);
    int int0 = (int)lua_tointeger(lua, -1);

    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());
    BlazeRpcError result = ERR_OK;
    if (instance)
    {
		TimeValue start(TimeValue::getTimeOfDay());
        result = LuaStats::GetInstance()->ReportError(name.c_str(), instance->draftAthleteLocal(int0));
		LuaStats::GetInstance()->ReportTime(name, start);
    }

    printf("DraftAthlete instance %p, franchise id %d\n", instance, int0);

    lua_pushinteger(lua, (int)result);
    
    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 1) : 1;
}

int SendDraftChat(lua_State *lua)
{
    static const eastl::string name("SendDraftChat");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);
    // the second needs to be our franchise id
    luaL_checktype(lua, -1, LUA_TNUMBER);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);
    int int0 = (int)lua_tointeger(lua, -1);

    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());
    BlazeRpcError result = ERR_OK;
    if (instance)
    {
		TimeValue start(TimeValue::getTimeOfDay());
        result = LuaStats::GetInstance()->ReportError(name.c_str(), instance->sendDraftChatLocal(int0));
		LuaStats::GetInstance()->ReportTime(name, start);
    }

    printf("SendDraftChat instance %p, franchise id %d\n", instance, int0);

    lua_pushinteger(lua, (int)result);
    
    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 1) : 1;
}

int PollDraft(lua_State *lua)
{
    static const eastl::string name("PollDraft");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);
    // the second needs to be our franchise id
    luaL_checktype(lua, -1, LUA_TNUMBER);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);
    int int0 = (int)lua_tointeger(lua, -1);

    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());
    BlazeRpcError result = ERR_OK;
    if (instance)
    {
		TimeValue start(TimeValue::getTimeOfDay());
        result = LuaStats::GetInstance()->ReportError(name.c_str(), instance->pollDraftLocal(int0));
		LuaStats::GetInstance()->ReportTime(name, start);
    }

    printf("PollDraft instance %p, franchise id %d\n", instance, int0);

    lua_pushinteger(lua, (int)result);
    
    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 1) : 1;
}

int CheckDraft(lua_State *lua)
{
    static const eastl::string name("CheckDraft");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);
    // the second needs to be our franchise id
    luaL_checktype(lua, -1, LUA_TNUMBER);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);
    int int0 = (int)lua_tointeger(lua, -1);

    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());
    BlazeRpcError result = ERR_OK;
    if (instance)
    {
		TimeValue start(TimeValue::getTimeOfDay());
        result = LuaStats::GetInstance()->ReportError(name.c_str(), instance->checkDraftLocal(int0));
		LuaStats::GetInstance()->ReportTime(name, start);
    }

    printf("CheckDraft instance %p, franchise id %d\n", instance, int0);

    lua_pushinteger(lua, (int)result);
    
    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 1) : 1;
}

int SetDepthChart(lua_State *lua)
{
    static const eastl::string name("SetDepthChart");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);
    // the second needs to be our franchise id
    luaL_checktype(lua, -1, LUA_TNUMBER);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);
    int int0 = (int)lua_tointeger(lua, -1);

    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());
    BlazeRpcError result = ERR_OK;
    if (instance)
    {
		TimeValue start(TimeValue::getTimeOfDay());
        result = LuaStats::GetInstance()->ReportError(name.c_str(), instance->setDepthChartLocal(int0));
		LuaStats::GetInstance()->ReportTime(name, start);
    }

    printf("SetDepthChart instance %p, franchise id %d\n", instance, int0);

    lua_pushinteger(lua, (int)result);
    
    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 1) : 1;
}

int GenerateDepthChart(lua_State *lua)
{
    static const eastl::string name("GenerateDepthChart");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);
    // the second needs to be our franchise id
    luaL_checktype(lua, -1, LUA_TNUMBER);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);
    int int0 = (int)lua_tointeger(lua, -1);

    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());
    BlazeRpcError result = ERR_OK;
    if (instance)
    {
		TimeValue start(TimeValue::getTimeOfDay());
        result = LuaStats::GetInstance()->ReportError(name.c_str(), instance->generateDepthChartLocal(int0));
		LuaStats::GetInstance()->ReportTime(name, start);
    }

    printf("GenerateDepthChart instance %p, franchise id %d\n", instance, int0);

    lua_pushinteger(lua, (int)result);
    
    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 1) : 1;
}

int SimSeason(lua_State *lua)
{
    static const eastl::string name("SimSeason");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);
    // the second needs to be our franchise id
    luaL_checktype(lua, -1, LUA_TNUMBER);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);
    int int0 = (int)lua_tointeger(lua, -1);

    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());
    BlazeRpcError result = ERR_OK;
    if (instance)
    {
		TimeValue start(TimeValue::getTimeOfDay());
        result = LuaStats::GetInstance()->ReportError(name.c_str(), instance->simSeasonLocal(int0));
		LuaStats::GetInstance()->ReportTime(name, start);
    }

    printf("SimSeason instance %p, franchise id %d\n", instance, int0);

    lua_pushinteger(lua, (int)result);
    
    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 1) : 1;
}

int AthleteProgress(lua_State *lua)
{
    static const eastl::string name("AthleteProgress");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);
    // the second needs to be our franchise id
    luaL_checktype(lua, -1, LUA_TNUMBER);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);
    int int0 = (int)lua_tointeger(lua, -1);

    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());
    BlazeRpcError result = ERR_OK;
    if (instance)
    {
		TimeValue start(TimeValue::getTimeOfDay());
        result = LuaStats::GetInstance()->ReportError(name.c_str(), instance->athleteProgressLocal(int0));
		LuaStats::GetInstance()->ReportTime(name, start);
    }

    printf("AthleteProgress instance %p, franchise id %d\n", instance, int0);

    lua_pushinteger(lua, (int)result);
    
    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 1) : 1;
}

int SignAthlete(lua_State *lua)
{
    static const eastl::string name("SignAthlete");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);
    // the second needs to be our franchise id
    luaL_checktype(lua, -1, LUA_TNUMBER);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);
    int int0 = (int)lua_tointeger(lua, -1);

    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());
    BlazeRpcError result = ERR_OK;
    if (instance)
    {
		TimeValue start(TimeValue::getTimeOfDay());
        result = LuaStats::GetInstance()->ReportError(name.c_str(), instance->signAthleteLocal(int0));
		LuaStats::GetInstance()->ReportTime(name, start);
    }

    printf("SignAthlete instance %p, franchise id %d\n", instance, int0);

    lua_pushinteger(lua, (int)result);
    
    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 1) : 1;
}

int ReleaseAthlete(lua_State *lua)
{
    static const eastl::string name("ReleaseAthlete");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);
    // the second needs to be our franchise id
    luaL_checktype(lua, -1, LUA_TNUMBER);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);
    int int0 = (int)lua_tointeger(lua, -1);

    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());
    BlazeRpcError result = ERR_OK;
    if (instance)
    {
		TimeValue start(TimeValue::getTimeOfDay());
        result = LuaStats::GetInstance()->ReportError(name.c_str(), instance->releaseAthleteLocal(int0));
		LuaStats::GetInstance()->ReportTime(name, start);
    }

    printf("ReleaseAthlete instance %p, franchise id %d\n", instance, int0);

    lua_pushinteger(lua, (int)result);
    
    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 1) : 1;
}

int SubmitFranchiseGameStats(lua_State *lua)
{
    static const eastl::string name("SubmitFranchiseGameStats");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);
    // the second needs to be our franchise id
    luaL_checktype(lua, -1, LUA_TNUMBER);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);
    int int0 = (int)lua_tointeger(lua, -1);

    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());
    BlazeRpcError result = ERR_OK;
    if (instance)
    {
		TimeValue start(TimeValue::getTimeOfDay());
        result = LuaStats::GetInstance()->ReportError(name.c_str(), instance->submitFranchiseGameStatsLocal(int0));
		LuaStats::GetInstance()->ReportTime(name, start);
    }

    printf("SubmitFranchiseGameStats instance %p, franchise id %d\n", instance, int0);

    lua_pushinteger(lua, (int)result);
    
    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 1) : 1;
}

int SubmitScoringSummary(lua_State *lua)
{
    static const eastl::string name("SubmitScoringSummary");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);
    // the second needs to be our franchise id
    luaL_checktype(lua, -1, LUA_TNUMBER);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);
    int int0 = (int)lua_tointeger(lua, -1);

    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());
    BlazeRpcError result = ERR_OK;
    if (instance)
    {
		TimeValue start(TimeValue::getTimeOfDay());
        result = LuaStats::GetInstance()->ReportError(name.c_str(), instance->submitScoringSummaryLocal(int0));
		LuaStats::GetInstance()->ReportTime(name, start);
    }

    printf("SubmitScoringSummary instance %p, franchise id %d\n", instance, int0);

    lua_pushinteger(lua, (int)result);
    
    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 1) : 1;
}

int SubmitAthleteGameStats(lua_State *lua)
{
    static const eastl::string name("SubmitAthleteGameStats");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);
    // the second needs to be our franchise id
    luaL_checktype(lua, -1, LUA_TNUMBER);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);
    int int0 = (int)lua_tointeger(lua, -1);

    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());
    BlazeRpcError result = ERR_OK;
    if (instance)
    {
		TimeValue start(TimeValue::getTimeOfDay());
        result = LuaStats::GetInstance()->ReportError(name.c_str(), instance->submitAthleteGameStatsLocal(int0));
		LuaStats::GetInstance()->ReportTime(name, start);
    }

    printf("SubmitAthleteGameStats instance %p, franchise id %d\n", instance, int0);

    lua_pushinteger(lua, (int)result);
    
    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 1) : 1;
}

int SubmitTeamGameStats(lua_State *lua)
{
    static const eastl::string name("SubmitTeamGameStats");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);
    // the second needs to be our franchise id
    luaL_checktype(lua, -1, LUA_TNUMBER);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);
    int int0 = (int)lua_tointeger(lua, -1);

    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());
    BlazeRpcError result = ERR_OK;
    if (instance)
    {
		TimeValue start(TimeValue::getTimeOfDay());
        result = LuaStats::GetInstance()->ReportError(name.c_str(), instance->submitTeamGameStatsLocal(int0));
		LuaStats::GetInstance()->ReportTime(name, start);
    }

    printf("SubmitTeamGameStats instance %p, franchise id %d\n", instance, int0);

    lua_pushinteger(lua, (int)result);
    
    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 1) : 1;
}

int GetCpuTrades(lua_State *lua)
{
    static const eastl::string name("GetCpuTrades");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);
    // the second needs to be our franchise id
    luaL_checktype(lua, -1, LUA_TNUMBER);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);
    int int0 = (int)lua_tointeger(lua, -1);

    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());
    BlazeRpcError result = ERR_OK;
    if (instance)
    {
		TimeValue start(TimeValue::getTimeOfDay());
        result = LuaStats::GetInstance()->ReportError(name.c_str(), instance->getCpuTradesLocal(int0));
		LuaStats::GetInstance()->ReportTime(name, start);
    }

    printf("GetCpuTrades instance %p, franchise id %d\n", instance, int0);

    lua_pushinteger(lua, (int)result);
    
    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 1) : 1;
}

int GetDisplayRosterByAthlete(lua_State *lua)
{
    static const eastl::string name("GetDisplayRosterByAthlete");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);
    // the second needs to be our franchise id
    luaL_checktype(lua, -1, LUA_TNUMBER);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);
    int int0 = (int)lua_tointeger(lua, -1);

    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());
    BlazeRpcError result = ERR_OK;
    if (instance)
    {
		TimeValue start(TimeValue::getTimeOfDay());
        result = LuaStats::GetInstance()->ReportError(name.c_str(), instance->getDisplayRosterByAthleteLocal(int0));
		LuaStats::GetInstance()->ReportTime(name, start);
    }

    printf("GetDisplayRosterByAthlete instance %p, franchise id %d\n", instance, int0);

    lua_pushinteger(lua, (int)result);
    
    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 1) : 1;
}

int SubmitAntiCheatInfo(lua_State *lua)
{
    static const eastl::string name("SubmitAntiCheatInfo");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);
    // the second needs to be our franchise id
    luaL_checktype(lua, -1, LUA_TNUMBER);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);
    int int0 = (int)lua_tointeger(lua, -1);

    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());
    BlazeRpcError result = ERR_OK;
    if (instance)
    {
		TimeValue start(TimeValue::getTimeOfDay());
        result = LuaStats::GetInstance()->ReportError(name.c_str(), instance->submitAntiCheatInfoLocal(int0));
		LuaStats::GetInstance()->ReportTime(name, start);
    }

    printf("SubmitAntiCheatInfo instance %p, franchise id %d\n", instance, int0);

    lua_pushinteger(lua, (int)result);
    
    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 1) : 1;
}

int GetGameplayInformation(lua_State *lua)
{
    static const eastl::string name("GetGameplayInformation");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);
    // the second needs to be our franchise id
    luaL_checktype(lua, -1, LUA_TNUMBER);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);
    int int0 = (int)lua_tointeger(lua, -1);

    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());
    BlazeRpcError result = ERR_OK;
    if (instance)
    {
		TimeValue start(TimeValue::getTimeOfDay());
        result = LuaStats::GetInstance()->ReportError(name.c_str(), instance->getGameplayInformationLocal(int0));
		LuaStats::GetInstance()->ReportTime(name, start);
    }

    printf("GetGameplayInformation instance %p, franchise id %d\n", instance, int0);

    lua_pushinteger(lua, (int)result);
    
    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 1) : 1;
}

int SetMemberStatus(lua_State *lua)
{
    static const eastl::string name("SetMemberStatus");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);
    // the second needs to be our franchise id
    luaL_checktype(lua, -1, LUA_TNUMBER);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);
    int int0 = (int)lua_tointeger(lua, -1);

    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());
    BlazeRpcError result = ERR_OK;
    if (instance)
    {
		TimeValue start(TimeValue::getTimeOfDay());
        result = LuaStats::GetInstance()->ReportError(name.c_str(), instance->setMemberStatusLocal(int0));
		LuaStats::GetInstance()->ReportTime(name, start);
    }

    printf("SetMemberStatus instance %p, franchise id %d\n", instance, int0);

    lua_pushinteger(lua, (int)result);
    
    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 1) : 1;
}

int GetRetirements(lua_State *lua)
{
    static const eastl::string name("GetRetirements");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);
    // the second needs to be our franchise id
    luaL_checktype(lua, -1, LUA_TNUMBER);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);
    int int0 = (int)lua_tointeger(lua, -1);

    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());
    BlazeRpcError result = ERR_OK;
    if (instance)
    {
		TimeValue start(TimeValue::getTimeOfDay());
        result = LuaStats::GetInstance()->ReportError(name.c_str(), instance->getRetirementsLocal(int0));
		LuaStats::GetInstance()->ReportTime(name, start);
    }

    printf("GetRetirements instance %p, franchise id %d\n", instance, int0);

    lua_pushinteger(lua, (int)result);
    
    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 1) : 1;
}

int GetGameStatReport(lua_State *lua)
{
    static const eastl::string name("GetGameStatReport");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);
    // the second needs to be our franchise id
    luaL_checktype(lua, -1, LUA_TNUMBER);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);
    int int0 = (int)lua_tointeger(lua, -1);

    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());
    BlazeRpcError result = ERR_OK;
    if (instance)
    {
		TimeValue start(TimeValue::getTimeOfDay());
        result = LuaStats::GetInstance()->ReportError(name.c_str(), instance->getGameStatReportLocal(int0));
		LuaStats::GetInstance()->ReportTime(name, start);
    }

    printf("GetGameStatReport instance %p, franchise id %d\n", instance, int0);

    lua_pushinteger(lua, (int)result);
    
    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 1) : 1;
}

int FindBlazeIdOfSomeoneElse(lua_State *lua)
{
    static const eastl::string name("FindBlazeIdOfSomeoneElse");
    
    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);
    // the second needs to be our franchise id
    luaL_checktype(lua, -1, LUA_TNUMBER);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);
    int int0 = (int)lua_tointeger(lua, -1);

    BlazeId bid = 0;
    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());
    if (instance)
    {
		TimeValue start(TimeValue::getTimeOfDay());
        bid = instance->findBlazeIdOfSomeoneElse(int0);
		LuaStats::GetInstance()->ReportTime(name, start);
    }

    printf("FindBlazeIdOfSomeoneElse instance %p, arg int 0 %d\n", instance, int0);

    lua_pushinteger(lua, (int)bid);
    
    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 1) : 1;
}

int GetBlazeId(lua_State *lua)
{
    static const eastl::string name("GetBlazeId");
    
    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -1, LUA_TTABLE);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);
    BlazeId bid = 0;

    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());
    if (instance)
    {
		TimeValue start(TimeValue::getTimeOfDay());
        bid = instance->getBlazeId();
		LuaStats::GetInstance()->ReportTime(name, start);
    }

    printf("GetBlazeId instance %p\n", instance);

    lua_pushinteger(lua, (int)bid);
    
    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 1) : 1;
}

int GetGMForFranchise(lua_State *lua)
{
    static const eastl::string name("GetGMForFranchise");
    
    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);
    // the second needs to be our franchise id
    luaL_checktype(lua, -1, LUA_TNUMBER);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);
    int int0 = (int)lua_tointeger(lua, -1);

    BlazeId bid = 0;
    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());
    if (instance)
    {
		TimeValue start(TimeValue::getTimeOfDay());
        bid = instance->getGMForFranchise(int0);
		LuaStats::GetInstance()->ReportTime(name, start);
    }

    printf("GetGMForFranchise instance %p, arg int 0 %d\n", instance, int0);

    lua_pushinteger(lua, (int)bid);
    
    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 1) : 1;
}

int FindNonGMMember(lua_State *lua)
{
    static const eastl::string name("FindNonGMMember");
    
    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);
    // the second needs to be our franchise id
    luaL_checktype(lua, -1, LUA_TNUMBER);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);
    int int0 = (int)lua_tointeger(lua, -1);

    BlazeId bid = 0;
    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());
    if (instance)
    {
		TimeValue start(TimeValue::getTimeOfDay());
        bid = instance->findNonGMMember(int0);
		LuaStats::GetInstance()->ReportTime(name, start);
    }

    printf("FindNonGMMember instance %p, arg int 0 %d\n", instance, int0);

    lua_pushinteger(lua, (int)bid);
    
    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 1) : 1;
}

int FindBannedMember(lua_State *lua)
{
    static const eastl::string name("FindBannedMember");
    
    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);
    // the second needs to be our franchise id
    luaL_checktype(lua, -1, LUA_TNUMBER);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);
    int int0 = (int)lua_tointeger(lua, -1);

    BlazeId bid = 0;
    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());
    if (instance)
    {
		TimeValue start(TimeValue::getTimeOfDay());
        bid = instance->findBannedMember(int0);
		LuaStats::GetInstance()->ReportTime(name, start);
    }

    printf("FindBannedMember instance %p, arg int 0 %d\n", instance, int0);

    lua_pushinteger(lua, (int)bid);
    
    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 1) : 1;
}

int ClearMemberDataForFranchise(lua_State *lua)
{
    static const eastl::string name("ClearMemberDataForFranchise");
    
    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);

    // the second needs to be our franchise id
    luaL_checktype(lua, -1, LUA_TNUMBER);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);
    int int0 = (int)lua_tointeger(lua, -1);

    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());
    if (instance)
    {
		TimeValue start(TimeValue::getTimeOfDay());
        instance->clearMemberDataForFranchise(int0);
		LuaStats::GetInstance()->ReportTime(name, start);
    }

    printf("ClearMemberDataForFranchise instance %p, arg int 0 %d\n", instance, int0);
    
    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 0) : 0;
}

int GetRandomFranchiseId(lua_State *lua)
{
    static const eastl::string name("GetRandomFranchiseId");
    
    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -1, LUA_TTABLE);
    
    LuaThread *thread = LuaThread::GetInstanceArgument(lua);
        
    int result = 0;
    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());
    if (instance)
    {
		TimeValue start(TimeValue::getTimeOfDay());
        result = instance->getRandomFranchiseId();
		LuaStats::GetInstance()->ReportTime(name, start);
    }
        
    printf("GetRandomFranchiseId instance %p\n", instance);
    
    lua_pushinteger(lua, (int)result);
   
    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 1) : 1;
}

int GetFranchiseWeCreated(lua_State *lua)
{
    static const eastl::string name("GetFranchiseWeCreated");
    
    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -1, LUA_TTABLE);
    
    LuaThread *thread = LuaThread::GetInstanceArgument(lua);
        
    int result = 0;
    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());
    if (instance)
    {
		TimeValue start(TimeValue::getTimeOfDay());
        result = instance->getFranchiseWeCreated();
		LuaStats::GetInstance()->ReportTime(name, start);
    }
        
    printf("GetFranchiseWeCreated instance %p\n", instance);
    
    lua_pushinteger(lua, (int)result);
   
    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 1) : 1;
}

int GetFranchiseWeAreNotAMemberOf(lua_State *lua)
{
    static const eastl::string name("GetFranchiseWeAreNotAMemberOf");
    
    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -1, LUA_TTABLE);
    
    LuaThread *thread = LuaThread::GetInstanceArgument(lua);
        
    int result = 0;
    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());
    if (instance)
    {
		TimeValue start(TimeValue::getTimeOfDay());
        result = instance->getFranchiseWeAreNotAMemberOf();
		LuaStats::GetInstance()->ReportTime(name, start);
    }
        
    printf("GetFranchiseWeAreNotAMemberOf instance %p\n", instance);
    
    lua_pushinteger(lua, (int)result);
   
    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 1) : 1;
}

int GetGMFranchiseIdForUser(lua_State *lua)
{
    static const eastl::string name("GetGMFranchiseIdForUser");
    
    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -1, LUA_TTABLE);
    
    LuaThread *thread = LuaThread::GetInstanceArgument(lua);
        
    int result = 0;
    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());
    if (instance)
    {
		TimeValue start(TimeValue::getTimeOfDay());
        result = instance->getGMFranchiseIdForUser();
		LuaStats::GetInstance()->ReportTime(name, start);
    }
        
    printf("GetGMFranchiseIdForUser instance %p\n", instance);
    
    lua_pushinteger(lua, (int)result);
   
    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 1) : 1;
}

// BlazeRpcError getFranchise(Franchise::GetFranchiseRequest& req, Franchise::Franchise& resp);
int RPC_getFranchise(lua_State *lua)
{
    static const eastl::string name("RPC_getFranchise");
    
    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);

    // the second needs to be our request
    luaL_checktype(lua, -1, LUA_TTABLE);
    
    LuaThread *thread = LuaThread::GetInstanceArgument(lua);
        
    BlazeRpcError err = ERR_OK;
    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());
    LuaTable luaResponse = 0;
    if (instance)
    {
        Franchise::GetFranchiseRequest request;
        Franchise::Franchise           response;

        lua_pushvalue(lua, -1);
        LuaTable luaRequest = (LuaTable)luaL_ref(lua, LUA_REGISTRYINDEX);

        ConvertFromLua_GetFranchiseRequest(thread, luaRequest, request);

		TimeValue start(TimeValue::getTimeOfDay());
        LuaStats::GetInstance()->ReportError(name, err = instance->getFranchise(request, response));
		LuaStats::GetInstance()->ReportTime (name, start);

        luaResponse = ConvertToLua_Franchise(thread, response);
    }
        
    printf("%s\n", name.c_str());
    
    lua_pushinteger(lua, (int)err);
    thread->PrintStackSize();
    lua_rawgeti(lua, LUA_REGISTRYINDEX, (int)luaResponse);
    thread->PrintStackSize();
    luaL_unref(lua, LUA_REGISTRYINDEX, (int)luaResponse);
    thread->PrintStackSize();
   
    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 2) : 2;
}

// BlazeRpcError createFranchise(Franchise::CreateFranchiseRequest& req, Franchise::CreateFranchiseResponse& resp);
int RPC_createFranchise(lua_State *lua)
{
    static const eastl::string name("RPC_createFranchise");
    
    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);

    // the second needs to be our request
    luaL_checktype(lua, -1, LUA_TTABLE);
    
    LuaThread *thread = LuaThread::GetInstanceArgument(lua);
        
    BlazeRpcError err = ERR_OK;
    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());
    LuaTable luaResponse = 0;
    if (instance)
    {
        Franchise::CreateFranchiseRequest  request;
        Franchise::CreateFranchiseResponse response;

        lua_pushvalue(lua, -1);
        LuaTable luaRequest = (LuaTable)luaL_ref(lua, LUA_REGISTRYINDEX);

        ConvertFromLua_CreateFranchiseRequest(thread, luaRequest, request);

		TimeValue start(TimeValue::getTimeOfDay());
        // $TODO this is not in the new version of FranchiseModule (as of 6/18)
        //LuaStats::GetInstance()->ReportError(name, err = instance->createFranchise(request, response));
		LuaStats::GetInstance()->ReportTime (name, start);

        luaResponse = ConvertToLua_CreateFranchiseResponse(thread, response);
    }
        
    printf("%s\n", name.c_str());
    
    lua_pushinteger(lua, (int)err);
    thread->PrintStackSize();
    lua_rawgeti(lua, LUA_REGISTRYINDEX, (int)luaResponse);
    thread->PrintStackSize();
    luaL_unref(lua, LUA_REGISTRYINDEX, (int)luaResponse);
    thread->PrintStackSize();
   
    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 2) : 2;
}

// BlazeRpcError getMembers(Franchise::GetMembersRequest& req, Franchise::GetMembersResponse& resp);
int RPC_getMembers(lua_State *lua)
{
    static const eastl::string name("RPC_getMembers");
    
    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);

    // the second needs to be our request
    luaL_checktype(lua, -1, LUA_TTABLE);
    
    LuaThread *thread = LuaThread::GetInstanceArgument(lua);
        
    BlazeRpcError err = ERR_OK;
    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());
    LuaTable luaResponse = 0;
    if (instance)
    {
        Franchise::GetMembersRequest  request;
        Franchise::GetMembersResponse response;

        lua_pushvalue(lua, -1);
        LuaTable luaRequest = (LuaTable)luaL_ref(lua, LUA_REGISTRYINDEX);

        ConvertFromLua_GetMembersRequest(thread, luaRequest, request);

		TimeValue start(TimeValue::getTimeOfDay());
        // $TODO this is not in the new version of FranchiseModule (as of 6/18)
        //LuaStats::GetInstance()->ReportError(name, err = instance->getMembers(request, response));
		LuaStats::GetInstance()->ReportTime (name, start);

        luaResponse = ConvertToLua_GetMembersResponse(thread, response);
    }
        
    printf("%s\n", name.c_str());
    
    lua_pushinteger(lua, (int)err);
    thread->PrintStackSize();
    lua_rawgeti(lua, LUA_REGISTRYINDEX, (int)luaResponse);
    thread->PrintStackSize();
    luaL_unref(lua, LUA_REGISTRYINDEX, (int)luaResponse);
    thread->PrintStackSize();
   
    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 2) : 2;
}

// BlazeRpcError getRegularSchedule(Franchise::GetRegularScheduleRequest& req, Franchise::GetRegularScheduleResponse& resp);
int RPC_getRegularSchedule(lua_State *lua)
{
    static const eastl::string name("RPC_getRegularSchedule");
    
    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);

    // the second needs to be our request
    luaL_checktype(lua, -1, LUA_TTABLE);
    
    LuaThread *thread = LuaThread::GetInstanceArgument(lua);
        
    BlazeRpcError err = ERR_OK;
    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());
    LuaTable luaResponse = 0;
    if (instance)
    {
        Franchise::GetRegularScheduleRequest  request;
        Franchise::GetRegularScheduleResponse response;

        lua_pushvalue(lua, -1);
        LuaTable luaRequest = (LuaTable)luaL_ref(lua, LUA_REGISTRYINDEX);

        ConvertFromLua_GetRegularScheduleRequest(thread, luaRequest, request);

		TimeValue start(TimeValue::getTimeOfDay());
        LuaStats::GetInstance()->ReportError(name, err = instance->getRegularSchedule(request, response));
		LuaStats::GetInstance()->ReportTime (name, start);

        luaResponse = ConvertToLua_GetRegularScheduleResponse(thread, response);
    }
        
    printf("%s\n", name.c_str());
    
    lua_pushinteger(lua, (int)err);
    thread->PrintStackSize();
    lua_rawgeti(lua, LUA_REGISTRYINDEX, (int)luaResponse);
    thread->PrintStackSize();
    luaL_unref(lua, LUA_REGISTRYINDEX, (int)luaResponse);
    thread->PrintStackSize();
   
    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 2) : 2;
}

// BlazeRpcError getPlayoffSchedule(Franchise::GetPlayoffScheduleRequest& req, Franchise::GetPlayoffScheduleResponse& resp);
int RPC_getPlayoffSchedule(lua_State *lua)
{
    static const eastl::string name("RPC_getPlayoffSchedule");
    
    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);

    // the second needs to be our request
    luaL_checktype(lua, -1, LUA_TTABLE);
    
    LuaThread *thread = LuaThread::GetInstanceArgument(lua);
        
    BlazeRpcError err = ERR_OK;
    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());
    LuaTable luaResponse = 0;
    if (instance)
    {
        Franchise::GetPlayoffScheduleRequest  request;
        Franchise::GetPlayoffScheduleResponse response;

        lua_pushvalue(lua, -1);
        LuaTable luaRequest = (LuaTable)luaL_ref(lua, LUA_REGISTRYINDEX);

        ConvertFromLua_GetPlayoffScheduleRequest(thread, luaRequest, request);

		TimeValue start(TimeValue::getTimeOfDay());
        // $TODO this is not in the new version of FranchiseModule (as of 6/18)
        //LuaStats::GetInstance()->ReportError(name, err = instance->getPlayoffSchedule(request, response));
		LuaStats::GetInstance()->ReportTime (name, start);

        luaResponse = ConvertToLua_GetPlayoffScheduleResponse(thread, response);
    }
        
    printf("%s\n", name.c_str());
    
    lua_pushinteger(lua, (int)err);
    thread->PrintStackSize();
    lua_rawgeti(lua, LUA_REGISTRYINDEX, (int)luaResponse);
    thread->PrintStackSize();
    luaL_unref(lua, LUA_REGISTRYINDEX, (int)luaResponse);
    thread->PrintStackSize();
   
    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 2) : 2;
}

} // Lua

void RegisterLuaFunctions_OLD(lua_State *lua)
{
    lua_register(lua, "RPC_getFranchise",        Lua::RPC_getFranchise);
    lua_register(lua, "RPC_createFranchise",     Lua::RPC_createFranchise);
    lua_register(lua, "RPC_getRegularSchedule",  Lua::RPC_getRegularSchedule);
    lua_register(lua, "RPC_getPlayoffSchedule",  Lua::RPC_getPlayoffSchedule);
    lua_register(lua, "RPC_getMembers",          Lua::RPC_getMembers);

    lua_register(lua, "RpcOk",                   Lua::RpcOk);
    lua_register(lua, "RpcFranchiseNameInUse",   Lua::RpcFranchiseNameInUse);
    
    lua_register(lua, "GetLastRpcError",         Lua::GetLastRpcError);
    lua_register(lua, "SetYieldAll",             Lua::SetYieldAll);
    // USER DEFINED FUNCTIONS
    lua_register(lua, "CreateFranchise", Lua::CreateFranchise);
    lua_register(lua, "JoinFranchise", Lua::JoinFranchise);
    lua_register(lua, "DeleteFranchise", Lua::DeleteFranchise);
    lua_register(lua, "GetFranchise", Lua::GetFranchise);
    lua_register(lua, "UpdateFranchiseSettings", Lua::UpdateFranchiseSettings);
    lua_register(lua, "GetMemberByBlazeId", Lua::GetMemberByBlazeId);
    lua_register(lua, "GetMemberByTeamId", Lua::GetMemberByTeamId);
    lua_register(lua, "RemoveMember", Lua::RemoveMember);
    lua_register(lua, "GetMembers", Lua::GetMembers);
    lua_register(lua, "PromoteToGM", Lua::PromoteToGM);
    lua_register(lua, "GetRegularSchedule", Lua::GetRegularSchedule);
    lua_register(lua, "GetPlayoffSchedule", Lua::GetPlayoffSchedule);
    lua_register(lua, "GetDisplayRoster", Lua::GetDisplayRoster);
    lua_register(lua, "GetGameplayRoster", Lua::GetGameplayRoster);
    lua_register(lua, "GetNumTeamInjuries", Lua::GetNumTeamInjuries);
    lua_register(lua, "GetInjuriesByTeam", Lua::GetInjuriesByTeam);
    lua_register(lua, "GetGameBoxScoreByGame", Lua::GetGameBoxScoreByGame);
    lua_register(lua, "GetGameBoxScoresByWeek", Lua::GetGameBoxScoresByWeek);
    lua_register(lua, "GetGameBoxScoresByTeam", Lua::GetGameBoxScoresByTeam);
    lua_register(lua, "GetScoringSummary", Lua::GetScoringSummary);
    lua_register(lua, "GetTeamStatsByGame", Lua::GetTeamStatsByGame);
    lua_register(lua, "GetAthleteStatsByGame", Lua::GetAthleteStatsByGame);
    lua_register(lua, "ClearBan", Lua::ClearBan);
    lua_register(lua, "GetSuperbowls", Lua::GetSuperbowls);
    lua_register(lua, "GetDisplayAthlete", Lua::GetDisplayAthlete);
    lua_register(lua, "GetRawData", Lua::GetRawData);
    lua_register(lua, "GetDepthChart", Lua::GetDepthChart);
    lua_register(lua, "GetNews", Lua::GetNews);
    lua_register(lua, "GetInvitations", Lua::GetInvitations);
    lua_register(lua, "GetDraftProfile", Lua::GetDraftProfile);
    lua_register(lua, "GetDraftHistory", Lua::GetDraftHistory);
    lua_register(lua, "GetDraftableAthletes", Lua::GetDraftableAthletes);
    lua_register(lua, "GetDraftOrder", Lua::GetDraftOrder);
    lua_register(lua, "GetTopFranchises", Lua::GetTopFranchises);
    lua_register(lua, "GetFranchiseRank", Lua::GetFranchiseRank);
    lua_register(lua, "ValidateRosters", Lua::ValidateRosters);
    lua_register(lua, "ValidateDepthCharts", Lua::ValidateDepthCharts);
    lua_register(lua, "SetDraftProfile", Lua::SetDraftProfile);
    lua_register(lua, "PostNews", Lua::PostNews);
    lua_register(lua, "SendInvitation", Lua::SendInvitation);
    lua_register(lua, "ProcessInvitation", Lua::ProcessInvitation);
    lua_register(lua, "JoinDraft", Lua::JoinDraft);
    lua_register(lua, "LeaveDraft", Lua::LeaveDraft);
    lua_register(lua, "BeginDraft", Lua::BeginDraft);
    lua_register(lua, "PauseDraft", Lua::PauseDraft);
    lua_register(lua, "DraftAthlete", Lua::DraftAthlete);
    lua_register(lua, "SendDraftChat", Lua::SendDraftChat);
    lua_register(lua, "PollDraft", Lua::PollDraft);
    lua_register(lua, "CheckDraft", Lua::CheckDraft);
    lua_register(lua, "SetDepthChart", Lua::SetDepthChart);
    lua_register(lua, "GenerateDepthChart", Lua::GenerateDepthChart);
    lua_register(lua, "SimSeason", Lua::SimSeason);
    lua_register(lua, "AthleteProgress", Lua::AthleteProgress);
    lua_register(lua, "SignAthlete", Lua::SignAthlete);
    lua_register(lua, "ReleaseAthlete", Lua::ReleaseAthlete);
    lua_register(lua, "SubmitFranchiseGameStats", Lua::SubmitFranchiseGameStats);
    lua_register(lua, "SubmitScoringSummary", Lua::SubmitScoringSummary);
    lua_register(lua, "SubmitAthleteGameStats", Lua::SubmitAthleteGameStats);
    lua_register(lua, "SubmitTeamGameStats", Lua::SubmitTeamGameStats);
    lua_register(lua, "GetCpuTrades", Lua::GetCpuTrades);
    lua_register(lua, "GetDisplayRosterByAthlete", Lua::GetDisplayRosterByAthlete);
    lua_register(lua, "SubmitAntiCheatInfo", Lua::SubmitAntiCheatInfo);
    lua_register(lua, "GetGameplayInformation", Lua::GetGameplayInformation);
    lua_register(lua, "SetMemberStatus", Lua::SetMemberStatus);
    lua_register(lua, "GetRetirements", Lua::GetRetirements);
    lua_register(lua, "GetGameStatReport", Lua::GetGameStatReport);
    lua_register(lua, "FindBlazeIdOfSomeoneElse", Lua::FindBlazeIdOfSomeoneElse);
    lua_register(lua, "GetBlazeId", Lua::GetBlazeId);
    lua_register(lua, "GetGMForFranchise", Lua::GetGMForFranchise);
    lua_register(lua, "FindNonGMMember", Lua::FindNonGMMember);
    lua_register(lua, "FindBannedMember", Lua::FindBannedMember);
    lua_register(lua, "ClearMemberDataForFranchise", Lua::ClearMemberDataForFranchise);
    lua_register(lua, "GetRandomFranchiseId", Lua::GetRandomFranchiseId);
    lua_register(lua, "GetFranchiseWeCreated", Lua::GetFranchiseWeCreated);
    lua_register(lua, "GetFranchiseWeAreNotAMemberOf", Lua::GetFranchiseWeAreNotAMemberOf);
    lua_register(lua, "GetGMFranchiseIdForUser", Lua::GetGMFranchiseIdForUser);
}

} // Stress
} // Blaze
