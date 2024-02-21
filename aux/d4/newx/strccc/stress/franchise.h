/******************************************************************************/
/*!

    Copyright 2009 Electronic Arts Inc.

    \file       franchise.h
    \brief      DO NOT EDIT - GENERATED FILE!
                 Header file for Lua RPC stress test bindings.
*/
/******************************************************************************/

#ifndef BLAZE_FRANCHISE_FRANCHISE_STRESS_H
#define BLAZE_FRANCHISE_FRANCHISE_STRESS_H


/*** Includes *****************************************************************/

#include "framework/blaze.h"
#include "blazerpcerrors.h"

struct lua_State;

namespace Blaze
{
namespace Stress
{
namespace Lua
{

// BlazeRpcError getDraftableAthletes(Blaze::Franchise::GetDraftableAthletesRequest &request, Blaze::Franchise::GetDraftableAthletesResponse &response);
int LuaRpc_getDraftableAthletes(lua_State *lua);

// BlazeRpcError getDraftProfile(Blaze::Franchise::GetDraftProfileRequest &request, Blaze::Franchise::DraftProfile &response);
int LuaRpc_getDraftProfile(lua_State *lua);

// BlazeRpcError setDraftProfile(Blaze::Franchise::SetDraftProfileRequest &request);
int LuaRpc_setDraftProfile(lua_State *lua);

// BlazeRpcError createFranchise(Blaze::Franchise::CreateFranchiseRequest &request, Blaze::Franchise::CreateFranchiseResponse &response);
int LuaRpc_createFranchise(lua_State *lua);

// BlazeRpcError joinFranchise(Blaze::Franchise::JoinFranchiseRequest &request);
int LuaRpc_joinFranchise(lua_State *lua);

// BlazeRpcError getFranchise(Blaze::Franchise::GetFranchiseRequest &request, Blaze::Franchise::Franchise &response);
int LuaRpc_getFranchise(lua_State *lua);

// BlazeRpcError getFranchisesByUser(Blaze::Franchise::FindFranchisesResponse &response);
int LuaRpc_getFranchisesByUser(lua_State *lua);

// BlazeRpcError deleteFranchise(Blaze::Franchise::DeleteFranchiseRequest &request);
int LuaRpc_deleteFranchise(lua_State *lua);

// BlazeRpcError getMemberByBlazeId(Blaze::Franchise::GetMemberByBlazeIdRequest &request, Blaze::Franchise::MemberInfo &response);
int LuaRpc_getMemberByBlazeId(lua_State *lua);

// BlazeRpcError getMemberByTeamId(Blaze::Franchise::GetMemberByTeamIdRequest &request, Blaze::Franchise::MemberInfo &response);
int LuaRpc_getMemberByTeamId(lua_State *lua);

// BlazeRpcError findFranchises(Blaze::Franchise::FindFranchisesRequest &request, Blaze::Franchise::FindFranchisesResponse &response);
int LuaRpc_findFranchises(lua_State *lua);

// BlazeRpcError findFranchisesAsync(Blaze::Franchise::FindFranchisesRequest &request);
int LuaRpc_findFranchisesAsync(lua_State *lua);

// BlazeRpcError removeMember(Blaze::Franchise::RemoveMemberRequest &request);
int LuaRpc_removeMember(lua_State *lua);

// BlazeRpcError updateFranchiseSettings(Blaze::Franchise::UpdateFranchiseSettingsRequest &request);
int LuaRpc_updateFranchiseSettings(lua_State *lua);

// BlazeRpcError postNews(Blaze::Franchise::PostNewsRequest &request);
int LuaRpc_postNews(lua_State *lua);

// BlazeRpcError getNews(Blaze::Franchise::GetNewsRequest &request, Blaze::Franchise::GetNewsResponse &response);
int LuaRpc_getNews(lua_State *lua);

// BlazeRpcError clearBan(Blaze::Franchise::ClearBanRequest &request);
int LuaRpc_clearBan(lua_State *lua);

// BlazeRpcError sendInvitation(Blaze::Franchise::SendInvitationRequest &request);
int LuaRpc_sendInvitation(lua_State *lua);

// BlazeRpcError getInvitations(Blaze::Franchise::GetInvitationsRequest &request, Blaze::Franchise::GetInvitationsResponse &response);
int LuaRpc_getInvitations(lua_State *lua);

// BlazeRpcError processInvitation(Blaze::Franchise::ProcessInvitationRequest &request);
int LuaRpc_processInvitation(lua_State *lua);

// BlazeRpcError proposeTrade(Blaze::Franchise::ProposeTradeRequest &request);
int LuaRpc_proposeTrade(lua_State *lua);

// BlazeRpcError processTrade(Blaze::Franchise::ProcessTradeRequest &request);
int LuaRpc_processTrade(lua_State *lua);

// BlazeRpcError getTrades(Blaze::Franchise::GetTradesRequest &request, Blaze::Franchise::GetTradesResponse &response);
int LuaRpc_getTrades(lua_State *lua);

// BlazeRpcError getMembers(Blaze::Franchise::GetMembersRequest &request, Blaze::Franchise::GetMembersResponse &response);
int LuaRpc_getMembers(lua_State *lua);

// BlazeRpcError joinDraft(Blaze::Franchise::JoinDraftRequest &request, Blaze::Franchise::JoinDraftResponse &response);
int LuaRpc_joinDraft(lua_State *lua);

// BlazeRpcError leaveDraft(Blaze::Franchise::LeaveDraftRequest &request);
int LuaRpc_leaveDraft(lua_State *lua);

// BlazeRpcError beginDraft(Blaze::Franchise::BeginDraftRequest &request);
int LuaRpc_beginDraft(lua_State *lua);

// BlazeRpcError pauseDraft(Blaze::Franchise::PauseDraftRequest &request);
int LuaRpc_pauseDraft(lua_State *lua);

// BlazeRpcError draftAthlete(Blaze::Franchise::DraftAthleteRequest &request);
int LuaRpc_draftAthlete(lua_State *lua);

// BlazeRpcError getDraftOrder(Blaze::Franchise::GetDraftOrderRequest &request, Blaze::Franchise::GetDraftOrderResponse &response);
int LuaRpc_getDraftOrder(lua_State *lua);

// BlazeRpcError sendDraftChat(Blaze::Franchise::SendDraftChatRequest &request);
int LuaRpc_sendDraftChat(lua_State *lua);

// BlazeRpcError getDraftHistory(Blaze::Franchise::GetDraftHistoryRequest &request, Blaze::Franchise::GetDraftHistoryResponse &response);
int LuaRpc_getDraftHistory(lua_State *lua);

// BlazeRpcError pollDraft(Blaze::Franchise::PollDraftRequest &request, Blaze::Franchise::PollDraftResponse &response);
int LuaRpc_pollDraft(lua_State *lua);

// BlazeRpcError checkDraft(Blaze::Franchise::CheckDraftRequest &request, Blaze::Franchise::CheckDraftResponse &response);
int LuaRpc_checkDraft(lua_State *lua);

// BlazeRpcError getDisplayRoster(Blaze::Franchise::GetDisplayRosterRequest &request, Blaze::Franchise::GetDisplayRosterResponse &response);
int LuaRpc_getDisplayRoster(lua_State *lua);

// BlazeRpcError getGameplayRoster(Blaze::Franchise::GetGameplayRosterRequest &request, Blaze::Franchise::GameplayRoster &response);
int LuaRpc_getGameplayRoster(lua_State *lua);

// BlazeRpcError getDisplayAthlete(Blaze::Franchise::GetDisplayAthleteRequest &request, Blaze::Franchise::GetDisplayAthleteResponse &response);
int LuaRpc_getDisplayAthlete(lua_State *lua);

// BlazeRpcError getDepthChart(Blaze::Franchise::GetDepthChartRequest &request, Blaze::Franchise::GetDepthChartResponse &response);
int LuaRpc_getDepthChart(lua_State *lua);

// BlazeRpcError setDepthChart(Blaze::Franchise::SetDepthChartRequest &request);
int LuaRpc_setDepthChart(lua_State *lua);

// BlazeRpcError generateDepthChart(Blaze::Franchise::GenerateDepthChartRequest &request);
int LuaRpc_generateDepthChart(lua_State *lua);

// BlazeRpcError getRegularSchedule(Blaze::Franchise::GetRegularScheduleRequest &request, Blaze::Franchise::GetRegularScheduleResponse &response);
int LuaRpc_getRegularSchedule(lua_State *lua);

// BlazeRpcError getPlayoffSchedule(Blaze::Franchise::GetPlayoffScheduleRequest &request, Blaze::Franchise::GetPlayoffScheduleResponse &response);
int LuaRpc_getPlayoffSchedule(lua_State *lua);

// BlazeRpcError getGameplayInformation(Blaze::Franchise::GetGameplayInformationRequest &request, Blaze::Franchise::GetGameplayInformationResponse &response);
int LuaRpc_getGameplayInformation(lua_State *lua);

// BlazeRpcError AdvanceFranchise(Blaze::Franchise::AdvanceFranchiseRequest &request);
int LuaRpc_AdvanceFranchise(lua_State *lua);

// BlazeRpcError SimSeason(Blaze::Franchise::SimSeasonRequest &request);
int LuaRpc_SimSeason(lua_State *lua);

// BlazeRpcError athleteProgress(Blaze::Franchise::AthleteProgressRequest &request);
int LuaRpc_athleteProgress(lua_State *lua);

// BlazeRpcError getRawData(Blaze::Franchise::GetRawDataRequest &request, Blaze::Franchise::GetRawDataResponse &response);
int LuaRpc_getRawData(lua_State *lua);

// BlazeRpcError signAthlete(Blaze::Franchise::SignAthleteRequest &request);
int LuaRpc_signAthlete(lua_State *lua);

// BlazeRpcError releaseAthlete(Blaze::Franchise::ReleaseAthleteRequest &request);
int LuaRpc_releaseAthlete(lua_State *lua);

// BlazeRpcError getNumTeamInjuries(Blaze::Franchise::GetNumTeamInjuriesRequest &request, Blaze::Franchise::GetNumTeamInjuriesResponse &response);
int LuaRpc_getNumTeamInjuries(lua_State *lua);

// BlazeRpcError getInjuriesByTeam(Blaze::Franchise::GetInjuriesByTeamRequest &request, Blaze::Franchise::GetInjuriesByTeamResponse &response);
int LuaRpc_getInjuriesByTeam(lua_State *lua);

// BlazeRpcError getGameBoxScoreByGame(Blaze::Franchise::GetGameBoxScoreByGameRequest &request, Blaze::Franchise::GameBoxScore &response);
int LuaRpc_getGameBoxScoreByGame(lua_State *lua);

// BlazeRpcError getGameBoxScoresByWeek(Blaze::Franchise::GetGameBoxScoresByWeekRequest &request, Blaze::Franchise::GetGameBoxScoresByWeekResponse &response);
int LuaRpc_getGameBoxScoresByWeek(lua_State *lua);

// BlazeRpcError getGameBoxScoresByTeam(Blaze::Franchise::GetGameBoxScoresByTeamRequest &request, Blaze::Franchise::GetGameBoxScoresByTeamResponse &response);
int LuaRpc_getGameBoxScoresByTeam(lua_State *lua);

// BlazeRpcError getScoringSummary(Blaze::Franchise::GetScoringSummaryRequest &request, Blaze::Franchise::GetScoringSummaryResponse &response);
int LuaRpc_getScoringSummary(lua_State *lua);

// BlazeRpcError getTeamStatsByGame(Blaze::Franchise::GetTeamStatsByGameRequest &request, Blaze::Franchise::GetTeamStatsByGameResponse &response);
int LuaRpc_getTeamStatsByGame(lua_State *lua);

// BlazeRpcError getAthleteStatsByGame(Blaze::Franchise::GetAthleteStatsByGameRequest &request, Blaze::Franchise::GetAthleteStatsByGameResponse &response);
int LuaRpc_getAthleteStatsByGame(lua_State *lua);

// BlazeRpcError promoteToGM(Blaze::Franchise::PromoteToGMRequest &request);
int LuaRpc_promoteToGM(lua_State *lua);

// BlazeRpcError getFranchiseRank(Blaze::Franchise::GetFranchiseRankRequest &request, Blaze::Franchise::FranchiseRank &response);
int LuaRpc_getFranchiseRank(lua_State *lua);

// BlazeRpcError getTopFranchises(Blaze::Franchise::GetTopFranchisesRequest &request, Blaze::Franchise::GetTopFranchisesResponse &response);
int LuaRpc_getTopFranchises(lua_State *lua);

// BlazeRpcError getSuperbowls(Blaze::Franchise::GetSuperbowlsRequest &request, Blaze::Franchise::GetSuperbowlsResponse &response);
int LuaRpc_getSuperbowls(lua_State *lua);

// BlazeRpcError submitFranchiseGameStats(Blaze::Franchise::SubmitFranchiseGameStatsRequest &request);
int LuaRpc_submitFranchiseGameStats(lua_State *lua);

// BlazeRpcError submitScoringSummary(Blaze::Franchise::SubmitScoringSummaryRequest &request);
int LuaRpc_submitScoringSummary(lua_State *lua);

// BlazeRpcError submitAthleteGameStats(Blaze::Franchise::SubmitAthleteGameStatsRequest &request);
int LuaRpc_submitAthleteGameStats(lua_State *lua);

// BlazeRpcError submitTeamGameStats(Blaze::Franchise::SubmitTeamGameStatsRequest &request);
int LuaRpc_submitTeamGameStats(lua_State *lua);

// BlazeRpcError validateRosters(Blaze::Franchise::ValidateRostersRequest &request);
int LuaRpc_validateRosters(lua_State *lua);

// BlazeRpcError validateDepthCharts(Blaze::Franchise::ValidateDepthChartsRequest &request);
int LuaRpc_validateDepthCharts(lua_State *lua);

// BlazeRpcError getCpuTrades(Blaze::Franchise::GetCpuTradesRequest &request, Blaze::Franchise::GetTradesResponse &response);
int LuaRpc_getCpuTrades(lua_State *lua);

// BlazeRpcError getDisplayRosterByAthlete(Blaze::Franchise::GetDisplayRosterByAthleteRequest &request, Blaze::Franchise::GetDisplayRosterResponse &response);
int LuaRpc_getDisplayRosterByAthlete(lua_State *lua);

// BlazeRpcError submitAntiCheatInfo(Blaze::Franchise::SubmitAntiCheatInfoRequest &request);
int LuaRpc_submitAntiCheatInfo(lua_State *lua);

// BlazeRpcError setMemberStatus(Blaze::Franchise::SetMemberStatusRequest &request);
int LuaRpc_setMemberStatus(lua_State *lua);

// BlazeRpcError getRetirements(Blaze::Franchise::GetRetirementsRequest &request, Blaze::Franchise::GetRetirementsResponse &response);
int LuaRpc_getRetirements(lua_State *lua);

// BlazeRpcError getGameStatReport(Blaze::Franchise::GetGameStatReportRequest &request, Blaze::Franchise::GetGameStatReportResponse &response);
int LuaRpc_getGameStatReport(lua_State *lua);

// BlazeRpcError validateMatch(Blaze::Franchise::ValidateMatchInfoRequest &request);
int LuaRpc_validateMatch(lua_State *lua);

// BlazeRpcError getServerSettings(Blaze::Franchise::GetServerSettingsResponse &response);
int LuaRpc_getServerSettings(lua_State *lua);

// BlazeRpcError setJerseyNumber(Blaze::Franchise::SetJerseyNumberRequest &request);
int LuaRpc_setJerseyNumber(lua_State *lua);

} // namespace Lua

void RegisterLuaFunctions(lua_State *lua);

} // namespace Stress
} // namespace Blaze

#endif  //  BLAZE_FRANCHISE_FRANCHISE_STRESS_H

