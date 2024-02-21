/******************************************************************************/
/*!

    Copyright 2009 Electronic Arts Inc.

    \file       franchise.cpp
    \brief      DO NOT EDIT - GENERATED FILE!
                 Implementation file for Lua RPC stress test bindings.
*/
/******************************************************************************/

/*** Includes *****************************************************************/

#include "framework/blaze.h"
#include "franchise.h"
#include "blazerpcerrors.h"
#include "franchisemodule.h"
#include "franchise/rpc/franchiseslave_proxy.h"
#include "franchise/tdf/franchise.h"
#include "franchisestress.h"

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

// BlazeRpcError getDraftableAthletes(Blaze::Franchise::GetDraftableAthletesRequest &request, Blaze::Franchise::GetDraftableAthletesResponse &response)
int LuaRpc_getDraftableAthletes(lua_State *lua)
{
    static const eastl::string name("getDraftableAthletes");

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
        Blaze::Franchise::GetDraftableAthletesRequest request;
        Blaze::Franchise::GetDraftableAthletesResponse response;

        lua_pushvalue(lua, -1);
        LuaTable luaRequest = (LuaTable)luaL_ref(lua, LUA_REGISTRYINDEX);

        ConvertFromLua_GetDraftableAthletesRequest(thread, luaRequest, request);
        TimeValue start(TimeValue::getTimeOfDay());

        LuaStats::GetInstance()->ReportError(name, err = instance->getProxy()->getDraftableAthletes(&request, &response));
        LuaStats::GetInstance()->ReportTime (name, start);
        luaResponse = ConvertToLua_GetDraftableAthletesResponse(thread, response);
    }

    printf("%s\n", name.c_str());

    // push the error
    lua_pushinteger(lua, (int)err);

    // then the response structure (table in Lua)
    lua_rawgeti(lua, LUA_REGISTRYINDEX, (int)luaResponse);
    luaL_unref(lua, LUA_REGISTRYINDEX, (int)luaResponse);

    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 2) : 2;
}


// BlazeRpcError getDraftProfile(Blaze::Franchise::GetDraftProfileRequest &request, Blaze::Franchise::DraftProfile &response)
int LuaRpc_getDraftProfile(lua_State *lua)
{
    static const eastl::string name("getDraftProfile");

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
        Blaze::Franchise::GetDraftProfileRequest request;
        Blaze::Franchise::DraftProfile response;

        lua_pushvalue(lua, -1);
        LuaTable luaRequest = (LuaTable)luaL_ref(lua, LUA_REGISTRYINDEX);

        ConvertFromLua_GetDraftProfileRequest(thread, luaRequest, request);
        TimeValue start(TimeValue::getTimeOfDay());

        LuaStats::GetInstance()->ReportError(name, err = instance->getProxy()->getDraftProfile(&request, &response));
        LuaStats::GetInstance()->ReportTime (name, start);
        luaResponse = ConvertToLua_DraftProfile(thread, response);
    }

    printf("%s\n", name.c_str());

    // push the error
    lua_pushinteger(lua, (int)err);

    // then the response structure (table in Lua)
    lua_rawgeti(lua, LUA_REGISTRYINDEX, (int)luaResponse);
    luaL_unref(lua, LUA_REGISTRYINDEX, (int)luaResponse);

    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 2) : 2;
}


// BlazeRpcError setDraftProfile(Blaze::Franchise::SetDraftProfileRequest &request)
int LuaRpc_setDraftProfile(lua_State *lua)
{
    static const eastl::string name("setDraftProfile");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);

    // the second needs to be our request
    luaL_checktype(lua, -1, LUA_TTABLE);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);

    BlazeRpcError err = ERR_OK;
    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());

    if (instance)
    {
        Blaze::Franchise::SetDraftProfileRequest request;
        lua_pushvalue(lua, -1);
        LuaTable luaRequest = (LuaTable)luaL_ref(lua, LUA_REGISTRYINDEX);

        ConvertFromLua_SetDraftProfileRequest(thread, luaRequest, request);
        TimeValue start(TimeValue::getTimeOfDay());

        LuaStats::GetInstance()->ReportError(name, err = instance->getProxy()->setDraftProfile(&request));
        LuaStats::GetInstance()->ReportTime (name, start);
    }

    printf("%s\n", name.c_str());

    // push the error
    lua_pushinteger(lua, (int)err);

    // then the response, which in this case is nil
    lua_pushnil(lua);

    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 2) : 2;
}


// BlazeRpcError createFranchise(Blaze::Franchise::CreateFranchiseRequest &request, Blaze::Franchise::CreateFranchiseResponse &response)
int LuaRpc_createFranchise(lua_State *lua)
{
    static const eastl::string name("createFranchise");

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
        Blaze::Franchise::CreateFranchiseRequest request;
        Blaze::Franchise::CreateFranchiseResponse response;

        lua_pushvalue(lua, -1);
        LuaTable luaRequest = (LuaTable)luaL_ref(lua, LUA_REGISTRYINDEX);

        ConvertFromLua_CreateFranchiseRequest(thread, luaRequest, request);
        TimeValue start(TimeValue::getTimeOfDay());

        LuaStats::GetInstance()->ReportError(name, err = instance->getProxy()->createFranchise(&request, &response));
        LuaStats::GetInstance()->ReportTime (name, start);
        luaResponse = ConvertToLua_CreateFranchiseResponse(thread, response);
    }

    printf("%s\n", name.c_str());

    // push the error
    lua_pushinteger(lua, (int)err);

    // then the response structure (table in Lua)
    lua_rawgeti(lua, LUA_REGISTRYINDEX, (int)luaResponse);
    luaL_unref(lua, LUA_REGISTRYINDEX, (int)luaResponse);

    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 2) : 2;
}


// BlazeRpcError joinFranchise(Blaze::Franchise::JoinFranchiseRequest &request)
int LuaRpc_joinFranchise(lua_State *lua)
{
    static const eastl::string name("joinFranchise");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);

    // the second needs to be our request
    luaL_checktype(lua, -1, LUA_TTABLE);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);

    BlazeRpcError err = ERR_OK;
    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());

    if (instance)
    {
        Blaze::Franchise::JoinFranchiseRequest request;
        lua_pushvalue(lua, -1);
        LuaTable luaRequest = (LuaTable)luaL_ref(lua, LUA_REGISTRYINDEX);

        ConvertFromLua_JoinFranchiseRequest(thread, luaRequest, request);
        TimeValue start(TimeValue::getTimeOfDay());

        LuaStats::GetInstance()->ReportError(name, err = instance->getProxy()->joinFranchise(&request));
        LuaStats::GetInstance()->ReportTime (name, start);
    }

    printf("%s\n", name.c_str());

    // push the error
    lua_pushinteger(lua, (int)err);

    // then the response, which in this case is nil
    lua_pushnil(lua);

    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 2) : 2;
}


// BlazeRpcError getFranchise(Blaze::Franchise::GetFranchiseRequest &request, Blaze::Franchise::Franchise &response)
int LuaRpc_getFranchise(lua_State *lua)
{
    static const eastl::string name("getFranchise");

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
        Blaze::Franchise::GetFranchiseRequest request;
        Blaze::Franchise::Franchise response;

        lua_pushvalue(lua, -1);
        LuaTable luaRequest = (LuaTable)luaL_ref(lua, LUA_REGISTRYINDEX);

        ConvertFromLua_GetFranchiseRequest(thread, luaRequest, request);
        TimeValue start(TimeValue::getTimeOfDay());

        LuaStats::GetInstance()->ReportError(name, err = instance->getProxy()->getFranchise(&request, &response));
        LuaStats::GetInstance()->ReportTime (name, start);
        luaResponse = ConvertToLua_Franchise(thread, response);
    }

    printf("%s\n", name.c_str());

    // push the error
    lua_pushinteger(lua, (int)err);

    // then the response structure (table in Lua)
    lua_rawgeti(lua, LUA_REGISTRYINDEX, (int)luaResponse);
    luaL_unref(lua, LUA_REGISTRYINDEX, (int)luaResponse);

    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 2) : 2;
}


// BlazeRpcError getFranchisesByUser(Blaze::Franchise::FindFranchisesResponse &response)
int LuaRpc_getFranchisesByUser(lua_State *lua)
{
    static const eastl::string name("getFranchisesByUser");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -1, LUA_TTABLE);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);

    BlazeRpcError err = ERR_OK;
    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());

    LuaTable luaResponse = 0;
    if (instance)
    {
        Blaze::Franchise::FindFranchisesResponse response;

        TimeValue start(TimeValue::getTimeOfDay());

        LuaStats::GetInstance()->ReportError(name, err = instance->getProxy()->getFranchisesByUser(&response));
        LuaStats::GetInstance()->ReportTime (name, start);
        luaResponse = ConvertToLua_FindFranchisesResponse(thread, response);
    }

    printf("%s\n", name.c_str());

    // push the error
    lua_pushinteger(lua, (int)err);

    // then the response structure (table in Lua)
    lua_rawgeti(lua, LUA_REGISTRYINDEX, (int)luaResponse);
    luaL_unref(lua, LUA_REGISTRYINDEX, (int)luaResponse);

    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 2) : 2;
}


// BlazeRpcError deleteFranchise(Blaze::Franchise::DeleteFranchiseRequest &request)
int LuaRpc_deleteFranchise(lua_State *lua)
{
    static const eastl::string name("deleteFranchise");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);

    // the second needs to be our request
    luaL_checktype(lua, -1, LUA_TTABLE);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);

    BlazeRpcError err = ERR_OK;
    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());

    if (instance)
    {
        Blaze::Franchise::DeleteFranchiseRequest request;
        lua_pushvalue(lua, -1);
        LuaTable luaRequest = (LuaTable)luaL_ref(lua, LUA_REGISTRYINDEX);

        ConvertFromLua_DeleteFranchiseRequest(thread, luaRequest, request);
        TimeValue start(TimeValue::getTimeOfDay());

        LuaStats::GetInstance()->ReportError(name, err = instance->getProxy()->deleteFranchise(&request));
        LuaStats::GetInstance()->ReportTime (name, start);
    }

    printf("%s\n", name.c_str());

    // push the error
    lua_pushinteger(lua, (int)err);

    // then the response, which in this case is nil
    lua_pushnil(lua);

    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 2) : 2;
}


// BlazeRpcError getMemberByBlazeId(Blaze::Franchise::GetMemberByBlazeIdRequest &request, Blaze::Franchise::MemberInfo &response)
int LuaRpc_getMemberByBlazeId(lua_State *lua)
{
    static const eastl::string name("getMemberByBlazeId");

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
        Blaze::Franchise::GetMemberByBlazeIdRequest request;
        Blaze::Franchise::MemberInfo response;

        lua_pushvalue(lua, -1);
        LuaTable luaRequest = (LuaTable)luaL_ref(lua, LUA_REGISTRYINDEX);

        ConvertFromLua_GetMemberByBlazeIdRequest(thread, luaRequest, request);
        TimeValue start(TimeValue::getTimeOfDay());

        LuaStats::GetInstance()->ReportError(name, err = instance->getProxy()->getMemberByBlazeId(&request, &response));
        LuaStats::GetInstance()->ReportTime (name, start);
        luaResponse = ConvertToLua_MemberInfo(thread, response);
    }

    printf("%s\n", name.c_str());

    // push the error
    lua_pushinteger(lua, (int)err);

    // then the response structure (table in Lua)
    lua_rawgeti(lua, LUA_REGISTRYINDEX, (int)luaResponse);
    luaL_unref(lua, LUA_REGISTRYINDEX, (int)luaResponse);

    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 2) : 2;
}


// BlazeRpcError getMemberByTeamId(Blaze::Franchise::GetMemberByTeamIdRequest &request, Blaze::Franchise::MemberInfo &response)
int LuaRpc_getMemberByTeamId(lua_State *lua)
{
    static const eastl::string name("getMemberByTeamId");

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
        Blaze::Franchise::GetMemberByTeamIdRequest request;
        Blaze::Franchise::MemberInfo response;

        lua_pushvalue(lua, -1);
        LuaTable luaRequest = (LuaTable)luaL_ref(lua, LUA_REGISTRYINDEX);

        ConvertFromLua_GetMemberByTeamIdRequest(thread, luaRequest, request);
        TimeValue start(TimeValue::getTimeOfDay());

        LuaStats::GetInstance()->ReportError(name, err = instance->getProxy()->getMemberByTeamId(&request, &response));
        LuaStats::GetInstance()->ReportTime (name, start);
        luaResponse = ConvertToLua_MemberInfo(thread, response);
    }

    printf("%s\n", name.c_str());

    // push the error
    lua_pushinteger(lua, (int)err);

    // then the response structure (table in Lua)
    lua_rawgeti(lua, LUA_REGISTRYINDEX, (int)luaResponse);
    luaL_unref(lua, LUA_REGISTRYINDEX, (int)luaResponse);

    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 2) : 2;
}


// BlazeRpcError findFranchises(Blaze::Franchise::FindFranchisesRequest &request, Blaze::Franchise::FindFranchisesResponse &response)
int LuaRpc_findFranchises(lua_State *lua)
{
    static const eastl::string name("findFranchises");

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
        Blaze::Franchise::FindFranchisesRequest request;
        Blaze::Franchise::FindFranchisesResponse response;

        lua_pushvalue(lua, -1);
        LuaTable luaRequest = (LuaTable)luaL_ref(lua, LUA_REGISTRYINDEX);

        ConvertFromLua_FindFranchisesRequest(thread, luaRequest, request);
        TimeValue start(TimeValue::getTimeOfDay());

        LuaStats::GetInstance()->ReportError(name, err = instance->getProxy()->findFranchises(&request, &response));
        LuaStats::GetInstance()->ReportTime (name, start);
        luaResponse = ConvertToLua_FindFranchisesResponse(thread, response);
    }

    printf("%s\n", name.c_str());

    // push the error
    lua_pushinteger(lua, (int)err);

    // then the response structure (table in Lua)
    lua_rawgeti(lua, LUA_REGISTRYINDEX, (int)luaResponse);
    luaL_unref(lua, LUA_REGISTRYINDEX, (int)luaResponse);

    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 2) : 2;
}


// BlazeRpcError findFranchisesAsync(Blaze::Franchise::FindFranchisesRequest &request)
int LuaRpc_findFranchisesAsync(lua_State *lua)
{
    static const eastl::string name("findFranchisesAsync");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);

    // the second needs to be our request
    luaL_checktype(lua, -1, LUA_TTABLE);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);

    BlazeRpcError err = ERR_OK;
    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());

    if (instance)
    {
        Blaze::Franchise::FindFranchisesRequest request;
        lua_pushvalue(lua, -1);
        LuaTable luaRequest = (LuaTable)luaL_ref(lua, LUA_REGISTRYINDEX);

        ConvertFromLua_FindFranchisesRequest(thread, luaRequest, request);
        TimeValue start(TimeValue::getTimeOfDay());

        LuaStats::GetInstance()->ReportError(name, err = instance->getProxy()->findFranchisesAsync(&request));
        LuaStats::GetInstance()->ReportTime (name, start);
    }

    printf("%s\n", name.c_str());

    // push the error
    lua_pushinteger(lua, (int)err);

    // then the response, which in this case is nil
    lua_pushnil(lua);

    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 2) : 2;
}


// BlazeRpcError removeMember(Blaze::Franchise::RemoveMemberRequest &request)
int LuaRpc_removeMember(lua_State *lua)
{
    static const eastl::string name("removeMember");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);

    // the second needs to be our request
    luaL_checktype(lua, -1, LUA_TTABLE);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);

    BlazeRpcError err = ERR_OK;
    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());

    if (instance)
    {
        Blaze::Franchise::RemoveMemberRequest request;
        lua_pushvalue(lua, -1);
        LuaTable luaRequest = (LuaTable)luaL_ref(lua, LUA_REGISTRYINDEX);

        ConvertFromLua_RemoveMemberRequest(thread, luaRequest, request);
        TimeValue start(TimeValue::getTimeOfDay());

        LuaStats::GetInstance()->ReportError(name, err = instance->getProxy()->removeMember(&request));
        LuaStats::GetInstance()->ReportTime (name, start);
    }

    printf("%s\n", name.c_str());

    // push the error
    lua_pushinteger(lua, (int)err);

    // then the response, which in this case is nil
    lua_pushnil(lua);

    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 2) : 2;
}


// BlazeRpcError updateFranchiseSettings(Blaze::Franchise::UpdateFranchiseSettingsRequest &request)
int LuaRpc_updateFranchiseSettings(lua_State *lua)
{
    static const eastl::string name("updateFranchiseSettings");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);

    // the second needs to be our request
    luaL_checktype(lua, -1, LUA_TTABLE);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);

    BlazeRpcError err = ERR_OK;
    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());

    if (instance)
    {
        Blaze::Franchise::UpdateFranchiseSettingsRequest request;
        lua_pushvalue(lua, -1);
        LuaTable luaRequest = (LuaTable)luaL_ref(lua, LUA_REGISTRYINDEX);

        ConvertFromLua_UpdateFranchiseSettingsRequest(thread, luaRequest, request);
        TimeValue start(TimeValue::getTimeOfDay());

        LuaStats::GetInstance()->ReportError(name, err = instance->getProxy()->updateFranchiseSettings(&request));
        LuaStats::GetInstance()->ReportTime (name, start);
    }

    printf("%s\n", name.c_str());

    // push the error
    lua_pushinteger(lua, (int)err);

    // then the response, which in this case is nil
    lua_pushnil(lua);

    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 2) : 2;
}


// BlazeRpcError postNews(Blaze::Franchise::PostNewsRequest &request)
int LuaRpc_postNews(lua_State *lua)
{
    static const eastl::string name("postNews");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);

    // the second needs to be our request
    luaL_checktype(lua, -1, LUA_TTABLE);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);

    BlazeRpcError err = ERR_OK;
    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());

    if (instance)
    {
        Blaze::Franchise::PostNewsRequest request;
        lua_pushvalue(lua, -1);
        LuaTable luaRequest = (LuaTable)luaL_ref(lua, LUA_REGISTRYINDEX);

        ConvertFromLua_PostNewsRequest(thread, luaRequest, request);
        TimeValue start(TimeValue::getTimeOfDay());

        LuaStats::GetInstance()->ReportError(name, err = instance->getProxy()->postNews(&request));
        LuaStats::GetInstance()->ReportTime (name, start);
    }

    printf("%s\n", name.c_str());

    // push the error
    lua_pushinteger(lua, (int)err);

    // then the response, which in this case is nil
    lua_pushnil(lua);

    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 2) : 2;
}


// BlazeRpcError getNews(Blaze::Franchise::GetNewsRequest &request, Blaze::Franchise::GetNewsResponse &response)
int LuaRpc_getNews(lua_State *lua)
{
    static const eastl::string name("getNews");

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
        Blaze::Franchise::GetNewsRequest request;
        Blaze::Franchise::GetNewsResponse response;

        lua_pushvalue(lua, -1);
        LuaTable luaRequest = (LuaTable)luaL_ref(lua, LUA_REGISTRYINDEX);

        ConvertFromLua_GetNewsRequest(thread, luaRequest, request);
        TimeValue start(TimeValue::getTimeOfDay());

        LuaStats::GetInstance()->ReportError(name, err = instance->getProxy()->getNews(&request, &response));
        LuaStats::GetInstance()->ReportTime (name, start);
        luaResponse = ConvertToLua_GetNewsResponse(thread, response);
    }

    printf("%s\n", name.c_str());

    // push the error
    lua_pushinteger(lua, (int)err);

    // then the response structure (table in Lua)
    lua_rawgeti(lua, LUA_REGISTRYINDEX, (int)luaResponse);
    luaL_unref(lua, LUA_REGISTRYINDEX, (int)luaResponse);

    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 2) : 2;
}


// BlazeRpcError clearBan(Blaze::Franchise::ClearBanRequest &request)
int LuaRpc_clearBan(lua_State *lua)
{
    static const eastl::string name("clearBan");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);

    // the second needs to be our request
    luaL_checktype(lua, -1, LUA_TTABLE);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);

    BlazeRpcError err = ERR_OK;
    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());

    if (instance)
    {
        Blaze::Franchise::ClearBanRequest request;
        lua_pushvalue(lua, -1);
        LuaTable luaRequest = (LuaTable)luaL_ref(lua, LUA_REGISTRYINDEX);

        ConvertFromLua_ClearBanRequest(thread, luaRequest, request);
        TimeValue start(TimeValue::getTimeOfDay());

        LuaStats::GetInstance()->ReportError(name, err = instance->getProxy()->clearBan(&request));
        LuaStats::GetInstance()->ReportTime (name, start);
    }

    printf("%s\n", name.c_str());

    // push the error
    lua_pushinteger(lua, (int)err);

    // then the response, which in this case is nil
    lua_pushnil(lua);

    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 2) : 2;
}


// BlazeRpcError sendInvitation(Blaze::Franchise::SendInvitationRequest &request)
int LuaRpc_sendInvitation(lua_State *lua)
{
    static const eastl::string name("sendInvitation");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);

    // the second needs to be our request
    luaL_checktype(lua, -1, LUA_TTABLE);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);

    BlazeRpcError err = ERR_OK;
    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());

    if (instance)
    {
        Blaze::Franchise::SendInvitationRequest request;
        lua_pushvalue(lua, -1);
        LuaTable luaRequest = (LuaTable)luaL_ref(lua, LUA_REGISTRYINDEX);

        ConvertFromLua_SendInvitationRequest(thread, luaRequest, request);
        TimeValue start(TimeValue::getTimeOfDay());

        LuaStats::GetInstance()->ReportError(name, err = instance->getProxy()->sendInvitation(&request));
        LuaStats::GetInstance()->ReportTime (name, start);
    }

    printf("%s\n", name.c_str());

    // push the error
    lua_pushinteger(lua, (int)err);

    // then the response, which in this case is nil
    lua_pushnil(lua);

    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 2) : 2;
}


// BlazeRpcError getInvitations(Blaze::Franchise::GetInvitationsRequest &request, Blaze::Franchise::GetInvitationsResponse &response)
int LuaRpc_getInvitations(lua_State *lua)
{
    static const eastl::string name("getInvitations");

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
        Blaze::Franchise::GetInvitationsRequest request;
        Blaze::Franchise::GetInvitationsResponse response;

        lua_pushvalue(lua, -1);
        LuaTable luaRequest = (LuaTable)luaL_ref(lua, LUA_REGISTRYINDEX);

        ConvertFromLua_GetInvitationsRequest(thread, luaRequest, request);
        TimeValue start(TimeValue::getTimeOfDay());

        LuaStats::GetInstance()->ReportError(name, err = instance->getProxy()->getInvitations(&request, &response));
        LuaStats::GetInstance()->ReportTime (name, start);
        luaResponse = ConvertToLua_GetInvitationsResponse(thread, response);
    }

    printf("%s\n", name.c_str());

    // push the error
    lua_pushinteger(lua, (int)err);

    // then the response structure (table in Lua)
    lua_rawgeti(lua, LUA_REGISTRYINDEX, (int)luaResponse);
    luaL_unref(lua, LUA_REGISTRYINDEX, (int)luaResponse);

    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 2) : 2;
}


// BlazeRpcError processInvitation(Blaze::Franchise::ProcessInvitationRequest &request)
int LuaRpc_processInvitation(lua_State *lua)
{
    static const eastl::string name("processInvitation");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);

    // the second needs to be our request
    luaL_checktype(lua, -1, LUA_TTABLE);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);

    BlazeRpcError err = ERR_OK;
    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());

    if (instance)
    {
        Blaze::Franchise::ProcessInvitationRequest request;
        lua_pushvalue(lua, -1);
        LuaTable luaRequest = (LuaTable)luaL_ref(lua, LUA_REGISTRYINDEX);

        ConvertFromLua_ProcessInvitationRequest(thread, luaRequest, request);
        TimeValue start(TimeValue::getTimeOfDay());

        LuaStats::GetInstance()->ReportError(name, err = instance->getProxy()->processInvitation(&request));
        LuaStats::GetInstance()->ReportTime (name, start);
    }

    printf("%s\n", name.c_str());

    // push the error
    lua_pushinteger(lua, (int)err);

    // then the response, which in this case is nil
    lua_pushnil(lua);

    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 2) : 2;
}


// BlazeRpcError proposeTrade(Blaze::Franchise::ProposeTradeRequest &request)
int LuaRpc_proposeTrade(lua_State *lua)
{
    static const eastl::string name("proposeTrade");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);

    // the second needs to be our request
    luaL_checktype(lua, -1, LUA_TTABLE);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);

    BlazeRpcError err = ERR_OK;
    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());

    if (instance)
    {
        Blaze::Franchise::ProposeTradeRequest request;
        lua_pushvalue(lua, -1);
        LuaTable luaRequest = (LuaTable)luaL_ref(lua, LUA_REGISTRYINDEX);

        ConvertFromLua_ProposeTradeRequest(thread, luaRequest, request);
        TimeValue start(TimeValue::getTimeOfDay());

        LuaStats::GetInstance()->ReportError(name, err = instance->getProxy()->proposeTrade(&request));
        LuaStats::GetInstance()->ReportTime (name, start);
    }

    printf("%s\n", name.c_str());

    // push the error
    lua_pushinteger(lua, (int)err);

    // then the response, which in this case is nil
    lua_pushnil(lua);

    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 2) : 2;
}


// BlazeRpcError processTrade(Blaze::Franchise::ProcessTradeRequest &request)
int LuaRpc_processTrade(lua_State *lua)
{
    static const eastl::string name("processTrade");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);

    // the second needs to be our request
    luaL_checktype(lua, -1, LUA_TTABLE);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);

    BlazeRpcError err = ERR_OK;
    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());

    if (instance)
    {
        Blaze::Franchise::ProcessTradeRequest request;
        lua_pushvalue(lua, -1);
        LuaTable luaRequest = (LuaTable)luaL_ref(lua, LUA_REGISTRYINDEX);

        ConvertFromLua_ProcessTradeRequest(thread, luaRequest, request);
        TimeValue start(TimeValue::getTimeOfDay());

        LuaStats::GetInstance()->ReportError(name, err = instance->getProxy()->processTrade(&request));
        LuaStats::GetInstance()->ReportTime (name, start);
    }

    printf("%s\n", name.c_str());

    // push the error
    lua_pushinteger(lua, (int)err);

    // then the response, which in this case is nil
    lua_pushnil(lua);

    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 2) : 2;
}


// BlazeRpcError getTrades(Blaze::Franchise::GetTradesRequest &request, Blaze::Franchise::GetTradesResponse &response)
int LuaRpc_getTrades(lua_State *lua)
{
    static const eastl::string name("getTrades");

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
        Blaze::Franchise::GetTradesRequest request;
        Blaze::Franchise::GetTradesResponse response;

        lua_pushvalue(lua, -1);
        LuaTable luaRequest = (LuaTable)luaL_ref(lua, LUA_REGISTRYINDEX);

        ConvertFromLua_GetTradesRequest(thread, luaRequest, request);
        TimeValue start(TimeValue::getTimeOfDay());

        LuaStats::GetInstance()->ReportError(name, err = instance->getProxy()->getTrades(&request, &response));
        LuaStats::GetInstance()->ReportTime (name, start);
        luaResponse = ConvertToLua_GetTradesResponse(thread, response);
    }

    printf("%s\n", name.c_str());

    // push the error
    lua_pushinteger(lua, (int)err);

    // then the response structure (table in Lua)
    lua_rawgeti(lua, LUA_REGISTRYINDEX, (int)luaResponse);
    luaL_unref(lua, LUA_REGISTRYINDEX, (int)luaResponse);

    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 2) : 2;
}


// BlazeRpcError getMembers(Blaze::Franchise::GetMembersRequest &request, Blaze::Franchise::GetMembersResponse &response)
int LuaRpc_getMembers(lua_State *lua)
{
    static const eastl::string name("getMembers");

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
        Blaze::Franchise::GetMembersRequest request;
        Blaze::Franchise::GetMembersResponse response;

        lua_pushvalue(lua, -1);
        LuaTable luaRequest = (LuaTable)luaL_ref(lua, LUA_REGISTRYINDEX);

        ConvertFromLua_GetMembersRequest(thread, luaRequest, request);
        TimeValue start(TimeValue::getTimeOfDay());

        LuaStats::GetInstance()->ReportError(name, err = instance->getProxy()->getMembers(&request, &response));
        LuaStats::GetInstance()->ReportTime (name, start);
        luaResponse = ConvertToLua_GetMembersResponse(thread, response);
    }

    printf("%s\n", name.c_str());

    // push the error
    lua_pushinteger(lua, (int)err);

    // then the response structure (table in Lua)
    lua_rawgeti(lua, LUA_REGISTRYINDEX, (int)luaResponse);
    luaL_unref(lua, LUA_REGISTRYINDEX, (int)luaResponse);

    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 2) : 2;
}


// BlazeRpcError joinDraft(Blaze::Franchise::JoinDraftRequest &request, Blaze::Franchise::JoinDraftResponse &response)
int LuaRpc_joinDraft(lua_State *lua)
{
    static const eastl::string name("joinDraft");

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
        Blaze::Franchise::JoinDraftRequest request;
        Blaze::Franchise::JoinDraftResponse response;

        lua_pushvalue(lua, -1);
        LuaTable luaRequest = (LuaTable)luaL_ref(lua, LUA_REGISTRYINDEX);

        ConvertFromLua_JoinDraftRequest(thread, luaRequest, request);
        TimeValue start(TimeValue::getTimeOfDay());

        LuaStats::GetInstance()->ReportError(name, err = instance->getProxy()->joinDraft(&request, &response));
        LuaStats::GetInstance()->ReportTime (name, start);
        luaResponse = ConvertToLua_JoinDraftResponse(thread, response);
    }

    printf("%s\n", name.c_str());

    // push the error
    lua_pushinteger(lua, (int)err);

    // then the response structure (table in Lua)
    lua_rawgeti(lua, LUA_REGISTRYINDEX, (int)luaResponse);
    luaL_unref(lua, LUA_REGISTRYINDEX, (int)luaResponse);

    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 2) : 2;
}


// BlazeRpcError leaveDraft(Blaze::Franchise::LeaveDraftRequest &request)
int LuaRpc_leaveDraft(lua_State *lua)
{
    static const eastl::string name("leaveDraft");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);

    // the second needs to be our request
    luaL_checktype(lua, -1, LUA_TTABLE);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);

    BlazeRpcError err = ERR_OK;
    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());

    if (instance)
    {
        Blaze::Franchise::LeaveDraftRequest request;
        lua_pushvalue(lua, -1);
        LuaTable luaRequest = (LuaTable)luaL_ref(lua, LUA_REGISTRYINDEX);

        ConvertFromLua_LeaveDraftRequest(thread, luaRequest, request);
        TimeValue start(TimeValue::getTimeOfDay());

        LuaStats::GetInstance()->ReportError(name, err = instance->getProxy()->leaveDraft(&request));
        LuaStats::GetInstance()->ReportTime (name, start);
    }

    printf("%s\n", name.c_str());

    // push the error
    lua_pushinteger(lua, (int)err);

    // then the response, which in this case is nil
    lua_pushnil(lua);

    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 2) : 2;
}


// BlazeRpcError beginDraft(Blaze::Franchise::BeginDraftRequest &request)
int LuaRpc_beginDraft(lua_State *lua)
{
    static const eastl::string name("beginDraft");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);

    // the second needs to be our request
    luaL_checktype(lua, -1, LUA_TTABLE);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);

    BlazeRpcError err = ERR_OK;
    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());

    if (instance)
    {
        Blaze::Franchise::BeginDraftRequest request;
        lua_pushvalue(lua, -1);
        LuaTable luaRequest = (LuaTable)luaL_ref(lua, LUA_REGISTRYINDEX);

        ConvertFromLua_BeginDraftRequest(thread, luaRequest, request);
        TimeValue start(TimeValue::getTimeOfDay());

        LuaStats::GetInstance()->ReportError(name, err = instance->getProxy()->beginDraft(&request));
        LuaStats::GetInstance()->ReportTime (name, start);
    }

    printf("%s\n", name.c_str());

    // push the error
    lua_pushinteger(lua, (int)err);

    // then the response, which in this case is nil
    lua_pushnil(lua);

    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 2) : 2;
}


// BlazeRpcError pauseDraft(Blaze::Franchise::PauseDraftRequest &request)
int LuaRpc_pauseDraft(lua_State *lua)
{
    static const eastl::string name("pauseDraft");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);

    // the second needs to be our request
    luaL_checktype(lua, -1, LUA_TTABLE);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);

    BlazeRpcError err = ERR_OK;
    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());

    if (instance)
    {
        Blaze::Franchise::PauseDraftRequest request;
        lua_pushvalue(lua, -1);
        LuaTable luaRequest = (LuaTable)luaL_ref(lua, LUA_REGISTRYINDEX);

        ConvertFromLua_PauseDraftRequest(thread, luaRequest, request);
        TimeValue start(TimeValue::getTimeOfDay());

        LuaStats::GetInstance()->ReportError(name, err = instance->getProxy()->pauseDraft(&request));
        LuaStats::GetInstance()->ReportTime (name, start);
    }

    printf("%s\n", name.c_str());

    // push the error
    lua_pushinteger(lua, (int)err);

    // then the response, which in this case is nil
    lua_pushnil(lua);

    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 2) : 2;
}


// BlazeRpcError draftAthlete(Blaze::Franchise::DraftAthleteRequest &request)
int LuaRpc_draftAthlete(lua_State *lua)
{
    static const eastl::string name("draftAthlete");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);

    // the second needs to be our request
    luaL_checktype(lua, -1, LUA_TTABLE);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);

    BlazeRpcError err = ERR_OK;
    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());

    if (instance)
    {
        Blaze::Franchise::DraftAthleteRequest request;
        lua_pushvalue(lua, -1);
        LuaTable luaRequest = (LuaTable)luaL_ref(lua, LUA_REGISTRYINDEX);

        ConvertFromLua_DraftAthleteRequest(thread, luaRequest, request);
        TimeValue start(TimeValue::getTimeOfDay());

        LuaStats::GetInstance()->ReportError(name, err = instance->getProxy()->draftAthlete(&request));
        LuaStats::GetInstance()->ReportTime (name, start);
    }

    printf("%s\n", name.c_str());

    // push the error
    lua_pushinteger(lua, (int)err);

    // then the response, which in this case is nil
    lua_pushnil(lua);

    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 2) : 2;
}


// BlazeRpcError getDraftOrder(Blaze::Franchise::GetDraftOrderRequest &request, Blaze::Franchise::GetDraftOrderResponse &response)
int LuaRpc_getDraftOrder(lua_State *lua)
{
    static const eastl::string name("getDraftOrder");

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
        Blaze::Franchise::GetDraftOrderRequest request;
        Blaze::Franchise::GetDraftOrderResponse response;

        lua_pushvalue(lua, -1);
        LuaTable luaRequest = (LuaTable)luaL_ref(lua, LUA_REGISTRYINDEX);

        ConvertFromLua_GetDraftOrderRequest(thread, luaRequest, request);
        TimeValue start(TimeValue::getTimeOfDay());

        LuaStats::GetInstance()->ReportError(name, err = instance->getProxy()->getDraftOrder(&request, &response));
        LuaStats::GetInstance()->ReportTime (name, start);
        luaResponse = ConvertToLua_GetDraftOrderResponse(thread, response);
    }

    printf("%s\n", name.c_str());

    // push the error
    lua_pushinteger(lua, (int)err);

    // then the response structure (table in Lua)
    lua_rawgeti(lua, LUA_REGISTRYINDEX, (int)luaResponse);
    luaL_unref(lua, LUA_REGISTRYINDEX, (int)luaResponse);

    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 2) : 2;
}


// BlazeRpcError sendDraftChat(Blaze::Franchise::SendDraftChatRequest &request)
int LuaRpc_sendDraftChat(lua_State *lua)
{
    static const eastl::string name("sendDraftChat");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);

    // the second needs to be our request
    luaL_checktype(lua, -1, LUA_TTABLE);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);

    BlazeRpcError err = ERR_OK;
    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());

    if (instance)
    {
        Blaze::Franchise::SendDraftChatRequest request;
        lua_pushvalue(lua, -1);
        LuaTable luaRequest = (LuaTable)luaL_ref(lua, LUA_REGISTRYINDEX);

        ConvertFromLua_SendDraftChatRequest(thread, luaRequest, request);
        TimeValue start(TimeValue::getTimeOfDay());

        LuaStats::GetInstance()->ReportError(name, err = instance->getProxy()->sendDraftChat(&request));
        LuaStats::GetInstance()->ReportTime (name, start);
    }

    printf("%s\n", name.c_str());

    // push the error
    lua_pushinteger(lua, (int)err);

    // then the response, which in this case is nil
    lua_pushnil(lua);

    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 2) : 2;
}


// BlazeRpcError getDraftHistory(Blaze::Franchise::GetDraftHistoryRequest &request, Blaze::Franchise::GetDraftHistoryResponse &response)
int LuaRpc_getDraftHistory(lua_State *lua)
{
    static const eastl::string name("getDraftHistory");

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
        Blaze::Franchise::GetDraftHistoryRequest request;
        Blaze::Franchise::GetDraftHistoryResponse response;

        lua_pushvalue(lua, -1);
        LuaTable luaRequest = (LuaTable)luaL_ref(lua, LUA_REGISTRYINDEX);

        ConvertFromLua_GetDraftHistoryRequest(thread, luaRequest, request);
        TimeValue start(TimeValue::getTimeOfDay());

        LuaStats::GetInstance()->ReportError(name, err = instance->getProxy()->getDraftHistory(&request, &response));
        LuaStats::GetInstance()->ReportTime (name, start);
        luaResponse = ConvertToLua_GetDraftHistoryResponse(thread, response);
    }

    printf("%s\n", name.c_str());

    // push the error
    lua_pushinteger(lua, (int)err);

    // then the response structure (table in Lua)
    lua_rawgeti(lua, LUA_REGISTRYINDEX, (int)luaResponse);
    luaL_unref(lua, LUA_REGISTRYINDEX, (int)luaResponse);

    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 2) : 2;
}


// BlazeRpcError pollDraft(Blaze::Franchise::PollDraftRequest &request, Blaze::Franchise::PollDraftResponse &response)
int LuaRpc_pollDraft(lua_State *lua)
{
    static const eastl::string name("pollDraft");

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
        Blaze::Franchise::PollDraftRequest request;
        Blaze::Franchise::PollDraftResponse response;

        lua_pushvalue(lua, -1);
        LuaTable luaRequest = (LuaTable)luaL_ref(lua, LUA_REGISTRYINDEX);

        ConvertFromLua_PollDraftRequest(thread, luaRequest, request);
        TimeValue start(TimeValue::getTimeOfDay());

        LuaStats::GetInstance()->ReportError(name, err = instance->getProxy()->pollDraft(&request, &response));
        LuaStats::GetInstance()->ReportTime (name, start);
        luaResponse = ConvertToLua_PollDraftResponse(thread, response);
    }

    printf("%s\n", name.c_str());

    // push the error
    lua_pushinteger(lua, (int)err);

    // then the response structure (table in Lua)
    lua_rawgeti(lua, LUA_REGISTRYINDEX, (int)luaResponse);
    luaL_unref(lua, LUA_REGISTRYINDEX, (int)luaResponse);

    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 2) : 2;
}


// BlazeRpcError checkDraft(Blaze::Franchise::CheckDraftRequest &request, Blaze::Franchise::CheckDraftResponse &response)
int LuaRpc_checkDraft(lua_State *lua)
{
    static const eastl::string name("checkDraft");

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
        Blaze::Franchise::CheckDraftRequest request;
        Blaze::Franchise::CheckDraftResponse response;

        lua_pushvalue(lua, -1);
        LuaTable luaRequest = (LuaTable)luaL_ref(lua, LUA_REGISTRYINDEX);

        ConvertFromLua_CheckDraftRequest(thread, luaRequest, request);
        TimeValue start(TimeValue::getTimeOfDay());

        LuaStats::GetInstance()->ReportError(name, err = instance->getProxy()->checkDraft(&request, &response));
        LuaStats::GetInstance()->ReportTime (name, start);
        luaResponse = ConvertToLua_CheckDraftResponse(thread, response);
    }

    printf("%s\n", name.c_str());

    // push the error
    lua_pushinteger(lua, (int)err);

    // then the response structure (table in Lua)
    lua_rawgeti(lua, LUA_REGISTRYINDEX, (int)luaResponse);
    luaL_unref(lua, LUA_REGISTRYINDEX, (int)luaResponse);

    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 2) : 2;
}


// BlazeRpcError getDisplayRoster(Blaze::Franchise::GetDisplayRosterRequest &request, Blaze::Franchise::GetDisplayRosterResponse &response)
int LuaRpc_getDisplayRoster(lua_State *lua)
{
    static const eastl::string name("getDisplayRoster");

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
        Blaze::Franchise::GetDisplayRosterRequest request;
        Blaze::Franchise::GetDisplayRosterResponse response;

        lua_pushvalue(lua, -1);
        LuaTable luaRequest = (LuaTable)luaL_ref(lua, LUA_REGISTRYINDEX);

        ConvertFromLua_GetDisplayRosterRequest(thread, luaRequest, request);
        TimeValue start(TimeValue::getTimeOfDay());

        LuaStats::GetInstance()->ReportError(name, err = instance->getProxy()->getDisplayRoster(&request, &response));
        LuaStats::GetInstance()->ReportTime (name, start);
        luaResponse = ConvertToLua_GetDisplayRosterResponse(thread, response);
    }

    printf("%s\n", name.c_str());

    // push the error
    lua_pushinteger(lua, (int)err);

    // then the response structure (table in Lua)
    lua_rawgeti(lua, LUA_REGISTRYINDEX, (int)luaResponse);
    luaL_unref(lua, LUA_REGISTRYINDEX, (int)luaResponse);

    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 2) : 2;
}


// BlazeRpcError getGameplayRoster(Blaze::Franchise::GetGameplayRosterRequest &request, Blaze::Franchise::GameplayRoster &response)
int LuaRpc_getGameplayRoster(lua_State *lua)
{
    static const eastl::string name("getGameplayRoster");

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
        Blaze::Franchise::GetGameplayRosterRequest request;
        Blaze::Franchise::GameplayRoster response;

        lua_pushvalue(lua, -1);
        LuaTable luaRequest = (LuaTable)luaL_ref(lua, LUA_REGISTRYINDEX);

        ConvertFromLua_GetGameplayRosterRequest(thread, luaRequest, request);
        TimeValue start(TimeValue::getTimeOfDay());

        LuaStats::GetInstance()->ReportError(name, err = instance->getProxy()->getGameplayRoster(&request, &response));
        LuaStats::GetInstance()->ReportTime (name, start);
        luaResponse = ConvertToLua_GameplayRoster(thread, response);
    }

    printf("%s\n", name.c_str());

    // push the error
    lua_pushinteger(lua, (int)err);

    // then the response structure (table in Lua)
    lua_rawgeti(lua, LUA_REGISTRYINDEX, (int)luaResponse);
    luaL_unref(lua, LUA_REGISTRYINDEX, (int)luaResponse);

    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 2) : 2;
}


// BlazeRpcError getDisplayAthlete(Blaze::Franchise::GetDisplayAthleteRequest &request, Blaze::Franchise::GetDisplayAthleteResponse &response)
int LuaRpc_getDisplayAthlete(lua_State *lua)
{
    static const eastl::string name("getDisplayAthlete");

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
        Blaze::Franchise::GetDisplayAthleteRequest request;
        Blaze::Franchise::GetDisplayAthleteResponse response;

        lua_pushvalue(lua, -1);
        LuaTable luaRequest = (LuaTable)luaL_ref(lua, LUA_REGISTRYINDEX);

        ConvertFromLua_GetDisplayAthleteRequest(thread, luaRequest, request);
        TimeValue start(TimeValue::getTimeOfDay());

        LuaStats::GetInstance()->ReportError(name, err = instance->getProxy()->getDisplayAthlete(&request, &response));
        LuaStats::GetInstance()->ReportTime (name, start);
        luaResponse = ConvertToLua_GetDisplayAthleteResponse(thread, response);
    }

    printf("%s\n", name.c_str());

    // push the error
    lua_pushinteger(lua, (int)err);

    // then the response structure (table in Lua)
    lua_rawgeti(lua, LUA_REGISTRYINDEX, (int)luaResponse);
    luaL_unref(lua, LUA_REGISTRYINDEX, (int)luaResponse);

    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 2) : 2;
}


// BlazeRpcError getDepthChart(Blaze::Franchise::GetDepthChartRequest &request, Blaze::Franchise::GetDepthChartResponse &response)
int LuaRpc_getDepthChart(lua_State *lua)
{
    static const eastl::string name("getDepthChart");

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
        Blaze::Franchise::GetDepthChartRequest request;
        Blaze::Franchise::GetDepthChartResponse response;

        lua_pushvalue(lua, -1);
        LuaTable luaRequest = (LuaTable)luaL_ref(lua, LUA_REGISTRYINDEX);

        ConvertFromLua_GetDepthChartRequest(thread, luaRequest, request);
        TimeValue start(TimeValue::getTimeOfDay());

        LuaStats::GetInstance()->ReportError(name, err = instance->getProxy()->getDepthChart(&request, &response));
        LuaStats::GetInstance()->ReportTime (name, start);
        luaResponse = ConvertToLua_GetDepthChartResponse(thread, response);
    }

    printf("%s\n", name.c_str());

    // push the error
    lua_pushinteger(lua, (int)err);

    // then the response structure (table in Lua)
    lua_rawgeti(lua, LUA_REGISTRYINDEX, (int)luaResponse);
    luaL_unref(lua, LUA_REGISTRYINDEX, (int)luaResponse);

    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 2) : 2;
}


// BlazeRpcError setDepthChart(Blaze::Franchise::SetDepthChartRequest &request)
int LuaRpc_setDepthChart(lua_State *lua)
{
    static const eastl::string name("setDepthChart");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);

    // the second needs to be our request
    luaL_checktype(lua, -1, LUA_TTABLE);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);

    BlazeRpcError err = ERR_OK;
    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());

    if (instance)
    {
        Blaze::Franchise::SetDepthChartRequest request;
        lua_pushvalue(lua, -1);
        LuaTable luaRequest = (LuaTable)luaL_ref(lua, LUA_REGISTRYINDEX);

        ConvertFromLua_SetDepthChartRequest(thread, luaRequest, request);
        TimeValue start(TimeValue::getTimeOfDay());

        LuaStats::GetInstance()->ReportError(name, err = instance->getProxy()->setDepthChart(&request));
        LuaStats::GetInstance()->ReportTime (name, start);
    }

    printf("%s\n", name.c_str());

    // push the error
    lua_pushinteger(lua, (int)err);

    // then the response, which in this case is nil
    lua_pushnil(lua);

    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 2) : 2;
}


// BlazeRpcError generateDepthChart(Blaze::Franchise::GenerateDepthChartRequest &request)
int LuaRpc_generateDepthChart(lua_State *lua)
{
    static const eastl::string name("generateDepthChart");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);

    // the second needs to be our request
    luaL_checktype(lua, -1, LUA_TTABLE);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);

    BlazeRpcError err = ERR_OK;
    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());

    if (instance)
    {
        Blaze::Franchise::GenerateDepthChartRequest request;
        lua_pushvalue(lua, -1);
        LuaTable luaRequest = (LuaTable)luaL_ref(lua, LUA_REGISTRYINDEX);

        ConvertFromLua_GenerateDepthChartRequest(thread, luaRequest, request);
        TimeValue start(TimeValue::getTimeOfDay());

        LuaStats::GetInstance()->ReportError(name, err = instance->getProxy()->generateDepthChart(&request));
        LuaStats::GetInstance()->ReportTime (name, start);
    }

    printf("%s\n", name.c_str());

    // push the error
    lua_pushinteger(lua, (int)err);

    // then the response, which in this case is nil
    lua_pushnil(lua);

    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 2) : 2;
}


// BlazeRpcError getRegularSchedule(Blaze::Franchise::GetRegularScheduleRequest &request, Blaze::Franchise::GetRegularScheduleResponse &response)
int LuaRpc_getRegularSchedule(lua_State *lua)
{
    static const eastl::string name("getRegularSchedule");

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
        Blaze::Franchise::GetRegularScheduleRequest request;
        Blaze::Franchise::GetRegularScheduleResponse response;

        lua_pushvalue(lua, -1);
        LuaTable luaRequest = (LuaTable)luaL_ref(lua, LUA_REGISTRYINDEX);

        ConvertFromLua_GetRegularScheduleRequest(thread, luaRequest, request);
        TimeValue start(TimeValue::getTimeOfDay());

        LuaStats::GetInstance()->ReportError(name, err = instance->getProxy()->getRegularSchedule(&request, &response));
        LuaStats::GetInstance()->ReportTime (name, start);
        luaResponse = ConvertToLua_GetRegularScheduleResponse(thread, response);
    }

    printf("%s\n", name.c_str());

    // push the error
    lua_pushinteger(lua, (int)err);

    // then the response structure (table in Lua)
    lua_rawgeti(lua, LUA_REGISTRYINDEX, (int)luaResponse);
    luaL_unref(lua, LUA_REGISTRYINDEX, (int)luaResponse);

    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 2) : 2;
}


// BlazeRpcError getPlayoffSchedule(Blaze::Franchise::GetPlayoffScheduleRequest &request, Blaze::Franchise::GetPlayoffScheduleResponse &response)
int LuaRpc_getPlayoffSchedule(lua_State *lua)
{
    static const eastl::string name("getPlayoffSchedule");

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
        Blaze::Franchise::GetPlayoffScheduleRequest request;
        Blaze::Franchise::GetPlayoffScheduleResponse response;

        lua_pushvalue(lua, -1);
        LuaTable luaRequest = (LuaTable)luaL_ref(lua, LUA_REGISTRYINDEX);

        ConvertFromLua_GetPlayoffScheduleRequest(thread, luaRequest, request);
        TimeValue start(TimeValue::getTimeOfDay());

        LuaStats::GetInstance()->ReportError(name, err = instance->getProxy()->getPlayoffSchedule(&request, &response));
        LuaStats::GetInstance()->ReportTime (name, start);
        luaResponse = ConvertToLua_GetPlayoffScheduleResponse(thread, response);
    }

    printf("%s\n", name.c_str());

    // push the error
    lua_pushinteger(lua, (int)err);

    // then the response structure (table in Lua)
    lua_rawgeti(lua, LUA_REGISTRYINDEX, (int)luaResponse);
    luaL_unref(lua, LUA_REGISTRYINDEX, (int)luaResponse);

    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 2) : 2;
}


// BlazeRpcError getGameplayInformation(Blaze::Franchise::GetGameplayInformationRequest &request, Blaze::Franchise::GetGameplayInformationResponse &response)
int LuaRpc_getGameplayInformation(lua_State *lua)
{
    static const eastl::string name("getGameplayInformation");

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
        Blaze::Franchise::GetGameplayInformationRequest request;
        Blaze::Franchise::GetGameplayInformationResponse response;

        lua_pushvalue(lua, -1);
        LuaTable luaRequest = (LuaTable)luaL_ref(lua, LUA_REGISTRYINDEX);

        ConvertFromLua_GetGameplayInformationRequest(thread, luaRequest, request);
        TimeValue start(TimeValue::getTimeOfDay());

        LuaStats::GetInstance()->ReportError(name, err = instance->getProxy()->getGameplayInformation(&request, &response));
        LuaStats::GetInstance()->ReportTime (name, start);
        luaResponse = ConvertToLua_GetGameplayInformationResponse(thread, response);
    }

    printf("%s\n", name.c_str());

    // push the error
    lua_pushinteger(lua, (int)err);

    // then the response structure (table in Lua)
    lua_rawgeti(lua, LUA_REGISTRYINDEX, (int)luaResponse);
    luaL_unref(lua, LUA_REGISTRYINDEX, (int)luaResponse);

    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 2) : 2;
}


// BlazeRpcError AdvanceFranchise(Blaze::Franchise::AdvanceFranchiseRequest &request)
int LuaRpc_AdvanceFranchise(lua_State *lua)
{
    static const eastl::string name("AdvanceFranchise");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);

    // the second needs to be our request
    luaL_checktype(lua, -1, LUA_TTABLE);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);

    BlazeRpcError err = ERR_OK;
    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());

    if (instance)
    {
        Blaze::Franchise::AdvanceFranchiseRequest request;
        lua_pushvalue(lua, -1);
        LuaTable luaRequest = (LuaTable)luaL_ref(lua, LUA_REGISTRYINDEX);

        ConvertFromLua_AdvanceFranchiseRequest(thread, luaRequest, request);
        TimeValue start(TimeValue::getTimeOfDay());

        LuaStats::GetInstance()->ReportError(name, err = instance->getProxy()->AdvanceFranchise(&request));
        LuaStats::GetInstance()->ReportTime (name, start);
    }

    printf("%s\n", name.c_str());

    // push the error
    lua_pushinteger(lua, (int)err);

    // then the response, which in this case is nil
    lua_pushnil(lua);

    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 2) : 2;
}


// BlazeRpcError SimSeason(Blaze::Franchise::SimSeasonRequest &request)
int LuaRpc_SimSeason(lua_State *lua)
{
    static const eastl::string name("SimSeason");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);

    // the second needs to be our request
    luaL_checktype(lua, -1, LUA_TTABLE);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);

    BlazeRpcError err = ERR_OK;
    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());

    if (instance)
    {
        Blaze::Franchise::SimSeasonRequest request;
        lua_pushvalue(lua, -1);
        LuaTable luaRequest = (LuaTable)luaL_ref(lua, LUA_REGISTRYINDEX);

        ConvertFromLua_SimSeasonRequest(thread, luaRequest, request);
        TimeValue start(TimeValue::getTimeOfDay());

        LuaStats::GetInstance()->ReportError(name, err = instance->getProxy()->SimSeason(&request));
        LuaStats::GetInstance()->ReportTime (name, start);
    }

    printf("%s\n", name.c_str());

    // push the error
    lua_pushinteger(lua, (int)err);

    // then the response, which in this case is nil
    lua_pushnil(lua);

    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 2) : 2;
}


// BlazeRpcError athleteProgress(Blaze::Franchise::AthleteProgressRequest &request)
int LuaRpc_athleteProgress(lua_State *lua)
{
    static const eastl::string name("athleteProgress");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);

    // the second needs to be our request
    luaL_checktype(lua, -1, LUA_TTABLE);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);

    BlazeRpcError err = ERR_OK;
    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());

    if (instance)
    {
        Blaze::Franchise::AthleteProgressRequest request;
        lua_pushvalue(lua, -1);
        LuaTable luaRequest = (LuaTable)luaL_ref(lua, LUA_REGISTRYINDEX);

        ConvertFromLua_AthleteProgressRequest(thread, luaRequest, request);
        TimeValue start(TimeValue::getTimeOfDay());

        LuaStats::GetInstance()->ReportError(name, err = instance->getProxy()->athleteProgress(&request));
        LuaStats::GetInstance()->ReportTime (name, start);
    }

    printf("%s\n", name.c_str());

    // push the error
    lua_pushinteger(lua, (int)err);

    // then the response, which in this case is nil
    lua_pushnil(lua);

    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 2) : 2;
}


// BlazeRpcError getRawData(Blaze::Franchise::GetRawDataRequest &request, Blaze::Franchise::GetRawDataResponse &response)
int LuaRpc_getRawData(lua_State *lua)
{
    static const eastl::string name("getRawData");

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
        Blaze::Franchise::GetRawDataRequest request;
        Blaze::Franchise::GetRawDataResponse response;

        lua_pushvalue(lua, -1);
        LuaTable luaRequest = (LuaTable)luaL_ref(lua, LUA_REGISTRYINDEX);

        ConvertFromLua_GetRawDataRequest(thread, luaRequest, request);
        TimeValue start(TimeValue::getTimeOfDay());

        LuaStats::GetInstance()->ReportError(name, err = instance->getProxy()->getRawData(&request, &response));
        LuaStats::GetInstance()->ReportTime (name, start);
        luaResponse = ConvertToLua_GetRawDataResponse(thread, response);
    }

    printf("%s\n", name.c_str());

    // push the error
    lua_pushinteger(lua, (int)err);

    // then the response structure (table in Lua)
    lua_rawgeti(lua, LUA_REGISTRYINDEX, (int)luaResponse);
    luaL_unref(lua, LUA_REGISTRYINDEX, (int)luaResponse);

    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 2) : 2;
}


// BlazeRpcError signAthlete(Blaze::Franchise::SignAthleteRequest &request)
int LuaRpc_signAthlete(lua_State *lua)
{
    static const eastl::string name("signAthlete");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);

    // the second needs to be our request
    luaL_checktype(lua, -1, LUA_TTABLE);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);

    BlazeRpcError err = ERR_OK;
    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());

    if (instance)
    {
        Blaze::Franchise::SignAthleteRequest request;
        lua_pushvalue(lua, -1);
        LuaTable luaRequest = (LuaTable)luaL_ref(lua, LUA_REGISTRYINDEX);

        ConvertFromLua_SignAthleteRequest(thread, luaRequest, request);
        TimeValue start(TimeValue::getTimeOfDay());

        LuaStats::GetInstance()->ReportError(name, err = instance->getProxy()->signAthlete(&request));
        LuaStats::GetInstance()->ReportTime (name, start);
    }

    printf("%s\n", name.c_str());

    // push the error
    lua_pushinteger(lua, (int)err);

    // then the response, which in this case is nil
    lua_pushnil(lua);

    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 2) : 2;
}


// BlazeRpcError releaseAthlete(Blaze::Franchise::ReleaseAthleteRequest &request)
int LuaRpc_releaseAthlete(lua_State *lua)
{
    static const eastl::string name("releaseAthlete");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);

    // the second needs to be our request
    luaL_checktype(lua, -1, LUA_TTABLE);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);

    BlazeRpcError err = ERR_OK;
    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());

    if (instance)
    {
        Blaze::Franchise::ReleaseAthleteRequest request;
        lua_pushvalue(lua, -1);
        LuaTable luaRequest = (LuaTable)luaL_ref(lua, LUA_REGISTRYINDEX);

        ConvertFromLua_ReleaseAthleteRequest(thread, luaRequest, request);
        TimeValue start(TimeValue::getTimeOfDay());

        LuaStats::GetInstance()->ReportError(name, err = instance->getProxy()->releaseAthlete(&request));
        LuaStats::GetInstance()->ReportTime (name, start);
    }

    printf("%s\n", name.c_str());

    // push the error
    lua_pushinteger(lua, (int)err);

    // then the response, which in this case is nil
    lua_pushnil(lua);

    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 2) : 2;
}


// BlazeRpcError getNumTeamInjuries(Blaze::Franchise::GetNumTeamInjuriesRequest &request, Blaze::Franchise::GetNumTeamInjuriesResponse &response)
int LuaRpc_getNumTeamInjuries(lua_State *lua)
{
    static const eastl::string name("getNumTeamInjuries");

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
        Blaze::Franchise::GetNumTeamInjuriesRequest request;
        Blaze::Franchise::GetNumTeamInjuriesResponse response;

        lua_pushvalue(lua, -1);
        LuaTable luaRequest = (LuaTable)luaL_ref(lua, LUA_REGISTRYINDEX);

        ConvertFromLua_GetNumTeamInjuriesRequest(thread, luaRequest, request);
        TimeValue start(TimeValue::getTimeOfDay());

        LuaStats::GetInstance()->ReportError(name, err = instance->getProxy()->getNumTeamInjuries(&request, &response));
        LuaStats::GetInstance()->ReportTime (name, start);
        luaResponse = ConvertToLua_GetNumTeamInjuriesResponse(thread, response);
    }

    printf("%s\n", name.c_str());

    // push the error
    lua_pushinteger(lua, (int)err);

    // then the response structure (table in Lua)
    lua_rawgeti(lua, LUA_REGISTRYINDEX, (int)luaResponse);
    luaL_unref(lua, LUA_REGISTRYINDEX, (int)luaResponse);

    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 2) : 2;
}


// BlazeRpcError getInjuriesByTeam(Blaze::Franchise::GetInjuriesByTeamRequest &request, Blaze::Franchise::GetInjuriesByTeamResponse &response)
int LuaRpc_getInjuriesByTeam(lua_State *lua)
{
    static const eastl::string name("getInjuriesByTeam");

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
        Blaze::Franchise::GetInjuriesByTeamRequest request;
        Blaze::Franchise::GetInjuriesByTeamResponse response;

        lua_pushvalue(lua, -1);
        LuaTable luaRequest = (LuaTable)luaL_ref(lua, LUA_REGISTRYINDEX);

        ConvertFromLua_GetInjuriesByTeamRequest(thread, luaRequest, request);
        TimeValue start(TimeValue::getTimeOfDay());

        LuaStats::GetInstance()->ReportError(name, err = instance->getProxy()->getInjuriesByTeam(&request, &response));
        LuaStats::GetInstance()->ReportTime (name, start);
        luaResponse = ConvertToLua_GetInjuriesByTeamResponse(thread, response);
    }

    printf("%s\n", name.c_str());

    // push the error
    lua_pushinteger(lua, (int)err);

    // then the response structure (table in Lua)
    lua_rawgeti(lua, LUA_REGISTRYINDEX, (int)luaResponse);
    luaL_unref(lua, LUA_REGISTRYINDEX, (int)luaResponse);

    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 2) : 2;
}


// BlazeRpcError getGameBoxScoreByGame(Blaze::Franchise::GetGameBoxScoreByGameRequest &request, Blaze::Franchise::GameBoxScore &response)
int LuaRpc_getGameBoxScoreByGame(lua_State *lua)
{
    static const eastl::string name("getGameBoxScoreByGame");

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
        Blaze::Franchise::GetGameBoxScoreByGameRequest request;
        Blaze::Franchise::GameBoxScore response;

        lua_pushvalue(lua, -1);
        LuaTable luaRequest = (LuaTable)luaL_ref(lua, LUA_REGISTRYINDEX);

        ConvertFromLua_GetGameBoxScoreByGameRequest(thread, luaRequest, request);
        TimeValue start(TimeValue::getTimeOfDay());

        LuaStats::GetInstance()->ReportError(name, err = instance->getProxy()->getGameBoxScoreByGame(&request, &response));
        LuaStats::GetInstance()->ReportTime (name, start);
        luaResponse = ConvertToLua_GameBoxScore(thread, response);
    }

    printf("%s\n", name.c_str());

    // push the error
    lua_pushinteger(lua, (int)err);

    // then the response structure (table in Lua)
    lua_rawgeti(lua, LUA_REGISTRYINDEX, (int)luaResponse);
    luaL_unref(lua, LUA_REGISTRYINDEX, (int)luaResponse);

    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 2) : 2;
}


// BlazeRpcError getGameBoxScoresByWeek(Blaze::Franchise::GetGameBoxScoresByWeekRequest &request, Blaze::Franchise::GetGameBoxScoresByWeekResponse &response)
int LuaRpc_getGameBoxScoresByWeek(lua_State *lua)
{
    static const eastl::string name("getGameBoxScoresByWeek");

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
        Blaze::Franchise::GetGameBoxScoresByWeekRequest request;
        Blaze::Franchise::GetGameBoxScoresByWeekResponse response;

        lua_pushvalue(lua, -1);
        LuaTable luaRequest = (LuaTable)luaL_ref(lua, LUA_REGISTRYINDEX);

        ConvertFromLua_GetGameBoxScoresByWeekRequest(thread, luaRequest, request);
        TimeValue start(TimeValue::getTimeOfDay());

        LuaStats::GetInstance()->ReportError(name, err = instance->getProxy()->getGameBoxScoresByWeek(&request, &response));
        LuaStats::GetInstance()->ReportTime (name, start);
        luaResponse = ConvertToLua_GetGameBoxScoresByWeekResponse(thread, response);
    }

    printf("%s\n", name.c_str());

    // push the error
    lua_pushinteger(lua, (int)err);

    // then the response structure (table in Lua)
    lua_rawgeti(lua, LUA_REGISTRYINDEX, (int)luaResponse);
    luaL_unref(lua, LUA_REGISTRYINDEX, (int)luaResponse);

    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 2) : 2;
}


// BlazeRpcError getGameBoxScoresByTeam(Blaze::Franchise::GetGameBoxScoresByTeamRequest &request, Blaze::Franchise::GetGameBoxScoresByTeamResponse &response)
int LuaRpc_getGameBoxScoresByTeam(lua_State *lua)
{
    static const eastl::string name("getGameBoxScoresByTeam");

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
        Blaze::Franchise::GetGameBoxScoresByTeamRequest request;
        Blaze::Franchise::GetGameBoxScoresByTeamResponse response;

        lua_pushvalue(lua, -1);
        LuaTable luaRequest = (LuaTable)luaL_ref(lua, LUA_REGISTRYINDEX);

        ConvertFromLua_GetGameBoxScoresByTeamRequest(thread, luaRequest, request);
        TimeValue start(TimeValue::getTimeOfDay());

        LuaStats::GetInstance()->ReportError(name, err = instance->getProxy()->getGameBoxScoresByTeam(&request, &response));
        LuaStats::GetInstance()->ReportTime (name, start);
        luaResponse = ConvertToLua_GetGameBoxScoresByTeamResponse(thread, response);
    }

    printf("%s\n", name.c_str());

    // push the error
    lua_pushinteger(lua, (int)err);

    // then the response structure (table in Lua)
    lua_rawgeti(lua, LUA_REGISTRYINDEX, (int)luaResponse);
    luaL_unref(lua, LUA_REGISTRYINDEX, (int)luaResponse);

    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 2) : 2;
}


// BlazeRpcError getScoringSummary(Blaze::Franchise::GetScoringSummaryRequest &request, Blaze::Franchise::GetScoringSummaryResponse &response)
int LuaRpc_getScoringSummary(lua_State *lua)
{
    static const eastl::string name("getScoringSummary");

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
        Blaze::Franchise::GetScoringSummaryRequest request;
        Blaze::Franchise::GetScoringSummaryResponse response;

        lua_pushvalue(lua, -1);
        LuaTable luaRequest = (LuaTable)luaL_ref(lua, LUA_REGISTRYINDEX);

        ConvertFromLua_GetScoringSummaryRequest(thread, luaRequest, request);
        TimeValue start(TimeValue::getTimeOfDay());

        LuaStats::GetInstance()->ReportError(name, err = instance->getProxy()->getScoringSummary(&request, &response));
        LuaStats::GetInstance()->ReportTime (name, start);
        luaResponse = ConvertToLua_GetScoringSummaryResponse(thread, response);
    }

    printf("%s\n", name.c_str());

    // push the error
    lua_pushinteger(lua, (int)err);

    // then the response structure (table in Lua)
    lua_rawgeti(lua, LUA_REGISTRYINDEX, (int)luaResponse);
    luaL_unref(lua, LUA_REGISTRYINDEX, (int)luaResponse);

    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 2) : 2;
}


// BlazeRpcError getTeamStatsByGame(Blaze::Franchise::GetTeamStatsByGameRequest &request, Blaze::Franchise::GetTeamStatsByGameResponse &response)
int LuaRpc_getTeamStatsByGame(lua_State *lua)
{
    static const eastl::string name("getTeamStatsByGame");

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
        Blaze::Franchise::GetTeamStatsByGameRequest request;
        Blaze::Franchise::GetTeamStatsByGameResponse response;

        lua_pushvalue(lua, -1);
        LuaTable luaRequest = (LuaTable)luaL_ref(lua, LUA_REGISTRYINDEX);

        ConvertFromLua_GetTeamStatsByGameRequest(thread, luaRequest, request);
        TimeValue start(TimeValue::getTimeOfDay());

        LuaStats::GetInstance()->ReportError(name, err = instance->getProxy()->getTeamStatsByGame(&request, &response));
        LuaStats::GetInstance()->ReportTime (name, start);
        luaResponse = ConvertToLua_GetTeamStatsByGameResponse(thread, response);
    }

    printf("%s\n", name.c_str());

    // push the error
    lua_pushinteger(lua, (int)err);

    // then the response structure (table in Lua)
    lua_rawgeti(lua, LUA_REGISTRYINDEX, (int)luaResponse);
    luaL_unref(lua, LUA_REGISTRYINDEX, (int)luaResponse);

    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 2) : 2;
}


// BlazeRpcError getAthleteStatsByGame(Blaze::Franchise::GetAthleteStatsByGameRequest &request, Blaze::Franchise::GetAthleteStatsByGameResponse &response)
int LuaRpc_getAthleteStatsByGame(lua_State *lua)
{
    static const eastl::string name("getAthleteStatsByGame");

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
        Blaze::Franchise::GetAthleteStatsByGameRequest request;
        Blaze::Franchise::GetAthleteStatsByGameResponse response;

        lua_pushvalue(lua, -1);
        LuaTable luaRequest = (LuaTable)luaL_ref(lua, LUA_REGISTRYINDEX);

        ConvertFromLua_GetAthleteStatsByGameRequest(thread, luaRequest, request);
        TimeValue start(TimeValue::getTimeOfDay());

        LuaStats::GetInstance()->ReportError(name, err = instance->getProxy()->getAthleteStatsByGame(&request, &response));
        LuaStats::GetInstance()->ReportTime (name, start);
        luaResponse = ConvertToLua_GetAthleteStatsByGameResponse(thread, response);
    }

    printf("%s\n", name.c_str());

    // push the error
    lua_pushinteger(lua, (int)err);

    // then the response structure (table in Lua)
    lua_rawgeti(lua, LUA_REGISTRYINDEX, (int)luaResponse);
    luaL_unref(lua, LUA_REGISTRYINDEX, (int)luaResponse);

    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 2) : 2;
}


// BlazeRpcError promoteToGM(Blaze::Franchise::PromoteToGMRequest &request)
int LuaRpc_promoteToGM(lua_State *lua)
{
    static const eastl::string name("promoteToGM");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);

    // the second needs to be our request
    luaL_checktype(lua, -1, LUA_TTABLE);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);

    BlazeRpcError err = ERR_OK;
    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());

    if (instance)
    {
        Blaze::Franchise::PromoteToGMRequest request;
        lua_pushvalue(lua, -1);
        LuaTable luaRequest = (LuaTable)luaL_ref(lua, LUA_REGISTRYINDEX);

        ConvertFromLua_PromoteToGMRequest(thread, luaRequest, request);
        TimeValue start(TimeValue::getTimeOfDay());

        LuaStats::GetInstance()->ReportError(name, err = instance->getProxy()->promoteToGM(&request));
        LuaStats::GetInstance()->ReportTime (name, start);
    }

    printf("%s\n", name.c_str());

    // push the error
    lua_pushinteger(lua, (int)err);

    // then the response, which in this case is nil
    lua_pushnil(lua);

    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 2) : 2;
}


// BlazeRpcError getFranchiseRank(Blaze::Franchise::GetFranchiseRankRequest &request, Blaze::Franchise::FranchiseRank &response)
int LuaRpc_getFranchiseRank(lua_State *lua)
{
    static const eastl::string name("getFranchiseRank");

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
        Blaze::Franchise::GetFranchiseRankRequest request;
        Blaze::Franchise::FranchiseRank response;

        lua_pushvalue(lua, -1);
        LuaTable luaRequest = (LuaTable)luaL_ref(lua, LUA_REGISTRYINDEX);

        ConvertFromLua_GetFranchiseRankRequest(thread, luaRequest, request);
        TimeValue start(TimeValue::getTimeOfDay());

        LuaStats::GetInstance()->ReportError(name, err = instance->getProxy()->getFranchiseRank(&request, &response));
        LuaStats::GetInstance()->ReportTime (name, start);
        luaResponse = ConvertToLua_FranchiseRank(thread, response);
    }

    printf("%s\n", name.c_str());

    // push the error
    lua_pushinteger(lua, (int)err);

    // then the response structure (table in Lua)
    lua_rawgeti(lua, LUA_REGISTRYINDEX, (int)luaResponse);
    luaL_unref(lua, LUA_REGISTRYINDEX, (int)luaResponse);

    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 2) : 2;
}


// BlazeRpcError getTopFranchises(Blaze::Franchise::GetTopFranchisesRequest &request, Blaze::Franchise::GetTopFranchisesResponse &response)
int LuaRpc_getTopFranchises(lua_State *lua)
{
    static const eastl::string name("getTopFranchises");

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
        Blaze::Franchise::GetTopFranchisesRequest request;
        Blaze::Franchise::GetTopFranchisesResponse response;

        lua_pushvalue(lua, -1);
        LuaTable luaRequest = (LuaTable)luaL_ref(lua, LUA_REGISTRYINDEX);

        ConvertFromLua_GetTopFranchisesRequest(thread, luaRequest, request);
        TimeValue start(TimeValue::getTimeOfDay());

        LuaStats::GetInstance()->ReportError(name, err = instance->getProxy()->getTopFranchises(&request, &response));
        LuaStats::GetInstance()->ReportTime (name, start);
        luaResponse = ConvertToLua_GetTopFranchisesResponse(thread, response);
    }

    printf("%s\n", name.c_str());

    // push the error
    lua_pushinteger(lua, (int)err);

    // then the response structure (table in Lua)
    lua_rawgeti(lua, LUA_REGISTRYINDEX, (int)luaResponse);
    luaL_unref(lua, LUA_REGISTRYINDEX, (int)luaResponse);

    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 2) : 2;
}


// BlazeRpcError getSuperbowls(Blaze::Franchise::GetSuperbowlsRequest &request, Blaze::Franchise::GetSuperbowlsResponse &response)
int LuaRpc_getSuperbowls(lua_State *lua)
{
    static const eastl::string name("getSuperbowls");

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
        Blaze::Franchise::GetSuperbowlsRequest request;
        Blaze::Franchise::GetSuperbowlsResponse response;

        lua_pushvalue(lua, -1);
        LuaTable luaRequest = (LuaTable)luaL_ref(lua, LUA_REGISTRYINDEX);

        ConvertFromLua_GetSuperbowlsRequest(thread, luaRequest, request);
        TimeValue start(TimeValue::getTimeOfDay());

        LuaStats::GetInstance()->ReportError(name, err = instance->getProxy()->getSuperbowls(&request, &response));
        LuaStats::GetInstance()->ReportTime (name, start);
        luaResponse = ConvertToLua_GetSuperbowlsResponse(thread, response);
    }

    printf("%s\n", name.c_str());

    // push the error
    lua_pushinteger(lua, (int)err);

    // then the response structure (table in Lua)
    lua_rawgeti(lua, LUA_REGISTRYINDEX, (int)luaResponse);
    luaL_unref(lua, LUA_REGISTRYINDEX, (int)luaResponse);

    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 2) : 2;
}


// BlazeRpcError submitFranchiseGameStats(Blaze::Franchise::SubmitFranchiseGameStatsRequest &request)
int LuaRpc_submitFranchiseGameStats(lua_State *lua)
{
    static const eastl::string name("submitFranchiseGameStats");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);

    // the second needs to be our request
    luaL_checktype(lua, -1, LUA_TTABLE);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);

    BlazeRpcError err = ERR_OK;
    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());

    if (instance)
    {
        Blaze::Franchise::SubmitFranchiseGameStatsRequest request;
        lua_pushvalue(lua, -1);
        LuaTable luaRequest = (LuaTable)luaL_ref(lua, LUA_REGISTRYINDEX);

        ConvertFromLua_SubmitFranchiseGameStatsRequest(thread, luaRequest, request);
        TimeValue start(TimeValue::getTimeOfDay());

        LuaStats::GetInstance()->ReportError(name, err = instance->getProxy()->submitFranchiseGameStats(&request));
        LuaStats::GetInstance()->ReportTime (name, start);
    }

    printf("%s\n", name.c_str());

    // push the error
    lua_pushinteger(lua, (int)err);

    // then the response, which in this case is nil
    lua_pushnil(lua);

    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 2) : 2;
}


// BlazeRpcError submitScoringSummary(Blaze::Franchise::SubmitScoringSummaryRequest &request)
int LuaRpc_submitScoringSummary(lua_State *lua)
{
    static const eastl::string name("submitScoringSummary");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);

    // the second needs to be our request
    luaL_checktype(lua, -1, LUA_TTABLE);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);

    BlazeRpcError err = ERR_OK;
    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());

    if (instance)
    {
        Blaze::Franchise::SubmitScoringSummaryRequest request;
        lua_pushvalue(lua, -1);
        LuaTable luaRequest = (LuaTable)luaL_ref(lua, LUA_REGISTRYINDEX);

        ConvertFromLua_SubmitScoringSummaryRequest(thread, luaRequest, request);
        TimeValue start(TimeValue::getTimeOfDay());

        LuaStats::GetInstance()->ReportError(name, err = instance->getProxy()->submitScoringSummary(&request));
        LuaStats::GetInstance()->ReportTime (name, start);
    }

    printf("%s\n", name.c_str());

    // push the error
    lua_pushinteger(lua, (int)err);

    // then the response, which in this case is nil
    lua_pushnil(lua);

    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 2) : 2;
}


// BlazeRpcError submitAthleteGameStats(Blaze::Franchise::SubmitAthleteGameStatsRequest &request)
int LuaRpc_submitAthleteGameStats(lua_State *lua)
{
    static const eastl::string name("submitAthleteGameStats");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);

    // the second needs to be our request
    luaL_checktype(lua, -1, LUA_TTABLE);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);

    BlazeRpcError err = ERR_OK;
    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());

    if (instance)
    {
        Blaze::Franchise::SubmitAthleteGameStatsRequest request;
        lua_pushvalue(lua, -1);
        LuaTable luaRequest = (LuaTable)luaL_ref(lua, LUA_REGISTRYINDEX);

        ConvertFromLua_SubmitAthleteGameStatsRequest(thread, luaRequest, request);
        TimeValue start(TimeValue::getTimeOfDay());

        LuaStats::GetInstance()->ReportError(name, err = instance->getProxy()->submitAthleteGameStats(&request));
        LuaStats::GetInstance()->ReportTime (name, start);
    }

    printf("%s\n", name.c_str());

    // push the error
    lua_pushinteger(lua, (int)err);

    // then the response, which in this case is nil
    lua_pushnil(lua);

    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 2) : 2;
}


// BlazeRpcError submitTeamGameStats(Blaze::Franchise::SubmitTeamGameStatsRequest &request)
int LuaRpc_submitTeamGameStats(lua_State *lua)
{
    static const eastl::string name("submitTeamGameStats");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);

    // the second needs to be our request
    luaL_checktype(lua, -1, LUA_TTABLE);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);

    BlazeRpcError err = ERR_OK;
    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());

    if (instance)
    {
        Blaze::Franchise::SubmitTeamGameStatsRequest request;
        lua_pushvalue(lua, -1);
        LuaTable luaRequest = (LuaTable)luaL_ref(lua, LUA_REGISTRYINDEX);

        ConvertFromLua_SubmitTeamGameStatsRequest(thread, luaRequest, request);
        TimeValue start(TimeValue::getTimeOfDay());

        LuaStats::GetInstance()->ReportError(name, err = instance->getProxy()->submitTeamGameStats(&request));
        LuaStats::GetInstance()->ReportTime (name, start);
    }

    printf("%s\n", name.c_str());

    // push the error
    lua_pushinteger(lua, (int)err);

    // then the response, which in this case is nil
    lua_pushnil(lua);

    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 2) : 2;
}


// BlazeRpcError validateRosters(Blaze::Franchise::ValidateRostersRequest &request)
int LuaRpc_validateRosters(lua_State *lua)
{
    static const eastl::string name("validateRosters");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);

    // the second needs to be our request
    luaL_checktype(lua, -1, LUA_TTABLE);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);

    BlazeRpcError err = ERR_OK;
    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());

    if (instance)
    {
        Blaze::Franchise::ValidateRostersRequest request;
        lua_pushvalue(lua, -1);
        LuaTable luaRequest = (LuaTable)luaL_ref(lua, LUA_REGISTRYINDEX);

        ConvertFromLua_ValidateRostersRequest(thread, luaRequest, request);
        TimeValue start(TimeValue::getTimeOfDay());

        LuaStats::GetInstance()->ReportError(name, err = instance->getProxy()->validateRosters(&request));
        LuaStats::GetInstance()->ReportTime (name, start);
    }

    printf("%s\n", name.c_str());

    // push the error
    lua_pushinteger(lua, (int)err);

    // then the response, which in this case is nil
    lua_pushnil(lua);

    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 2) : 2;
}


// BlazeRpcError validateDepthCharts(Blaze::Franchise::ValidateDepthChartsRequest &request)
int LuaRpc_validateDepthCharts(lua_State *lua)
{
    static const eastl::string name("validateDepthCharts");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);

    // the second needs to be our request
    luaL_checktype(lua, -1, LUA_TTABLE);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);

    BlazeRpcError err = ERR_OK;
    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());

    if (instance)
    {
        Blaze::Franchise::ValidateDepthChartsRequest request;
        lua_pushvalue(lua, -1);
        LuaTable luaRequest = (LuaTable)luaL_ref(lua, LUA_REGISTRYINDEX);

        ConvertFromLua_ValidateDepthChartsRequest(thread, luaRequest, request);
        TimeValue start(TimeValue::getTimeOfDay());

        LuaStats::GetInstance()->ReportError(name, err = instance->getProxy()->validateDepthCharts(&request));
        LuaStats::GetInstance()->ReportTime (name, start);
    }

    printf("%s\n", name.c_str());

    // push the error
    lua_pushinteger(lua, (int)err);

    // then the response, which in this case is nil
    lua_pushnil(lua);

    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 2) : 2;
}


// BlazeRpcError getCpuTrades(Blaze::Franchise::GetCpuTradesRequest &request, Blaze::Franchise::GetTradesResponse &response)
int LuaRpc_getCpuTrades(lua_State *lua)
{
    static const eastl::string name("getCpuTrades");

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
        Blaze::Franchise::GetCpuTradesRequest request;
        Blaze::Franchise::GetTradesResponse response;

        lua_pushvalue(lua, -1);
        LuaTable luaRequest = (LuaTable)luaL_ref(lua, LUA_REGISTRYINDEX);

        ConvertFromLua_GetCpuTradesRequest(thread, luaRequest, request);
        TimeValue start(TimeValue::getTimeOfDay());

        LuaStats::GetInstance()->ReportError(name, err = instance->getProxy()->getCpuTrades(&request, &response));
        LuaStats::GetInstance()->ReportTime (name, start);
        luaResponse = ConvertToLua_GetTradesResponse(thread, response);
    }

    printf("%s\n", name.c_str());

    // push the error
    lua_pushinteger(lua, (int)err);

    // then the response structure (table in Lua)
    lua_rawgeti(lua, LUA_REGISTRYINDEX, (int)luaResponse);
    luaL_unref(lua, LUA_REGISTRYINDEX, (int)luaResponse);

    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 2) : 2;
}


// BlazeRpcError getDisplayRosterByAthlete(Blaze::Franchise::GetDisplayRosterByAthleteRequest &request, Blaze::Franchise::GetDisplayRosterResponse &response)
int LuaRpc_getDisplayRosterByAthlete(lua_State *lua)
{
    static const eastl::string name("getDisplayRosterByAthlete");

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
        Blaze::Franchise::GetDisplayRosterByAthleteRequest request;
        Blaze::Franchise::GetDisplayRosterResponse response;

        lua_pushvalue(lua, -1);
        LuaTable luaRequest = (LuaTable)luaL_ref(lua, LUA_REGISTRYINDEX);

        ConvertFromLua_GetDisplayRosterByAthleteRequest(thread, luaRequest, request);
        TimeValue start(TimeValue::getTimeOfDay());

        LuaStats::GetInstance()->ReportError(name, err = instance->getProxy()->getDisplayRosterByAthlete(&request, &response));
        LuaStats::GetInstance()->ReportTime (name, start);
        luaResponse = ConvertToLua_GetDisplayRosterResponse(thread, response);
    }

    printf("%s\n", name.c_str());

    // push the error
    lua_pushinteger(lua, (int)err);

    // then the response structure (table in Lua)
    lua_rawgeti(lua, LUA_REGISTRYINDEX, (int)luaResponse);
    luaL_unref(lua, LUA_REGISTRYINDEX, (int)luaResponse);

    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 2) : 2;
}


// BlazeRpcError submitAntiCheatInfo(Blaze::Franchise::SubmitAntiCheatInfoRequest &request)
int LuaRpc_submitAntiCheatInfo(lua_State *lua)
{
    static const eastl::string name("submitAntiCheatInfo");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);

    // the second needs to be our request
    luaL_checktype(lua, -1, LUA_TTABLE);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);

    BlazeRpcError err = ERR_OK;
    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());

    if (instance)
    {
        Blaze::Franchise::SubmitAntiCheatInfoRequest request;
        lua_pushvalue(lua, -1);
        LuaTable luaRequest = (LuaTable)luaL_ref(lua, LUA_REGISTRYINDEX);

        ConvertFromLua_SubmitAntiCheatInfoRequest(thread, luaRequest, request);
        TimeValue start(TimeValue::getTimeOfDay());

        LuaStats::GetInstance()->ReportError(name, err = instance->getProxy()->submitAntiCheatInfo(&request));
        LuaStats::GetInstance()->ReportTime (name, start);
    }

    printf("%s\n", name.c_str());

    // push the error
    lua_pushinteger(lua, (int)err);

    // then the response, which in this case is nil
    lua_pushnil(lua);

    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 2) : 2;
}


// BlazeRpcError setMemberStatus(Blaze::Franchise::SetMemberStatusRequest &request)
int LuaRpc_setMemberStatus(lua_State *lua)
{
    static const eastl::string name("setMemberStatus");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);

    // the second needs to be our request
    luaL_checktype(lua, -1, LUA_TTABLE);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);

    BlazeRpcError err = ERR_OK;
    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());

    if (instance)
    {
        Blaze::Franchise::SetMemberStatusRequest request;
        lua_pushvalue(lua, -1);
        LuaTable luaRequest = (LuaTable)luaL_ref(lua, LUA_REGISTRYINDEX);

        ConvertFromLua_SetMemberStatusRequest(thread, luaRequest, request);
        TimeValue start(TimeValue::getTimeOfDay());

        LuaStats::GetInstance()->ReportError(name, err = instance->getProxy()->setMemberStatus(&request));
        LuaStats::GetInstance()->ReportTime (name, start);
    }

    printf("%s\n", name.c_str());

    // push the error
    lua_pushinteger(lua, (int)err);

    // then the response, which in this case is nil
    lua_pushnil(lua);

    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 2) : 2;
}


// BlazeRpcError getRetirements(Blaze::Franchise::GetRetirementsRequest &request, Blaze::Franchise::GetRetirementsResponse &response)
int LuaRpc_getRetirements(lua_State *lua)
{
    static const eastl::string name("getRetirements");

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
        Blaze::Franchise::GetRetirementsRequest request;
        Blaze::Franchise::GetRetirementsResponse response;

        lua_pushvalue(lua, -1);
        LuaTable luaRequest = (LuaTable)luaL_ref(lua, LUA_REGISTRYINDEX);

        ConvertFromLua_GetRetirementsRequest(thread, luaRequest, request);
        TimeValue start(TimeValue::getTimeOfDay());

        LuaStats::GetInstance()->ReportError(name, err = instance->getProxy()->getRetirements(&request, &response));
        LuaStats::GetInstance()->ReportTime (name, start);
        luaResponse = ConvertToLua_GetRetirementsResponse(thread, response);
    }

    printf("%s\n", name.c_str());

    // push the error
    lua_pushinteger(lua, (int)err);

    // then the response structure (table in Lua)
    lua_rawgeti(lua, LUA_REGISTRYINDEX, (int)luaResponse);
    luaL_unref(lua, LUA_REGISTRYINDEX, (int)luaResponse);

    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 2) : 2;
}


// BlazeRpcError getGameStatReport(Blaze::Franchise::GetGameStatReportRequest &request, Blaze::Franchise::GetGameStatReportResponse &response)
int LuaRpc_getGameStatReport(lua_State *lua)
{
    static const eastl::string name("getGameStatReport");

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
        Blaze::Franchise::GetGameStatReportRequest request;
        Blaze::Franchise::GetGameStatReportResponse response;

        lua_pushvalue(lua, -1);
        LuaTable luaRequest = (LuaTable)luaL_ref(lua, LUA_REGISTRYINDEX);

        ConvertFromLua_GetGameStatReportRequest(thread, luaRequest, request);
        TimeValue start(TimeValue::getTimeOfDay());

        LuaStats::GetInstance()->ReportError(name, err = instance->getProxy()->getGameStatReport(&request, &response));
        LuaStats::GetInstance()->ReportTime (name, start);
        luaResponse = ConvertToLua_GetGameStatReportResponse(thread, response);
    }

    printf("%s\n", name.c_str());

    // push the error
    lua_pushinteger(lua, (int)err);

    // then the response structure (table in Lua)
    lua_rawgeti(lua, LUA_REGISTRYINDEX, (int)luaResponse);
    luaL_unref(lua, LUA_REGISTRYINDEX, (int)luaResponse);

    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 2) : 2;
}


// BlazeRpcError validateMatch(Blaze::Franchise::ValidateMatchInfoRequest &request)
int LuaRpc_validateMatch(lua_State *lua)
{
    static const eastl::string name("validateMatch");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);

    // the second needs to be our request
    luaL_checktype(lua, -1, LUA_TTABLE);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);

    BlazeRpcError err = ERR_OK;
    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());

    if (instance)
    {
        Blaze::Franchise::ValidateMatchInfoRequest request;
        lua_pushvalue(lua, -1);
        LuaTable luaRequest = (LuaTable)luaL_ref(lua, LUA_REGISTRYINDEX);

        ConvertFromLua_ValidateMatchInfoRequest(thread, luaRequest, request);
        TimeValue start(TimeValue::getTimeOfDay());

        LuaStats::GetInstance()->ReportError(name, err = instance->getProxy()->validateMatch(&request));
        LuaStats::GetInstance()->ReportTime (name, start);
    }

    printf("%s\n", name.c_str());

    // push the error
    lua_pushinteger(lua, (int)err);

    // then the response, which in this case is nil
    lua_pushnil(lua);

    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 2) : 2;
}


// BlazeRpcError getServerSettings(Blaze::Franchise::GetServerSettingsResponse &response)
int LuaRpc_getServerSettings(lua_State *lua)
{
    static const eastl::string name("getServerSettings");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -1, LUA_TTABLE);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);

    BlazeRpcError err = ERR_OK;
    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());

    LuaTable luaResponse = 0;
    if (instance)
    {
        Blaze::Franchise::GetServerSettingsResponse response;

        TimeValue start(TimeValue::getTimeOfDay());

        LuaStats::GetInstance()->ReportError(name, err = instance->getProxy()->getServerSettings(&response));
        LuaStats::GetInstance()->ReportTime (name, start);
        luaResponse = ConvertToLua_GetServerSettingsResponse(thread, response);
    }

    printf("%s\n", name.c_str());

    // push the error
    lua_pushinteger(lua, (int)err);

    // then the response structure (table in Lua)
    lua_rawgeti(lua, LUA_REGISTRYINDEX, (int)luaResponse);
    luaL_unref(lua, LUA_REGISTRYINDEX, (int)luaResponse);

    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 2) : 2;
}


// BlazeRpcError setJerseyNumber(Blaze::Franchise::SetJerseyNumberRequest &request)
int LuaRpc_setJerseyNumber(lua_State *lua)
{
    static const eastl::string name("setJerseyNumber");

    // first parameter needs to be our lua stress instance
    luaL_checktype(lua, -2, LUA_TTABLE);

    // the second needs to be our request
    luaL_checktype(lua, -1, LUA_TTABLE);

    LuaThread *thread = LuaThread::GetInstanceArgument(lua);

    BlazeRpcError err = ERR_OK;
    FranchiseInstance *instance = static_cast<FranchiseInstance *>(thread->GetContainer());

    if (instance)
    {
        Blaze::Franchise::SetJerseyNumberRequest request;
        lua_pushvalue(lua, -1);
        LuaTable luaRequest = (LuaTable)luaL_ref(lua, LUA_REGISTRYINDEX);

        ConvertFromLua_SetJerseyNumberRequest(thread, luaRequest, request);
        TimeValue start(TimeValue::getTimeOfDay());

        LuaStats::GetInstance()->ReportError(name, err = instance->getProxy()->setJerseyNumber(&request));
        LuaStats::GetInstance()->ReportTime (name, start);
    }

    printf("%s\n", name.c_str());

    // push the error
    lua_pushinteger(lua, (int)err);

    // then the response, which in this case is nil
    lua_pushnil(lua);

    return LuaStats::GetInstance()->GetYieldAll() ? lua_yield(lua, 2) : 2;
}

} // namespace Lua

void RegisterLuaFunctions(lua_State *lua)
{
    lua_register(lua, "getDraftableAthletes", Lua::LuaRpc_getDraftableAthletes);
    lua_register(lua, "getDraftProfile", Lua::LuaRpc_getDraftProfile);
    lua_register(lua, "setDraftProfile", Lua::LuaRpc_setDraftProfile);
    lua_register(lua, "createFranchise", Lua::LuaRpc_createFranchise);
    lua_register(lua, "joinFranchise", Lua::LuaRpc_joinFranchise);
    lua_register(lua, "getFranchise", Lua::LuaRpc_getFranchise);
    lua_register(lua, "getFranchisesByUser", Lua::LuaRpc_getFranchisesByUser);
    lua_register(lua, "deleteFranchise", Lua::LuaRpc_deleteFranchise);
    lua_register(lua, "getMemberByBlazeId", Lua::LuaRpc_getMemberByBlazeId);
    lua_register(lua, "getMemberByTeamId", Lua::LuaRpc_getMemberByTeamId);
    lua_register(lua, "findFranchises", Lua::LuaRpc_findFranchises);
    lua_register(lua, "findFranchisesAsync", Lua::LuaRpc_findFranchisesAsync);
    lua_register(lua, "removeMember", Lua::LuaRpc_removeMember);
    lua_register(lua, "updateFranchiseSettings", Lua::LuaRpc_updateFranchiseSettings);
    lua_register(lua, "postNews", Lua::LuaRpc_postNews);
    lua_register(lua, "getNews", Lua::LuaRpc_getNews);
    lua_register(lua, "clearBan", Lua::LuaRpc_clearBan);
    lua_register(lua, "sendInvitation", Lua::LuaRpc_sendInvitation);
    lua_register(lua, "getInvitations", Lua::LuaRpc_getInvitations);
    lua_register(lua, "processInvitation", Lua::LuaRpc_processInvitation);
    lua_register(lua, "proposeTrade", Lua::LuaRpc_proposeTrade);
    lua_register(lua, "processTrade", Lua::LuaRpc_processTrade);
    lua_register(lua, "getTrades", Lua::LuaRpc_getTrades);
    lua_register(lua, "getMembers", Lua::LuaRpc_getMembers);
    lua_register(lua, "joinDraft", Lua::LuaRpc_joinDraft);
    lua_register(lua, "leaveDraft", Lua::LuaRpc_leaveDraft);
    lua_register(lua, "beginDraft", Lua::LuaRpc_beginDraft);
    lua_register(lua, "pauseDraft", Lua::LuaRpc_pauseDraft);
    lua_register(lua, "draftAthlete", Lua::LuaRpc_draftAthlete);
    lua_register(lua, "getDraftOrder", Lua::LuaRpc_getDraftOrder);
    lua_register(lua, "sendDraftChat", Lua::LuaRpc_sendDraftChat);
    lua_register(lua, "getDraftHistory", Lua::LuaRpc_getDraftHistory);
    lua_register(lua, "pollDraft", Lua::LuaRpc_pollDraft);
    lua_register(lua, "checkDraft", Lua::LuaRpc_checkDraft);
    lua_register(lua, "getDisplayRoster", Lua::LuaRpc_getDisplayRoster);
    lua_register(lua, "getGameplayRoster", Lua::LuaRpc_getGameplayRoster);
    lua_register(lua, "getDisplayAthlete", Lua::LuaRpc_getDisplayAthlete);
    lua_register(lua, "getDepthChart", Lua::LuaRpc_getDepthChart);
    lua_register(lua, "setDepthChart", Lua::LuaRpc_setDepthChart);
    lua_register(lua, "generateDepthChart", Lua::LuaRpc_generateDepthChart);
    lua_register(lua, "getRegularSchedule", Lua::LuaRpc_getRegularSchedule);
    lua_register(lua, "getPlayoffSchedule", Lua::LuaRpc_getPlayoffSchedule);
    lua_register(lua, "getGameplayInformation", Lua::LuaRpc_getGameplayInformation);
    lua_register(lua, "AdvanceFranchise", Lua::LuaRpc_AdvanceFranchise);
    lua_register(lua, "SimSeason", Lua::LuaRpc_SimSeason);
    lua_register(lua, "athleteProgress", Lua::LuaRpc_athleteProgress);
    lua_register(lua, "getRawData", Lua::LuaRpc_getRawData);
    lua_register(lua, "signAthlete", Lua::LuaRpc_signAthlete);
    lua_register(lua, "releaseAthlete", Lua::LuaRpc_releaseAthlete);
    lua_register(lua, "getNumTeamInjuries", Lua::LuaRpc_getNumTeamInjuries);
    lua_register(lua, "getInjuriesByTeam", Lua::LuaRpc_getInjuriesByTeam);
    lua_register(lua, "getGameBoxScoreByGame", Lua::LuaRpc_getGameBoxScoreByGame);
    lua_register(lua, "getGameBoxScoresByWeek", Lua::LuaRpc_getGameBoxScoresByWeek);
    lua_register(lua, "getGameBoxScoresByTeam", Lua::LuaRpc_getGameBoxScoresByTeam);
    lua_register(lua, "getScoringSummary", Lua::LuaRpc_getScoringSummary);
    lua_register(lua, "getTeamStatsByGame", Lua::LuaRpc_getTeamStatsByGame);
    lua_register(lua, "getAthleteStatsByGame", Lua::LuaRpc_getAthleteStatsByGame);
    lua_register(lua, "promoteToGM", Lua::LuaRpc_promoteToGM);
    lua_register(lua, "getFranchiseRank", Lua::LuaRpc_getFranchiseRank);
    lua_register(lua, "getTopFranchises", Lua::LuaRpc_getTopFranchises);
    lua_register(lua, "getSuperbowls", Lua::LuaRpc_getSuperbowls);
    lua_register(lua, "submitFranchiseGameStats", Lua::LuaRpc_submitFranchiseGameStats);
    lua_register(lua, "submitScoringSummary", Lua::LuaRpc_submitScoringSummary);
    lua_register(lua, "submitAthleteGameStats", Lua::LuaRpc_submitAthleteGameStats);
    lua_register(lua, "submitTeamGameStats", Lua::LuaRpc_submitTeamGameStats);
    lua_register(lua, "validateRosters", Lua::LuaRpc_validateRosters);
    lua_register(lua, "validateDepthCharts", Lua::LuaRpc_validateDepthCharts);
    lua_register(lua, "getCpuTrades", Lua::LuaRpc_getCpuTrades);
    lua_register(lua, "getDisplayRosterByAthlete", Lua::LuaRpc_getDisplayRosterByAthlete);
    lua_register(lua, "submitAntiCheatInfo", Lua::LuaRpc_submitAntiCheatInfo);
    lua_register(lua, "setMemberStatus", Lua::LuaRpc_setMemberStatus);
    lua_register(lua, "getRetirements", Lua::LuaRpc_getRetirements);
    lua_register(lua, "getGameStatReport", Lua::LuaRpc_getGameStatReport);
    lua_register(lua, "validateMatch", Lua::LuaRpc_validateMatch);
    lua_register(lua, "getServerSettings", Lua::LuaRpc_getServerSettings);
    lua_register(lua, "setJerseyNumber", Lua::LuaRpc_setJerseyNumber);
}

} // namespace Stress
} // namespace Blaze