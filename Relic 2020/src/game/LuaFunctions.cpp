#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

#include <string>

#include "net/NetGame.h"
#include "render/Light.h"

extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

LUALIB_API void luaL_checktypeornil(lua_State *L, int narg, int t);
LUALIB_API void *luaL_checkudataornil(lua_State *L, int ud, const char *tname);
}

#include "luautil/LuaUtils.h"
#include "luautil/LuaThread.h"
#include "luautil/LuaPack.h"
#include "luautil/LuaEvents.h"

#include "serialize/Codec.h"

#include "LuaFunctions_Ui.h"
#include "LuaFunctions_Math.h"

#include "Character.h"
#include "GameState.h"
#include "Multiplayer.h"

#include "render/OverheadCamera.h"
#include "render/QuadRenderer.h"
#include "render/LocalGameServices.h"
#include "render/ContentLoader.h"
#include "render/ParticleEffects.h"
#include "render/GLUtils.h"
#include "render/Light.h"
#include "render/GLStates.h"
#include "render/GraphicsDevice.h"
#include "map/DynamicMap.h"
#include "map/Map.h"
#include "map/ITileMap.h"
#include "gamecore/IGameObject.h"
#include "gamecore/IEventList.h"
#include "events/Event.h"
#include "ui/GameUI.h"
#include "core/strs.h"
#include "serialize/LuaSerialize.h"
#include "game/Sync.h"
#include "time/Timer.h"
#include "input/GameInput.h"
#include "map/ObjectTileSet.h"

#include "level/Level.h"

#include "fast/Allocator.h"

#include "audio/SoundManager.h"
#include "game/Game.h"

#include "net/ENetUtils.h"
#include "GLAbstract.h"

extern bool  buttons[32];

extern "C" bool GetPlatformIsiPad(void);

extern "C" void SendEmail(const char *subject, const char *message);
//extern "C" void StartCrystal();
//extern "C" void ActivateCrystaliPad();
//extern "C" void DeactivateCrystaliPad();
//extern "C" void ShowCrystalLeaderboards();
//extern "C" void ShowCrystalAchievements();
extern "C" bool IsHiResDevice();
extern "C" void CaptureScreen();
extern "C" void SetAnimationFrameInterval(int value);
extern "C" void PostAchievement(const char *achievementId, const char *description);
extern "C" void PostLeaderboard(const char *leaderboardId, double value);
extern "C" void Alert(const char *title, const char *text);
extern "C" void SendMemoryWarning();
//extern "C" bool IsCrystalActive();
//extern "C" bool ShouldCrystalActivate();

void LuaReportError(const char *string);

class GraphicsDevice;

static GameInput &GetInput()
{
    GameState *state = GameState::GetInstance();
    return state->GetInput();
}

int GetGlobalFrames()
{
    GameState *state = GameState::GetInstance();
    return state->GetFrames();
}

bool gIsLowEndDevice = false;

bool IsLowEndDevice()
{
    return gIsLowEndDevice;
}

void SetIsLowEndDevice(bool isLowEnd)
{
    gIsLowEndDevice = isLowEnd;
}

static GraphicsDevice *GetDevice()
{
    return GraphicsDevice::GetInstance();
}

class CompileData
{
    enum { MAX_COMPILE_SIZE = 65536 };

public:

    inline CompileData() : mWriter(mData) { memset(mData, 0, MAX_COMPILE_SIZE); }

    bool Write(const void *data, int size)
    {
        if (mWriter + size >= reinterpret_cast<const char *>(data) + MAX_COMPILE_SIZE)
            return false;
        memcpy(mWriter, data, size);
        mWriter += size;
        return true;
    }
    inline int         Size() const { return static_cast<int>(mWriter - mData); }
    inline const char *Data() const { return mData; }

private:

    char  mData[MAX_COMPILE_SIZE];
    char *mWriter;
};

#ifdef WIN32
#include <process.h>
extern "C" void GoToUrl(const char *url)
{
    _spawnl(_P_DETACH, "C:\\Program Files\\Internet Explorer\\iexplore.exe", " ", url, NULL);
}
#endif

extern "C" void GoToUrl(const char *url);

static int GoToLink(lua_State *lua)
{
    luaL_checktype(lua, -4, LUA_TSTRING);
    char url[4096];
    memset(url, 0, sizeof(url));
#ifdef WIN32
    _snprintf(url, sizeof(url) - 2, "http://axolotlstudios.com/links/%s/", lua_tostring(lua, -1));
#else
    snprintf(url, sizeof(url) - 2, "http://axolotlstudios.com/links/%s/", lua_tostring(lua, -1));
#endif
    GoToUrl(url);
    return 0;
}

static int CompileWriter(lua_State *, const void *p, size_t size, void *u)
{
    CompileData *data = reinterpret_cast<CompileData *>(u);
    return data->Write(p, static_cast<int>(size)) ? 0 : 1;
}

static int LuaCompileScript(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TSTRING);
    const char *script = lua_tostring(lua, -1);
    luaL_loadstring(lua, script);
    CompileData data;
    lua_dump(lua, CompileWriter, &data);
    lua_pop(lua, 1);
    lua_pushlstring(lua, data.Data(), data.Size());
    return 1;
}

static int LuaBreakpoint(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TSTRING);
    const char *message = lua_tostring(lua, -1);
    printf("BKPT: %s\n", message);
    core_wait(0);
    return 0;
}

static int CaptureScreen(lua_State *lua)
{
    CaptureScreen();
    return 0;
}

static int SendMemoryWarning(lua_State *lua)
{
    SendMemoryWarning();
    return 0;
}

static int Alert(lua_State *lua)
{
    luaL_checktype(lua, -2, LUA_TSTRING);
    luaL_checktype(lua, -1, LUA_TSTRING);	
    const char *title = lua_tostring(lua, -2);	
    const char *message = lua_tostring(lua, -1);
	Alert(title, message);
    return 0;
}

static int SendEmail(lua_State *lua)
{
    luaL_checktype(lua, -2, LUA_TSTRING);
    luaL_checktype(lua, -1, LUA_TSTRING);	
    const char *subject = lua_tostring(lua, -2);	
    const char *message = lua_tostring(lua, -1);
//  SendEmail(subject, message);
    return 0;
}

static int PostAchievement(lua_State *lua)
{
    luaL_checktype(lua, -2, LUA_TSTRING);
    luaL_checktype(lua, -1, LUA_TSTRING);	
    const char *achievementId = lua_tostring(lua, -2);	
    const char *description = lua_tostring(lua, -1);
	PostAchievement(achievementId, description);
    return 0;
}

static int PostLeaderboard(lua_State *lua)
{
    luaL_checktype(lua, -2, LUA_TSTRING);
    luaL_checktype(lua, -1, LUA_TNUMBER);	
    const char *leaderboardId = lua_tostring(lua, -2);	
    double value = (double)lua_tonumber(lua, -1);
	PostLeaderboard(leaderboardId, value);
    return 0;
}

static int Lua_SetLight0OnPlayer(lua_State *lua)
{
    Vector3 v = GetGlobalGame()->GetPlayer()->GetPosition();
    Vector4 v4(v.x, v.y, v.z - 64, 1.0);
    GLSetLight4(GL_LIGHT0, GL_POSITION, v4.data());
    return 0;
}

static int SetAnimationFrameInterval(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TNUMBER);	
	SetAnimationFrameInterval((int)lua_tointeger(lua, -1));
    return 0;
}

static int Audio_PlayTheme(lua_State *lua)
{
    SoundManager *sounds = SoundManager::GetInstance();
    luaL_checktype(lua, -2, LUA_TSTRING);
    luaL_checktype(lua, -1, LUA_TNUMBER);
    sounds->PlayTheme(lua_tostring(lua, -2), (float)lua_tonumber(lua, -1));
    return 0;
}

static int Audio_StopTheme(lua_State *lua)
{
    SoundManager *sounds = SoundManager::GetInstance();
    sounds->StopTheme();
    return 0;
}

static int Audio_Play(lua_State *lua)
{
    SoundManager *sounds = SoundManager::GetInstance();
    luaL_checktype(lua, -4, LUA_TSTRING);
    luaL_checktype(lua, -3, LUA_TSTRING);
    luaL_checktype(lua, -2, LUA_TNUMBER);
    luaL_checktype(lua, -1, LUA_TBOOLEAN);
    sounds->Play(lua_tostring(lua, -4), lua_tostring(lua, -3), (float)lua_tonumber(lua, -2), lua_toboolean(lua, -1) != 0);
    return 0;
}

static int Audio_SetAmbientVolume(lua_State *lua)
{
    SoundManager *sounds = SoundManager::GetInstance();
    luaL_checktype(lua, -1, LUA_TNUMBER);
    sounds->SetAmbientVolume((float)lua_tonumber(lua, -1));
    return 0;
}

static int Audio_SetListenerPosition(lua_State *lua)
{
    SoundManager *sounds = SoundManager::GetInstance();
    luaL_checktype(lua, -3, LUA_TNUMBER); // x
    luaL_checktype(lua, -2, LUA_TNUMBER); // y
    luaL_checktype(lua, -1, LUA_TNUMBER); // z
    sounds->SetListenerPosition(Vector3((float)lua_tonumber(lua, -3), 
                                        (float)lua_tonumber(lua, -2), 
                                        (float)lua_tonumber(lua, -1)));
    return 0;
}

static int Audio_PlayAt(lua_State *lua)
{
    SoundManager *sounds = SoundManager::GetInstance();
    luaL_checktype(lua, -5, LUA_TSTRING);
    luaL_checktype(lua, -4, LUA_TNUMBER);
    luaL_checktype(lua, -3, LUA_TNUMBER);
    luaL_checktype(lua, -2, LUA_TNUMBER);
    luaL_checktype(lua, -1, LUA_TNUMBER);
    Vector3 at((float)lua_tonumber(lua, -4), (float)lua_tonumber(lua, -3), (float)lua_tonumber(lua, -2));
    sounds->PlayAt(lua_tostring(lua, -5), at, (float)lua_tonumber(lua, -1));
    return 0;
}

static int Audio_Load(lua_State *lua)
{
    SoundManager *sounds = SoundManager::GetInstance();
    luaL_checktype(lua, -2, LUA_TSTRING);
    luaL_checktype(lua, -1, LUA_TSTRING);
    sounds->Load(lua_tostring(lua, -2), lua_tostring(lua, -1));
    return 0;
}

static int Audio_StartAmbient(lua_State *lua)
{
    SoundManager *sounds = SoundManager::GetInstance();
    sounds->StartAmbient();
    return 0;
}

static int Audio_StopAmbient(lua_State *lua)
{
    SoundManager *sounds = SoundManager::GetInstance();
    sounds->StopAmbient();
    return 0;
}

// $TODO This DEFERRED CALL STUFF NEEDS TO GO ELSEWHERE & GET GENERALIZED
extern void DestroyCharacter(Character *);
extern Character *GetCharacter(lua_State *, int index = 1);

static int AddContentBundle(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TSTRING);
    ContentLoader::GetInstance()->AddBundle(
        globalTranslatePath(lua_tostring(lua, -1)));
    return 0;    
}

static int RemoveContentBundle(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TSTRING);
    ContentLoader::GetInstance()->RemoveBundle(lua_tostring(lua, -1));
    return 0;    
}

static int PreloadTexture(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TSTRING);
    ContentLoader::GetInstance()->PreloadTexture(lua_tostring(lua, -1));
    return 0;    
}

static int UnloadTexture(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TSTRING);
    ContentLoader::GetInstance()->UnloadTexture(lua_tostring(lua, -1));
    return 0;
}

static int PreloadMesh(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TSTRING);
    ContentLoader::GetInstance()->PreloadMesh(lua_tostring(lua, -1));
    return 0;    
}

static int UnloadMesh(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TSTRING);
    ContentLoader::GetInstance()->UnloadMesh(lua_tostring(lua, -1));
    return 0;
}

static int GetGameStateFrames(lua_State *lua)
{
    GameState *state = GameState::GetInstance();
    lua_pushinteger(lua, state->GetFrames());
    return 1;
}

static int GetGameFrames(lua_State *lua)
{
    GameState *state = GameState::GetInstance();
    int frames = 0;
    if (state->GetGame()) {
        frames = GetGlobalGame()->GetFrames();
    }
    lua_pushinteger(lua, frames);
    return 1;
}

static int GetVectorInput(lua_State *lua)
{
    Vector2 v = GetInput().move.GetVector();
    lua_pushnumber(lua, v.x); // y 
    lua_pushnumber(lua, v.y); // x
    lua_pushnumber(lua, 0.f); // z
//  lua_pushnumber(lua, accel[1]); // y // x and y are swapped in the accel vector
//  lua_pushnumber(lua, accel[0]); // x // retarded but true
//  lua_pushnumber(lua, accel[2]); // z
    return 3;
}

static int GetTouchPoint(lua_State *lua)
{
    Vector3 v = GetInput().GetTouchPoint(GameInput::TouchAction_Began);
    lua_pushnumber(lua, v.x); // y
    lua_pushnumber(lua, v.y); // x
    lua_pushnumber(lua, v.z); // z
//  lua_pushnumber(lua, accel[1]); // y // x and y are swapped in the accel vector
//  lua_pushnumber(lua, accel[0]); // x // retarded but true
//  lua_pushnumber(lua, accel[2]); // z
    return 3;
}

static int GetTransformedTouchPoint(lua_State *lua)
{
    Vector3 v = GetInput().GetTransformedTouchPoint(GameInput::TouchAction_Began);
    lua_pushnumber(lua, v.x); // y
    lua_pushnumber(lua, v.y); // x
    lua_pushnumber(lua, v.z); // z
//  lua_pushnumber(lua, accel[1]); // y // x and y are swapped in the accel vector
//  lua_pushnumber(lua, accel[0]); // x // retarded but true
//  lua_pushnumber(lua, accel[2]); // z
    return 3;
}

static int SetIsLowEndDevice(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TBOOLEAN);
    bool lowend = lua_toboolean(lua, -1) ? true : false;
    SetIsLowEndDevice(lowend);
    return 0;
}

static int ShouldCrystalActivate(lua_State *lua)
{
    lua_pushboolean(lua, /*ShouldCrystalActivate() ? 1 :*/ 0);
    return 1;
}

static int IsCrystalActive(lua_State *lua)
{
    lua_pushboolean(lua, /*IsCrystalActive() ? 1 :*/ 0);
    return 1;
}

static int IsLowEndDevice(lua_State *lua)
{
    lua_pushboolean(lua, IsLowEndDevice() ? 1 : 0);
    return 1;
}

static int SetPaused(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TBOOLEAN);
    bool paused = lua_toboolean(lua, -1) ? true : false;
    GetGlobalGame()->SetPaused(paused);
    return 0;
}

static int IsPaused(lua_State *lua)
{
    lua_pushboolean(lua, GetGlobalGame()->IsPaused() ? 1 : 0);
    return 1;
}

static int IsHiResDevice(lua_State *lua)
{
    lua_pushboolean(lua, IsHiResDevice() ? 1 : 0);
    return 1;
}

static int Visit(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TNUMBER);
    luaL_checktype(lua, -2, LUA_TNUMBER);
    luaL_checktype(lua, -3, LUA_TNUMBER);
    luaL_checktype(lua, -4, LUA_TNUMBER);
    int x = (int)lua_tointeger(lua, -4),
        y = (int)lua_tointeger(lua, -3),
        w = (int)lua_tointeger(lua, -2),
        h = (int)lua_tointeger(lua, -1);
    GetGlobalGame()->Visit(core::Rectangle(x, y, w, h));
    return 0;
}

static int GetRenderRange(lua_State *lua)
{
    Level *level = GetGlobalGame()->GetLevel();
    core::Rectangle range(0, 0, 0, 0);
    if (level)
        range = level->GetRenderRange();
    lua_pushnumber(lua, range.x);
    lua_pushnumber(lua, range.y);
    lua_pushnumber(lua, range.width);
    lua_pushnumber(lua, range.height);
    return 4;
}

static int SetRenderPaused(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TBOOLEAN);
    bool paused = lua_toboolean(lua, -1) ? true : false;
    GetGlobalGame()->SetRenderPaused(paused);
    return 0;
}

static int IsRenderPaused(lua_State *lua)
{
    lua_pushboolean(lua, GetGlobalGame()->IsRenderPaused() ? 1 : 0);
    return 1;
}

static int SetCharacterPaused(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TBOOLEAN);
    bool paused = lua_toboolean(lua, -1) ? true : false;
    GetGlobalGame()->SetCharacterPaused(paused);
    return 0;
}

static int IsCharacterPaused(lua_State *lua)
{
    lua_pushboolean(lua, GetGlobalGame()->IsCharacterPaused() ? 1 : 0);
    return 1;
}

static int IsButtonDown(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TNUMBER); // button index
    int button = (int)lua_tointeger(lua, -1);
    lua_pushboolean(lua, buttons[button] ? 1 : 0);
    return 1;
}

static int SetVectorInput(lua_State *lua)
{
    luaL_checktype(lua, -3, LUA_TNUMBER); // x
    luaL_checktype(lua, -2, LUA_TNUMBER); // y
    luaL_checktype(lua, -1, LUA_TNUMBER); // z // we ignore this tho
    GetInput().move.SetVector(Vector2((float)lua_tonumber(lua, -2), // x and y are swapped in the accel vector
                                             (float)lua_tonumber(lua, -3)));
    /*
    accel[1] = (float)lua_tonumber(lua, -3); // x and y are swapped in the accel vector
    accel[0] = (float)lua_tonumber(lua, -2); // retarded but true
    accel[2] = (float)lua_tonumber(lua, -1);
     */
    return 0;
}

static int SetDefaultLightingType(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TNUMBER);
    SetDefaultLightingType((int)lua_tointeger(lua, -1));
    return 0;
}

static int CollideClampVector(lua_State *lua)
{
    luaL_checktype(lua, -4, LUA_TNUMBER); // mask
    luaL_checktype(lua, -3, LUA_TNUMBER); // x
    luaL_checktype(lua, -2, LUA_TNUMBER); // y
    luaL_checktype(lua, -1, LUA_TNUMBER); // z

    int   mask = (int)lua_tointeger(lua, -4);
    float x    = (float)lua_tonumber(lua, -3),
          y    = (float)lua_tonumber(lua, -2),
          z    = (float)lua_tonumber(lua, -1);

    enum { UP_MASK    =   2 |  4 |  8,
           RIGHT_MASK =   8 | 16 |  32,
           DOWN_MASK  =  32 | 64 | 128,
           LEFT_MASK  = 128 |  1 |   2 };

    if (mask & UP_MASK    && y < 0.f) y = 0.f;
    if (mask & RIGHT_MASK && x > 0.f) x = 0.f;
    if (mask & DOWN_MASK  && y > 0.f) y = 0.f;
    if (mask & LEFT_MASK  && x < 0.f) x = 0.f;

    lua_pushnumber(lua, x);
    lua_pushnumber(lua, y);
    lua_pushnumber(lua, z);

    return 3;
}

static int CollideBounceVector(lua_State *lua)
{
    luaL_checktype(lua, -4, LUA_TNUMBER); // mask
    luaL_checktype(lua, -3, LUA_TNUMBER); // x
    luaL_checktype(lua, -2, LUA_TNUMBER); // y
    luaL_checktype(lua, -1, LUA_TNUMBER); // z

    int   mask = (int)lua_tointeger(lua, -4);
    float x    = (float)lua_tonumber(lua, -3),
          y    = (float)lua_tonumber(lua, -2),
          z    = (float)lua_tonumber(lua, -1);

    enum { UP_MASK    =   2 |  4 |   8,
           RIGHT_MASK =   8 | 16 |  32,
           DOWN_MASK  =  32 | 64 | 128,
           LEFT_MASK  = 128 |  1 |   2 };

    if (mask & UP_MASK    && y < 0.f) y = -y;
    if (mask & RIGHT_MASK && x > 0.f) x = -x;
    if (mask & DOWN_MASK  && y > 0.f) y = -y;
    if (mask & LEFT_MASK  && x < 0.f) x = -x;

    lua_pushnumber(lua, x);
    lua_pushnumber(lua, y);
    lua_pushnumber(lua, z);

    return 3;
}

#include "level/LevelManager.h"
#include "level/Level.h"

static int GetVisitedData(lua_State *lua)
{
    Level *level = GetGlobalGame()->GetLevels()->GetLevel();
    int size = 0;
    const void *data = level->GetVisitedData(&size);
    lua_pushlstring(lua, (const char *)data, size);
    delete [] (const char *)data;
    return 1;
}

static int SetVisitedData(lua_State *lua)
{
    Level *level = GetGlobalGame()->GetLevels()->GetLevel();
    printf("level ptr %p 1 (SetVisitedData)\n", level);
    lua_datastring_t data = GetDataString(lua, -1);
    /*
    luaL_checktype(lua, -1, LUA_TSTRING);
    size_t a = 0, size = 0, b = 0;
    const char *data = lua_tolstring(lua, -1, (size_t *)&size);
    printf("data %s size %u\n", data, size);
    */
    printf("level ptr %p 2 (SetVisitedData)\n", level);
    level->SetVisitedData(data.str, int32_t(data.size));
    return 0;
}

static int GetSpawnablePoint(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TNUMBER);
    Level *level = GetGlobalGame()->GetLevels()->GetLevel();
    int index = (int)lua_tointeger(lua, -1);
    ReturnVector3(lua, level->GetSpawnablePoint(index));
    return 3;
}

static int GetNamedPoint(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TNUMBER);
    luaL_checktype(lua, -2, LUA_TSTRING);
    Level *level = GetGlobalGame()->GetLevels()->GetLevel();
    int         index = (int)lua_tointeger(lua, -1);
    const char *name  = lua_tostring(lua, -2);
    ReturnVector3(lua, level->GetNamedPoint(name, index));
    return 3;
}

static int SetMapTileIdAt(lua_State *lua)
{
    Level *level = GetGlobalGame()->GetLevels()->GetLevel();
    Map *map = level->GetMap(0);
    map->SetTileIdAt(v2(GetVector3(lua, -2)), GetInt(lua, -1));
    return 0;
}

static int GetMapTileIdAt(lua_State *lua)
{
    Level *level = GetGlobalGame()->GetLevels()->GetLevel();
    Map *map = level->GetMap(0);
    int id = map->GetTileIdAt(v2(GetVector3(lua, -1)));
    lua_pushinteger(lua, id);
    return 1;
}

static int GetNamedPointCount(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TSTRING);
    Level *level = GetGlobalGame()->GetLevels()->GetLevel();
    const char *name  = lua_tostring(lua, -1);
    lua_pushinteger(lua, level->GetNamedPointCount(name));
    return 1;
}

static int GetStartPoint(lua_State *lua)
{
    Level *level = GetGlobalGame()->GetLevels()->GetLevel();
    ReturnVector3(lua, level->GetStartPoint());
    return 3;
}

static Vector4 GetUiBounds()
{
    const GameDimensions &dim = GameState::GetInstance()->GetDimensions();
    Tuple3f min = dim.GetBounds().minimum,
            max = dim.GetBounds().maximum;
    return Tuple4f(min.x, min.y, max.x, max.y);
}

static Vector4 GetUiBackgroundOffsetSize()
{
    return GetUiBounds();
}

static Vector4 GetReferenceBounds()
{
    const GameDimensions &dim = GameState::GetInstance()->GetDimensions();
    Tuple3f min = dim.GetReferenceBounds().minimum,
            max = dim.GetReferenceBounds().maximum;
    return Tuple4f(min.x, min.y, max.x, max.y);
}

static int GetBackgroundOffsetSize(lua_State *lua)
{
    Vector4 sz = GetUiBackgroundOffsetSize();
    ReturnVector4(lua, sz);
    return 4;
}

static int GetBackgroundSize(lua_State *lua)
{
    Vector4 sz = GetUiBackgroundOffsetSize();
    ReturnVector2(lua, Vector2(sz.z, sz.w));
    return 2;
}

static int Lua_GetLookAt(lua_State *lua)
{
    ReturnVector3(lua, GameState::GetInstance()->GetCamera()->GetLookAt());
    return 3;
}

static int Lua_GetUiBounds(lua_State *lua)
{
    ReturnVector4(lua, GetUiBounds());
    return 4;
}

static int Lua_GetReferenceBounds(lua_State *lua)
{
    ReturnVector4(lua, GetReferenceBounds());
    return 4;
}

static int GetPlaceName(lua_State *lua)
{
    Vector3 v(GetVector3(lua, -1));
    lua_pushstring(lua, GetGlobalGame()->GetLevels()->GetPlaceName(v));
    return 1;
}

static int GetFpsAverage(lua_State *lua)
{
    GameState *state = GameState::GetInstance();
	float avfps = 0.f;
	if (state)
		avfps = state->GetFpsAverage();
	lua_pushnumber(lua, avfps);
	return 1;
}

static int ResetFpsAverage(lua_State *lua)
{
    GameState *state = GameState::GetInstance();
	if (state)
		state->ResetFpsAverage();
	return 0;
}

static int GetLevelNumber(lua_State *lua)
{
    if (!GetGlobalGame()->GetLevels()->IsLevelLoaded())
    {
        lua_pushnumber(lua, -1);
        return 1;
    }
    Level *level = GetGlobalGame()->GetLevels()->GetLevel();
    lua_pushnumber(lua, level->GetLevelNumber());
    return 1;
}

static int GetPlayerTileCenter(lua_State *lua)
{
    ReturnVector3(lua, GetGlobalGame()->GetPlayerTileCenter());
    return 3;
}

static int GetCameraFovDegrees(lua_State *lua)
{
    GameState *state = GameState::GetInstance();
    lua_pushnumber(lua, state->GetCamera()->GetFovDegrees()); // fov
    return 1;
}

static int SetPlayer(lua_State *lua)
{
    void *ud = luaL_checkudataornil(lua, -1, "D.Character");
    GetGlobalGame()->SetPlayer(!ud ? 0 : *(Character **)lua_touserdata(lua, -1));
    return 0;
}

static int GetPlayer(lua_State *lua)
{
    Character *player = GetGlobalGame()->GetPlayer();
    if (!player || !Character::IsCharacterValid(player))
        lua_pushnil(lua);
    else lua_rawgeti(lua, LUA_REGISTRYINDEX, player->GetLuaReference());
    return 1;
}

struct DeferredCall
{
    enum Deferred { LOAD_LEVEL, UNLOAD_LEVEL, RELOAD_LEVEL, DESTROY_CHARACTER, START_GAME_STATE, START_GAME, START_CRYSTAL, END_GAME };

    Deferred call;
    int      arg[4];
    void    *argp[4];
    int      defercount;

    DeferredCall() : defercount(30) {}

    bool Call()
    {
        defercount--;
        if (defercount == 0)
        {
            GameState *state = GameState::GetInstance();
            switch (call)
            {
            case START_GAME_STATE:  state->StartGameState();                          return true;
            case START_GAME:        
                {
                    GameDescription gd(arg[0]);
                    GetGlobalGame()->StartGame(gd);
                    return true;
                }
            case END_GAME:          GetGlobalGame()->EndGame();                       return true;
            case LOAD_LEVEL:        GetGlobalGame()->GetLevels()->LoadLevel(arg[0]);  return true;
            case UNLOAD_LEVEL:      GetGlobalGame()->GetLevels()->UnloadLevel();      return true;
            case RELOAD_LEVEL:      GetGlobalGame()->GetLevels()->ReloadLevel();      return true;
			case START_CRYSTAL:     /*StartCrystal();*/                               return true;
			case DESTROY_CHARACTER: DestroyCharacter((Character *)argp[0]);           return true;
            }
        }
        return false;
    }
};

std::vector<DeferredCall> deferred;

static int LoadRenderables(lua_State *lua)
{
    GameState *state = GameState::GetInstance();
    state->LoadRenderables();
	return 0;
}

static int RemoveTileSet(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TSTRING); // fov
    ObjectTileSetLoader::RemoveTileSet(lua_tostring(lua, -1));
    return 0;
}

static int StartGameSession(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TNUMBER); // level
    int level = (int)lua_tointeger(lua, -1);
    DeferredCall dc;
    dc.call = DeferredCall::START_GAME;
    dc.arg[0] = level;
    dc.defercount = 2;
    deferred.push_back(dc);
    return 0;	
}

static int StartCrystal(lua_State *lua)
{
	/*
	DeferredCall dc;
	dc.call = DeferredCall::START_CRYSTAL;
	dc.defercount = 1;
	deferred.push_back(dc);
	*/
	//StartCrystal();
    return 0;	
}

static int ActivateCrystaliPad(lua_State *lua)
{
	//ActivateCrystaliPad();
    return 0;	
}

static int DeactivateCrystaliPad(lua_State *lua)
{
	//DeactivateCrystaliPad();
    return 0;	
}

static int ShowCrystalLeaderboards(lua_State *lua)
{
	//ShowCrystalLeaderboards();
    return 0;	
}

static int ShowCrystalAchievements(lua_State *lua)
{
	//ShowCrystalAchievements();
    return 0;	
}

static int EndGameSession(lua_State *lua)
{
    DeferredCall dc;
    dc.call = DeferredCall::END_GAME;
    dc.defercount = 1;
    deferred.push_back(dc);
    return 0;	
}

static int StartGameState(lua_State *lua)
{
    DeferredCall dc;
    dc.call = DeferredCall::START_GAME_STATE;
    dc.defercount = 1;
    deferred.push_back(dc);
    return 0;	
}

void DestroyCharacterDeferred(Character *character)
{
//  Character::InvalidateCharacter(character);
    DeferredCall dc;
    dc.call = DeferredCall::DESTROY_CHARACTER;
    dc.defercount = 1;
    dc.argp[0] = character;
    deferred.push_back(dc);
}

static int DestroyCharacter(lua_State *lua)
{
    Character *instance = GetCharacter(lua);
    DestroyCharacterDeferred(instance);
    return 0;
}

static int LoadLevel(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TNUMBER);
    DeferredCall dc;
    dc.call = DeferredCall::LOAD_LEVEL;
    dc.arg[0] = (int)lua_tointeger(lua, -1);
    deferred.push_back(dc);
    //state->GetLevels()->LoadLevel();
    return 0;
}

static int UnloadLevel(lua_State *lua)
{
    DeferredCall dc;
    dc.call = DeferredCall::UNLOAD_LEVEL;
    deferred.push_back(dc);
    //state->GetLevels()->UnloadLevel();
    return 0;
}

static int ReloadLevel(lua_State *lua)
{
    DeferredCall dc;
    dc.call = DeferredCall::RELOAD_LEVEL;
    deferred.push_back(dc);
    //state->GetLevels()->ReloadLevel();
    return 0;
}

extern "C" const char *GetPlatformUniqueIdentifier();
extern "C" const char *GetPlatformDeviceName();
extern "C" const char *GetPlatformSystemName();
extern "C" const char *GetPlatformSystemVersion();
extern "C" const char *GetPlatformModel();
extern "C" const char *GetPlatformMachine();
extern "C" const char *GetPlatformLocalizedModel();

extern "C" bool        GetPlatformIsFirstGen();
extern "C" bool        GetPlatformIsSecondGen();
extern "C" bool        GetPlatformIsiPad();

static int GetPlatformUniqueIdentifier(lua_State *lua)
{
    lua_pushstring(lua, GetPlatformUniqueIdentifier());
    return 1;
}

static int GetPlatformDeviceName(lua_State *lua)
{
    lua_pushstring(lua, GetPlatformDeviceName());
    return 1;
}

static int GetPlatformSystemName(lua_State *lua)
{
    lua_pushstring(lua, GetPlatformSystemName());
    return 1;
}

static int GetPlatformSystemVersion(lua_State *lua)
{
    lua_pushstring(lua, GetPlatformSystemVersion());
    return 1;
}

static int GetPlatformModel(lua_State *lua)
{
    lua_pushstring(lua, GetPlatformModel());
    return 1;
}

static int GetPlatformMachine(lua_State *lua)
{
    lua_pushstring(lua, GetPlatformMachine());
    return 1;
}

static int GetPlatformLocalizedModel(lua_State *lua)
{
    lua_pushstring(lua, GetPlatformLocalizedModel());
    return 1;
}

static int GetPlatformIsFirstGen(lua_State *lua)
{
    lua_pushboolean(lua, GetPlatformIsFirstGen() ? 1 : 0);
    return 1;
}

static int GetPlatformIsSecondGen(lua_State *lua)
{
    lua_pushboolean(lua, GetPlatformIsSecondGen() ? 1 : 0);
    return 1;
}

static int GetPlatformIsiPad(lua_State *lua)
{
    lua_pushboolean(lua, GetPlatformIsiPad() ? 1 : 0);
    return 1;
}

void LuaFunctionsDeferredUpdate()
{
    std::vector<DeferredCall> copy;
    for (std::vector<DeferredCall>::iterator i = deferred.begin(), e = deferred.end(); i != e; ++i)
        if (!(*i).Call())
            copy.push_back(*i);
    deferred = copy;
}

static int SetCameraFovDegrees(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TNUMBER); // fov
    float fov = (float)lua_tonumber(lua, -1);
    GameState *state = GameState::GetInstance();
    state->GetCamera()->SetFovDegrees(fov);
    return 0;
}

static int SetCameraTilt(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TNUMBER); // fov
    float tilt = (float)lua_tonumber(lua, -1);
    GameState *state = GameState::GetInstance();
    state->GetCamera()->SetTilt(Vector2(0.f, tilt));
    return 0;
}

static float BASE_SCALE = 1.0f;

static int SetBaseDepthScale(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TNUMBER); // fov
    BASE_SCALE = (float)lua_tonumber(lua, -1);
    return 0;
}

static int SetDefaultMovableRadius(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TNUMBER); // fov
    Movable::SetDefaultMovableRadius((float)lua_tonumber(lua, -1));
    return 0;
}

static void SetSpeedDepth(GameState *state, const Vector3 &v, float scale)
{
    state->GetCamera()->SetDepthScale(BASE_SCALE + v.length() * scale);
}

static void SetSpeedDepthScalar(GameState *state, float length, float scale)
{
    state->GetCamera()->SetDepthScale(BASE_SCALE + length * scale);
}

static int SetSpeedDepth(lua_State *lua)
{
    luaL_checktype(lua, -4,  LUA_TNUMBER); // x
    luaL_checktype(lua, -3,  LUA_TNUMBER); // y
    luaL_checktype(lua, -2,  LUA_TNUMBER); // z

    luaL_checktype(lua, -1,  LUA_TNUMBER); // scale

    Vector3 v((float)lua_tonumber(lua, -4),
              (float)lua_tonumber(lua, -3),
              (float)lua_tonumber(lua, -2));
    GameState *state = GameState::GetInstance();
    SetSpeedDepth(state, v, (float)lua_tonumber(lua, -1));
    return 0;
}

static int IsIPhone(lua_State *lua)
{
#ifdef WIN32
    lua_pushboolean(lua, 0);
#else
    lua_pushboolean(lua, 1);
#endif
    return 1;
}

static int SetSpeedDepthScalar(lua_State *lua)
{
    luaL_checktype(lua, -2,  LUA_TNUMBER); // length
    luaL_checktype(lua, -1,  LUA_TNUMBER); // scale
    GameState *state = GameState::GetInstance();
    SetSpeedDepthScalar(state, (float)lua_tonumber(lua, -2), (float)lua_tonumber(lua, -1));
    return 0;
}

static int DrawQuad(lua_State *lua)
{
    luaL_checktype(lua, -12, LUA_TSTRING); // texture name
    luaL_checktype(lua, -11, LUA_TNUMBER); // alpha
    luaL_checktype(lua, -10, LUA_TNUMBER); // start.x
    luaL_checktype(lua, -9,  LUA_TNUMBER); // start.y
    luaL_checktype(lua, -8,  LUA_TNUMBER); // start.z

    luaL_checktype(lua, -7,  LUA_TNUMBER); // end.x 
    luaL_checktype(lua, -6,  LUA_TNUMBER); // end.y
    luaL_checktype(lua, -5,  LUA_TNUMBER); // end.z

    luaL_checktype(lua, -4,  LUA_TNUMBER); // t0.u
    luaL_checktype(lua, -3,  LUA_TNUMBER); // t0.v

    luaL_checktype(lua, -2,  LUA_TNUMBER); // t1.u
    luaL_checktype(lua, -1,  LUA_TNUMBER); // t1.v

    DeviceTexture *texture = ContentLoader::GetInstance()->GetTexture(lua_tostring(lua, -12));
    Vector4        color   = Vector4(1.0, 1.0, 1.0, (float)lua_tonumber(lua, -11));
    Vector3        start   = Vector3((float)lua_tonumber(lua, -10), (float)lua_tonumber(lua, -9), (float)lua_tonumber(lua, -8));
    Vector3        end     = Vector3((float)lua_tonumber(lua, -7),  (float)lua_tonumber(lua, -6), (float)lua_tonumber(lua, -5));
    Vector2        t0      = Vector2((float)lua_tonumber(lua, -4),  (float)lua_tonumber(lua, -3)),
                   t1      = Vector2((float)lua_tonumber(lua, -2),  (float)lua_tonumber(lua, -1));

    GameState *state = GameState::GetInstance();
    state->GetQuadRenderer().RenderScreenAlignedQuad(texture, color, start, end, t0, t1);
    return 0;
}

static int DrawQuadRotate(lua_State *lua)
{
    luaL_checktype(lua, -13, LUA_TSTRING); // texture name
    luaL_checktype(lua, -12, LUA_TNUMBER); // alpha
    luaL_checktype(lua, -11, LUA_TNUMBER); // start.x
    luaL_checktype(lua, -10, LUA_TNUMBER); // start.y
    luaL_checktype(lua, -9,  LUA_TNUMBER); // start.z

    luaL_checktype(lua, -8,  LUA_TNUMBER); // end.x 
    luaL_checktype(lua, -7,  LUA_TNUMBER); // end.y
    luaL_checktype(lua, -6,  LUA_TNUMBER); // end.z

    luaL_checktype(lua, -5,  LUA_TNUMBER); // t0.u
    luaL_checktype(lua, -4,  LUA_TNUMBER); // t0.v

    luaL_checktype(lua, -3,  LUA_TNUMBER); // t1.u
    luaL_checktype(lua, -2,  LUA_TNUMBER); // t1.v

    luaL_checktype(lua, -1,  LUA_TNUMBER); // angle

    DeviceTexture *texture = ContentLoader::GetInstance()->GetTexture(lua_tostring(lua, -13));
    Vector4        color   = Vector4(1.0, 1.0, 1.0, (float)lua_tonumber(lua, -12));
    Vector3        start   = Vector3((float)lua_tonumber(lua, -11), (float)lua_tonumber(lua, -10), (float)lua_tonumber(lua, -9));
    Vector3        end     = Vector3((float)lua_tonumber(lua, -8),  (float)lua_tonumber(lua, -7),  (float)lua_tonumber(lua, -6));
    Vector2        t0      = Vector2((float)lua_tonumber(lua, -5),  (float)lua_tonumber(lua, -4)),
                   t1      = Vector2((float)lua_tonumber(lua, -3),  (float)lua_tonumber(lua, -2));
    float          angle   = (float)lua_tonumber(lua, -1);

    GameState *state = GameState::GetInstance();
    state->GetQuadRenderer().RenderScreenAlignedQuadRotate(texture, color, start, end, t0, t1, angle);

    return 0;
}

static int Draw9Slice(lua_State *lua)
{
    luaL_checktype(lua, -10, LUA_TSTRING); // texture name
    luaL_checktype(lua, -9,  LUA_TNUMBER); // alpha
    luaL_checktype(lua, -8,  LUA_TNUMBER); // start.x
    luaL_checktype(lua, -7,  LUA_TNUMBER); // start.y
    luaL_checktype(lua, -6,  LUA_TNUMBER); // start.z

    luaL_checktype(lua, -5,  LUA_TNUMBER); // end.x 
    luaL_checktype(lua, -4,  LUA_TNUMBER); // end.y
    luaL_checktype(lua, -3,  LUA_TNUMBER); // end.z

    luaL_checktype(lua, -2,  LUA_TNUMBER); // cornerSize
    luaL_checktype(lua, -1,  LUA_TNUMBER); // middleRange 0.33 - 0.45 good values

    DeviceTexture *texture     = ContentLoader::GetInstance()->GetTexture(lua_tostring(lua, -10));
    Vector4        color       = Vector4(1.0, 1.0, 1.0, (float)lua_tonumber(lua, -9));
    Vector3        start       = Vector3((float)lua_tonumber(lua, -8), (float)lua_tonumber(lua, -7), (float)lua_tonumber(lua, -6));
    Vector3        end         = Vector3((float)lua_tonumber(lua, -5), (float)lua_tonumber(lua, -4), (float)lua_tonumber(lua, -3));
    float          cornerSize  = (float)lua_tonumber(lua, -2);
    float          middleRange = (float)lua_tonumber(lua, -1);

    GameState *state = GameState::GetInstance();
    state->GetQuadRenderer().Render9Slice(texture, color, start, end, cornerSize, middleRange);
    return 0;
}

static int SetQuad(lua_State *lua)
{
    luaL_checktype(lua, -13, LUA_TSTRING); // quad name
    luaL_checktype(lua, -12, LUA_TSTRING); // texture name

    luaL_checktype(lua, -11, LUA_TNUMBER); // alpha

    luaL_checktype(lua, -10, LUA_TNUMBER); // start.x
    luaL_checktype(lua, -9,  LUA_TNUMBER); // start.y
    luaL_checktype(lua, -8,  LUA_TNUMBER); // start.z

    luaL_checktype(lua, -7,  LUA_TNUMBER); // end.x 
    luaL_checktype(lua, -6,  LUA_TNUMBER); // end.y
    luaL_checktype(lua, -5,  LUA_TNUMBER); // end.z

    luaL_checktype(lua, -4,  LUA_TNUMBER); // t0.u
    luaL_checktype(lua, -3,  LUA_TNUMBER); // t0.v

    luaL_checktype(lua, -2,  LUA_TNUMBER); // t1.u
    luaL_checktype(lua, -1,  LUA_TNUMBER); // t1.v

    ScreenQuadToRender quad;
    quad.texture = ContentLoader::GetInstance()->GetTexture(lua_tostring(lua, -12));

    quad.color   = Vector4(1.0, 1.0, 1.0, (float)lua_tonumber(lua, -11));

    quad.start   = Vector3((float)lua_tonumber(lua, -10),
                           (float)lua_tonumber(lua, -9),
                           (float)lua_tonumber(lua, -8));

    quad.end     = Vector3((float)lua_tonumber(lua, -7),
                           (float)lua_tonumber(lua, -6),
                           (float)lua_tonumber(lua, -5));

    quad.t0      = Vector2((float)lua_tonumber(lua, -4),
                           (float)lua_tonumber(lua, -3));

    quad.t1      = Vector2((float)lua_tonumber(lua, -2),
                           (float)lua_tonumber(lua, -1));

    GameState *state = GameState::GetInstance();
    state->GetQuadRenderer().Set(lua_tostring(lua, -13), quad);
    return 0;
}

static int RemoveQuad(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TSTRING); // quad name
    GameState *state = GameState::GetInstance();
    state->GetQuadRenderer().Remove(lua_tostring(lua, -1));
    return 0;
}

static int MakeReadOnlyPath(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TSTRING); // quad name
    lua_pushstring(lua, globalTranslatePath(lua_tostring(lua, -1)));
    return 1;
}

static int MakeReadWritePath(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TSTRING); // quad name
    lua_pushstring(lua, globalTranslateReadWritePath(lua_tostring(lua, -1)));
    return 1;
}

static int GetReadOnlyPath(lua_State *lua)
{
    lua_pushstring(lua, getGlobalReadOnlyPath());
    return 1;
}

static int GetReadWritePath(lua_State *lua)
{
    lua_pushstring(lua, getGlobalReadWritePath());
    return 1;
}

// particle systems
static int CreateCloud(lua_State *lua)
{
    float           speed = GetFloat(lua, -1),
                    time  = GetFloat(lua, -2);
    Vector3         position(GetVector3(lua, -3));
    DeviceTexture  *texture = ContentLoader::GetInstance()->GetTexture(GetString(lua, -6));
    ParticleSystem *ps = ParticleEffects::GetInstance()->CreateCloud(texture, position, time, speed, false, true, false);
    lua_pushlightuserdata(lua, ps);
    return 1;
}

// particle systems
static int CreateCloudIndependent(lua_State *lua)
{
    float           speed = GetFloat(lua, -1),
                    time  = GetFloat(lua, -2);
    Vector3         position(GetVector3(lua, -3));
    DeviceTexture  *texture = ContentLoader::GetInstance()->GetTexture(GetString(lua, -6));
    ParticleSystem *ps = ParticleEffects::GetInstance()->CreateCloudIndependent(texture, position, time, speed, false, true, false);
    lua_pushlightuserdata(lua, ps);
    return 1;
}

// particle systems
static int CreatePerpetualCloud(lua_State *lua)
{
    float           speed = GetFloat(lua, -1),
                    time  = GetFloat(lua, -2);
    Vector3         position(GetVector3(lua, -3));
    DeviceTexture  *texture = ContentLoader::GetInstance()->GetTexture(GetString(lua, -6));
    ParticleSystem *ps = ParticleEffects::GetInstance()->CreateCloud(texture, position, time, speed, true, false, false);
    lua_pushlightuserdata(lua, ps);
    return 1;
}

// particle systems
static int CreatePerpetualCloudIndependent(lua_State *lua)
{
    float           speed = GetFloat(lua, -1),
                    time  = GetFloat(lua, -2);
    Vector3         position(GetVector3(lua, -3));
    DeviceTexture  *texture = ContentLoader::GetInstance()->GetTexture(GetString(lua, -6));
    ParticleSystem *ps = ParticleEffects::GetInstance()->CreateCloudIndependent(texture, position, time, speed, true, false, false);
    lua_pushlightuserdata(lua, ps);
    return 1;
}


// particle systems
static int CreateCloudLit(lua_State *lua)
{
    float           speed = GetFloat(lua, -1),
                    time  = GetFloat(lua, -2);
    Vector3         position(GetVector3(lua, -3));
    DeviceTexture  *texture = ContentLoader::GetInstance()->GetTexture(GetString(lua, -6));
    ParticleSystem *ps = ParticleEffects::GetInstance()->CreateCloud(texture, position, time, speed, false, true, true);
    lua_pushlightuserdata(lua, ps);
    return 1;
}

// particle systems
static int CreateCloudLitIndependent(lua_State *lua)
{
    float           speed = GetFloat(lua, -1),
                    time  = GetFloat(lua, -2);
    Vector3         position(GetVector3(lua, -3));
    DeviceTexture  *texture = ContentLoader::GetInstance()->GetTexture(GetString(lua, -6));
    ParticleSystem *ps = ParticleEffects::GetInstance()->CreateCloudIndependent(texture, position, time, speed, false, true, true);
    lua_pushlightuserdata(lua, ps);
    return 1;
}

// particle systems
static int CreatePerpetualCloudLit(lua_State *lua)
{
    float           speed = GetFloat(lua, -1),
                    time  = GetFloat(lua, -2);
    Vector3         position(GetVector3(lua, -3));
    DeviceTexture  *texture = ContentLoader::GetInstance()->GetTexture(GetString(lua, -6));
    ParticleSystem *ps = ParticleEffects::GetInstance()->CreateCloud(texture, position, time, speed, true, false, true);
    lua_pushlightuserdata(lua, ps);
    return 1;
}

// particle systems
static int CreatePerpetualCloudLitIndependent(lua_State *lua)
{
    float           speed = GetFloat(lua, -1),
                    time  = GetFloat(lua, -2);
    Vector3         position(GetVector3(lua, -3));
    DeviceTexture  *texture = ContentLoader::GetInstance()->GetTexture(GetString(lua, -6));
    ParticleSystem *ps = ParticleEffects::GetInstance()->CreateCloudIndependent(texture, position, time, speed, true, false, true);
    lua_pushlightuserdata(lua, ps);
    return 1;
}

// particle systems
static int CreatePerpetualCircle(lua_State *lua)
{
    float           speed = GetFloat(lua, -1),
                    time  = GetFloat(lua, -2);
    Vector3         position(GetVector3(lua, -3));
    DeviceTexture  *texture = ContentLoader::GetInstance()->GetTexture(GetString(lua, -6));
    ParticleSystem *ps = ParticleEffects::GetInstance()->CreateCircle(texture, position, time, speed, true, false);
    lua_pushlightuserdata(lua, ps);
    return 1;
}

static int CreateCircleCloud(lua_State *lua)
{
    float           speed = GetFloat(lua, -1),
                    time  = GetFloat(lua, -2);
    Vector3         position(GetVector3(lua, -3));
    DeviceTexture  *texture = ContentLoader::GetInstance()->GetTexture(GetString(lua, -6));
    ParticleSystem *ps = ParticleEffects::GetInstance()->CreateCircle(texture, position, time, speed, false, true);
    lua_pushlightuserdata(lua, ps);
    return 1;
}

static int CreateCollisionTest(lua_State *lua)
{
    float           speed = GetFloat(lua, -1),
                    time  = GetFloat(lua, -2);
    Vector3         position(GetVector3(lua, -3));
    DeviceTexture  *texture = ContentLoader::GetInstance()->GetTexture(GetString(lua, -6));
    ParticleSystem *ps = ParticleEffects::GetInstance()->CreateCollisionTest(texture, position, time, speed);                         
    lua_pushlightuserdata(lua, ps);
    return 1;
}

static int Effect_SetPosition(lua_State *lua)
{
    luaL_checktype(lua, -4, LUA_TLIGHTUSERDATA);
    ParticleSystem *ps = (ParticleSystem *)lua_touserdata(lua, -4);
    Vector3 position(GetVector3(lua, -1));
    ps->SetPosition(position);
    return 0;
}

static int Effect_Destroy(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TLIGHTUSERDATA);
    ParticleSystem *ps = (ParticleSystem *)lua_touserdata(lua, -1);
    ParticleEffects::GetInstance()->DeferredDestroy(ps);
    return 0;
}

static int Darken(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TNUMBER);
    GameState *state = GameState::GetInstance();
    state->GetEffects().Darken(static_cast<int>(lua_tointeger(lua, -1)));
    return 0;
}

static int UnDarken(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TNUMBER);
    GameState *state = GameState::GetInstance();
    state->GetEffects().UnDarken(static_cast<int>(lua_tointeger(lua, -1)));
    return 0;
}

static int Lighten(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TNUMBER);
    GameState *state = GameState::GetInstance();
    state->GetEffects().Lighten(static_cast<int>(lua_tointeger(lua, -1)));
    return 0;
}

static int UnLighten(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TNUMBER);
    GameState *state = GameState::GetInstance();
    state->GetEffects().UnLighten(static_cast<int>(lua_tointeger(lua, -1)));
    return 0;
}

static int Flash(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TNUMBER);
    GameState *state = GameState::GetInstance();
    state->GetEffects().Flash(static_cast<int>(lua_tointeger(lua, -1)));
    return 0;
}

static int GetPlayerTileEventCount(lua_State *lua)
{
    lua_pushinteger(lua, GetGlobalGame()->GetLevel()->GetTileEventCountAt(GetGlobalGame()->GetPlayer()->GetPosition()));
    return 1;
}

static int GetMovableCount(lua_State *lua)
{
    lua_pushinteger(lua, GetGlobalGame()->GetLevel()->GetMovableCount());
    return 1;
}

static int GetPlayerTileEvent(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TNUMBER);
    int index = static_cast<int>(lua_tointeger(lua, -1));
    lua_pushstring(lua, GetGlobalGame()->GetLevel()->GetTileEventAt(GetGlobalGame()->GetPlayer()->GetPosition(), index));
    return 1;
}

static int HasPlayerTileEvent(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TSTRING);
    const char *prefix = lua_tostring(lua, -1);
    Character *player = GetGlobalGame()->GetPlayer();
    if (!Character::IsCharacterValid(player))
        lua_pushboolean(lua, false);
    else
        lua_pushboolean(lua, GetGlobalGame()->GetLevel()->HasTileEventAt(prefix, GetGlobalGame()->GetPlayer()->GetPosition()) != 0);
    return 1;
}

static int Multiplayer_StartENetServer(lua_State *lua)
{
    luaL_checktype(lua, -2, LUA_TSTRING);
    luaL_checktype(lua, -1, LUA_TNUMBER);
    const char *name = lua_tostring(lua, -2);
    int         port = (int)lua_tointeger(lua, -1);
    bool success = GetGlobalGame()->GetMultiplayer().StartENetServer(name, port);
    lua_pushboolean(lua, success ? 1 : 0);
    return 1;
}

static int Multiplayer_StartENetClient(lua_State *lua)
{
    luaL_checktype(lua, -3, LUA_TSTRING);
    luaL_checktype(lua, -2, LUA_TSTRING);
    luaL_checktype(lua, -1, LUA_TNUMBER);
    const char *name    = lua_tostring(lua, -3);
    const char *address = lua_tostring(lua, -2);
    int         port    = (int)lua_tointeger(lua, -1);
    bool success = GetGlobalGame()->GetMultiplayer().StartENetClient(name, address, port);
    lua_pushboolean(lua, success ? 1 : 0);
    return 1;
}

static int Multiplayer_StartBluetoothServer(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TSTRING);
    const char *name = lua_tostring(lua, -1);
    bool success = GetGlobalGame()->GetMultiplayer().StartBluetoothServer(name);
    lua_pushboolean(lua, success ? 1 : 0);
    return 1;
}

static int Multiplayer_StartBluetoothClient(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TSTRING);
    const char *name = lua_tostring(lua, -1);
    bool success = GetGlobalGame()->GetMultiplayer().StartBluetoothClient(name);
    lua_pushboolean(lua, success ? 1 : 0);
    return 1;
}

static int Multiplayer_SetProfileName(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TSTRING);
    GetGlobalGame()->GetMultiplayer().SetProfileName(lua_tostring(lua, -1));
    return 0;
}

static int Multiplayer_StartServer(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TSTRING);
    lua_pushboolean(lua, GetGlobalGame()->GetMultiplayer().StartServer(NETGAME_PORT, lua_tostring(lua, -1)) ? 1 : 0);
    return 1;
}

static int Multiplayer_StartClient(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TSTRING);
    lua_pushboolean(lua, GetGlobalGame()->GetMultiplayer().StartClient(lua_tostring(lua, -1)) ? 1 : 0);
    return 1;
}

static int Multiplayer_Join(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TSTRING);
    lua_pushboolean(lua, GetGlobalGame()->GetMultiplayer().Join(lua_tostring(lua, -1), NETGAME_PORT) ? 1 : 0);
    return 1;
}

static int Multiplayer_IsJoined(lua_State *lua)
{
    lua_pushboolean(lua, GetGlobalGame()->GetMultiplayer().IsJoined());
    return 1;
}

static Connection::ConnectionManager *GetConnectionManager()
{
    return GetGlobalGame()->GetMultiplayer().GetConnectionManager();
}

static int Multiplayer_IsClient(lua_State *lua)
{
    lua_pushboolean(lua, GetConnectionManager()->IsServer() ? 0 : 1);
    return 1;
}

static int Multiplayer_IsServer(lua_State *lua)
{
    lua_pushboolean(lua, GetConnectionManager()->IsServer() ? 1 : 0);
    return 1;
}

static int Multiplayer_GetConnectionCount(lua_State *lua)
{
    Connection::ConnectionManager *cm = GetConnectionManager();
    int count = 0;
    if (cm) count = GetConnectionManager()->GetConnectionCount();
    lua_pushinteger(lua, count);
    return 1;
}

static int Multiplayer_IsLosingConnection(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TNUMBER);
    int cid = (int)lua_tointeger(lua, -1);
    lua_pushboolean(lua, GetConnectionManager()->IsLosingConnection(cid) ? 1 : 0);
    return 1;
}

static int Multiplayer_Close(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TNUMBER);
    int cid = (int)lua_tointeger(lua, -1);
    lua_pushboolean(lua, GetConnectionManager()->CloseById(cid) ? 1 : 0);
    return 1;
}

static int Multiplayer_GetIndexById(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TNUMBER);
    int cid = (int)lua_tointeger(lua, -1);
    lua_pushinteger(lua, GetConnectionManager()->GetIndexById(cid));
    return 1;
}

static int Multiplayer_GetIdByIndex(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TNUMBER);
    int cid = (int)lua_tointeger(lua, -1);
    lua_pushinteger(lua, GetConnectionManager()->GetIdByIndex(cid));
    return 1;
}

static int Multiplayer_ChangeId(lua_State *lua)
{
    luaL_checktype(lua, -2, LUA_TNUMBER);
    luaL_checktype(lua, -1, LUA_TNUMBER);
    int oldid = (int)lua_tointeger(lua, -2),
        newid = (int)lua_tointeger(lua, -1);
    lua_pushboolean(lua, GetConnectionManager()->ChangeId(oldid, newid));
    return 1;
}

/*
static int Multiplayer_GetId(lua_State *lua)
{
    lua_pushinteger(lua, GetGlobalGame()->GetMultiplayer().GetId());
    return 1;
}
*/

static int Sync_SendLuaMessage(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TSTRING);
    size_t length = 0;
    const char *str = lua_tolstring(lua, -1, &length);
    Multiplayer &multiplayer = GetGlobalGame()->GetMultiplayer();
    multiplayer.SendLuaMessage(str, static_cast<int>(length));
    return 0;
}

static int Sync_SendDataMessage(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TSTRING);
    size_t length = 0;
    const char *str = lua_tolstring(lua, -1, &length);
    Multiplayer &multiplayer = GetGlobalGame()->GetMultiplayer();
    multiplayer.SendDataMessage(str, static_cast<int>(length), false, false);
    return 0;
}

static int Sync_HasMessage(lua_State *lua, Multiplayer::Queue queue)
{
    Multiplayer &multiplayer = GetGlobalGame()->GetMultiplayer();
    lua_pushboolean(lua, multiplayer.HasMessage(queue));
    return 1;
}

static int Sync_RemoveMessage(lua_State *lua, Multiplayer::Queue queue)
{
    Multiplayer &multiplayer = GetGlobalGame()->GetMultiplayer();
    multiplayer.RemoveMessage(queue);
    return 0;
}

static int Sync_ClearMessages(lua_State *lua, Multiplayer::Queue queue)
{
    Multiplayer &multiplayer = GetGlobalGame()->GetMultiplayer();
    multiplayer.ClearMessages(queue);
    return 0;
}

static int Sync_NextMessage(lua_State *lua, Multiplayer::Queue queue)
{
    Multiplayer &multiplayer = GetGlobalGame()->GetMultiplayer();
    const std::string &s = multiplayer.NextMessage(queue);
    lua_pushlstring(lua, s.data(), s.size());
    return 1;
}

static int Sync_PopMessage(lua_State *lua, Multiplayer::Queue queue)
{
    Multiplayer &multiplayer = GetGlobalGame()->GetMultiplayer();
    const std::string &s = multiplayer.NextMessage(queue);
    lua_pushlstring(lua, s.data(), s.size());
    multiplayer.RemoveMessage(queue);
    return 1;
}

static int Sync_HasLuaMessage(lua_State *lua)     { return Sync_HasMessage(lua, Multiplayer::LUA_MESSAGE_QUEUE); }
static int Sync_HasDataMessage(lua_State *lua)    { return Sync_HasMessage(lua, Multiplayer::DATA_MESSAGE_QUEUE); }
static int Sync_RemoveLuaMessage(lua_State *lua)  { return Sync_RemoveMessage(lua, Multiplayer::LUA_MESSAGE_QUEUE); }
static int Sync_RemoveDataMessage(lua_State *lua) { return Sync_RemoveMessage(lua, Multiplayer::DATA_MESSAGE_QUEUE); }
static int Sync_ClearLuaMessages(lua_State *lua)  { return Sync_ClearMessages(lua, Multiplayer::LUA_MESSAGE_QUEUE); }
static int Sync_ClearDataMessages(lua_State *lua) { return Sync_ClearMessages(lua, Multiplayer::DATA_MESSAGE_QUEUE); }
static int Sync_NextLuaMessage(lua_State *lua)    { return Sync_NextMessage(lua, Multiplayer::LUA_MESSAGE_QUEUE); }
static int Sync_NextDataMessage(lua_State *lua)   { return Sync_NextMessage(lua, Multiplayer::DATA_MESSAGE_QUEUE); }
static int Sync_PopLuaMessage(lua_State *lua)     { return Sync_PopMessage(lua, Multiplayer::LUA_MESSAGE_QUEUE); }
static int Sync_PopDataMessage(lua_State *lua)    { return Sync_PopMessage(lua, Multiplayer::DATA_MESSAGE_QUEUE); }

static int GetTime(lua_State *lua)
{
    lua_pushnumber(lua, GetCurrentTimeMs());
    return 1;
}

static int PrintMemoryStatistics(lua_State *lua)
{
    _gAllocator.PrintStatistics();
    return 0;
}

static int DumpMemoryStatistics(lua_State *lua)
{
    _gAllocator.DumpStatistics();
    return 0;
}

static int SetOverlayColor(lua_State *lua)
{
    GameState *state = GameState::GetInstance();
    Vector4 color = GetVector4(lua, -1);
    state->SetOverlayColor(color);   
    return 0;
}

static int GetOverlayColor(lua_State *lua)
{
    GameState *state = GameState::GetInstance();
    ReturnVector4(lua, state->GetOverlayColor());
    return 3;
}

static GameState::Pass CheckType_LuxPass(lua_State *lua, int arg)
{
    luaL_checktype(lua, arg, LUA_TNUMBER);  // pass
    int passi = (int)lua_tointeger(lua, arg);
    if (passi >= (int)GameState::PASS_COUNT) {
        char buffer[1024];
        snprintf(buffer, sizeof(buffer) - 1, "lux - pass (%d) is not a valid value", passi);
        LuaReportError(buffer);
        return GameState::PASS_COUNT;
    }
    return (GameState::Pass)passi;
}

static int CheckType_LuxLightId(lua_State *lua, int arg)
{
    luaL_checktype(lua, arg, LUA_TNUMBER);
    int id = (int)lua_tointeger(lua, arg);
    if (id < 0) {
        char buffer[1024];
        snprintf(buffer, sizeof(buffer) - 1, "lux - light id (%d) is not a valid value", id);
        LuaReportError(buffer);
        return GameState::PASS_COUNT;
    }
    return id;
}

static int Lua_LuxEnableLighting(lua_State *lua)
{
    GameState::Pass pass = CheckType_LuxPass(lua, -2);
    luaL_checktype(lua, -1, LUA_TBOOLEAN); // enable
    bool enable = (lua_toboolean(lua, -1) != 0);
    if (pass < GameState::PASS_COUNT)
        GameState::GetInstance()->GetLights(pass).Enable(enable);
    return 0;
}

static int Lua_LuxEnable(lua_State *lua)
{
    GameState::Pass pass   = CheckType_LuxPass(lua, -3);
    int             id     = CheckType_LuxLightId(lua, -2);
    luaL_checktype(lua, -1, LUA_TBOOLEAN); // enable
    bool            enable = (lua_toboolean(lua, -1) != 0);
    if (pass < GameState::PASS_COUNT && id >= 0) {
        GameState::GetInstance()->GetLights(pass).Enable(size_t(id), enable);
    }
    return 0;
}

static int Lua_LuxSetVector(lua_State *lua, Light::VectorType type)
{
    GameState::Pass pass = CheckType_LuxPass(lua, -6);
    size_t          id   = CheckType_LuxLightId(lua, -5);
    Vector4 v(GetVector4(lua, -1));
    if (pass < GameState::PASS_COUNT && id >= 0) {
        GameState::GetInstance()->GetLights(pass).SetVector(size_t(id), type, v);
    }
    return 0;
}

static int Lua_LuxGetVector(lua_State *lua, Light::VectorType type)
{
    GameState::Pass pass = CheckType_LuxPass(lua, -2);
    size_t          id   = CheckType_LuxLightId(lua, -1);
    Vector4 v;
    v.set(0.f);
    if (pass < GameState::PASS_COUNT && id >= 0) {
        const Vector4 *p = GameState::GetInstance()->GetLights(pass).GetVector(size_t(id), type);
        v = p ? *p : v;
    }
    ReturnVector4(lua, v);
    return 4;
}

static int Lua_LuxSetFloat(lua_State *lua, Light::FloatType type)
{
    GameState::Pass pass  = CheckType_LuxPass(lua, -3);
    size_t          id    = CheckType_LuxLightId(lua, -2);
    float           value = GetFloat(lua, -1);
    if (pass < GameState::PASS_COUNT && id >= 0) {
        GameState::GetInstance()->GetLights(pass).SetFloat(size_t(id), type, value);
    }
    return 0;
}

static int Lua_LuxGetFloat(lua_State *lua, Light::FloatType type)
{
    GameState::Pass pass  = CheckType_LuxPass(lua, -2);
    size_t          id    = CheckType_LuxLightId(lua, -1);
    float           value = 0.f;
    if (pass < GameState::PASS_COUNT && id >= 0) {
        const float *p = GameState::GetInstance()->GetLights(pass).GetFloat(size_t(id), type);
        value = p ? *p : 0.f;
    }
    lua_pushnumber(lua, value);
    return 1;
}

static int Lua_LuxGetWorldPosition(lua_State *lua)        { return Lua_LuxGetVector(lua, Light::V4_WORLD_POSITION); }
static int Lua_LuxGetAmbient(lua_State *lua)              { return Lua_LuxGetVector(lua, Light::V4_AMBIENT); }
static int Lua_LuxGetDiffuse(lua_State *lua)              { return Lua_LuxGetVector(lua, Light::V4_DIFFUSE); }
static int Lua_LuxGetSpecular(lua_State *lua)             { return Lua_LuxGetVector(lua, Light::V4_SPECULAR); }
static int Lua_LuxGetSpotDirection(lua_State *lua)        { return Lua_LuxGetVector(lua, Light::V4_SPOT_DIRECTION); }

static int Lua_LuxGetShininess(lua_State *lua)            { return Lua_LuxGetFloat (lua, Light::F1_SHININESS); }
static int Lua_LuxGetSpotExponent(lua_State *lua)         { return Lua_LuxGetFloat (lua, Light::F1_SPOT_EXPONENT); }
static int Lua_LuxGetSpotCutoff(lua_State *lua)           { return Lua_LuxGetFloat (lua, Light::F1_SPOT_CUTOFF); }
static int Lua_LuxGetConstantAttenuation(lua_State *lua)  { return Lua_LuxGetFloat (lua, Light::F1_CONSTANT_ATTENUATION); }
static int Lua_LuxGetLinearAttenuation(lua_State *lua)    { return Lua_LuxGetFloat (lua, Light::F1_LINEAR_ATTENUATION); }
static int Lua_LuxGetQuadraticAttenuation(lua_State *lua) { return Lua_LuxGetFloat (lua, Light::F1_QUADRATIC_ATTENUATION); }

static int Lua_LuxSetWorldPosition(lua_State *lua)        { return Lua_LuxSetVector(lua, Light::V4_WORLD_POSITION); }
static int Lua_LuxSetAmbient(lua_State *lua)              { return Lua_LuxSetVector(lua, Light::V4_AMBIENT); }
static int Lua_LuxSetDiffuse(lua_State *lua)              { return Lua_LuxSetVector(lua, Light::V4_DIFFUSE); }
static int Lua_LuxSetSpecular(lua_State *lua)             { return Lua_LuxSetVector(lua, Light::V4_SPECULAR); }
static int Lua_LuxSetSpotDirection(lua_State *lua)        { return Lua_LuxSetVector(lua, Light::V4_SPOT_DIRECTION); }

static int Lua_LuxSetShininess(lua_State *lua)            { return Lua_LuxSetFloat (lua, Light::F1_SHININESS); }
static int Lua_LuxSetSpotExponent(lua_State *lua)         { return Lua_LuxSetFloat (lua, Light::F1_SPOT_EXPONENT); }
static int Lua_LuxSetSpotCutoff(lua_State *lua)           { return Lua_LuxSetFloat (lua, Light::F1_SPOT_CUTOFF); }
static int Lua_LuxSetConstantAttenuation(lua_State *lua)  { return Lua_LuxSetFloat (lua, Light::F1_CONSTANT_ATTENUATION); }
static int Lua_LuxSetLinearAttenuation(lua_State *lua)    { return Lua_LuxSetFloat (lua, Light::F1_LINEAR_ATTENUATION); }
static int Lua_LuxSetQuadraticAttenuation(lua_State *lua) { return Lua_LuxSetFloat (lua, Light::F1_QUADRATIC_ATTENUATION); }

static int SetBaseUvTransform(lua_State *lua)
{
    Matrix ma, mb;
    ma.scale(Vector3(1.f, -1.f, 1.f));
    mb.translate(Vector3(0.f, 1.f, 0.f));
    ma = ma * mb;
    GetDevice()->SetBaseUvTransform(ma);
	return 0;
}

static int PrintLoadedTextures(lua_State *lua)
{
	GameState::PrintLoadedTextures();
	return 0;
}

static int UnloadAllTextures(lua_State *lua)
{
	ContentLoader::GetInstance()->UnloadAllTextures();
	return 0;
}

static int RemoveAllTileSets(lua_State *lua)
{
	ObjectTileSetLoader::RemoveAllTileSets();
	return 0;
}

static int UnsetBaseUvTransform(lua_State *lua)
{
    Matrix identity;
    GetDevice()->SetBaseUvTransform(identity);
	return 0;
}

static int ParseNumbers(lua_State *lua)
{
    luaL_checktype(lua, -3, LUA_TSTRING); // the string to parse    
    luaL_checktype(lua, -2, LUA_TNUMBER); // count of numbers to parse    
    luaL_checktype(lua, -1, LUA_TNUMBER); // start read at index
    
    float       values[256]; // no more than 256
    int         start = (int)lua_tointeger(lua, -1);
    int         count = (int)lua_tointeger(lua, -2);
    const char *str   = lua_tostring (lua, -3);
    memset(values, 0, sizeof(values));
    const char *pstr = str + start;
    if ((int)strlen(str) <= start || !read_array(&pstr, values, count))
    {
        for (int i = 0; i < count; i++)
            lua_pushnil(lua);
    }
    else
    {
        for (int i = 0; i < count; i++)
            lua_pushnumber(lua, values[i]);
    }
    return count;
}

static int StartsWith(lua_State *lua)
{
    luaL_checktype(lua, -2, LUA_TSTRING); // string
    luaL_checktype(lua, -1, LUA_TSTRING); // start string
    bool result = starts(lua_tostring(lua, -2), lua_tostring(lua, -1));
    lua_pushboolean(lua, result ? 1 : 0);
    return 1;
}

static int EndsWith(lua_State *lua)
{
    luaL_checktype(lua, -2, LUA_TSTRING); // string
    luaL_checktype(lua, -1, LUA_TSTRING); // start string
    bool result = ends(lua_tostring(lua, -2), lua_tostring(lua, -1));
    lua_pushboolean(lua, result ? 1 : 0);
    return 1;    
}

static int Trim(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TSTRING); // string to trim
    char *s = trim(lua_tostring(lua, -1));
    lua_pushstring(lua, s);
    delete [] s;
    return 1;    
}

static int TrimOf(lua_State *lua)
{
    luaL_checktype(lua, -2, LUA_TSTRING); // string    
    luaL_checktype(lua, -1, LUA_TSTRING); // of these characters
    char *s = trim(lua_tostring(lua, -2), lua_tostring(lua, -1));
    lua_pushstring(lua, s);
    delete [] s;    
    return 1;
}

static int GetFreeMemory(lua_State *lua)
{
    unsigned bytes = GetFreeMemory();
    lua_pushnumber(lua, (double)bytes);
    return 1;		
}

static int FreeMemory(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TNUMBER);
    int required = (int)lua_tointeger(lua, -1);
    bool succeeded = FreeMemory(required);
    lua_pushboolean(lua, succeeded ? 1 : 0);
    return 1;
}

static int ReceivedLowMemoryWarning(lua_State *lua)
{
    lua_pushboolean(lua, ReceivedLowMemoryWarning() ? 1 : 0);
    return 1;
}

static int Math_Log2(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TNUMBER);    
    double value = (double)lua_tonumber(lua, -1);
    static const double log2 = 1.0 / log(2.0);
    lua_pushnumber(lua, log(value) * log2);
    return 1;
}

static int HexColor(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TNUMBER);
    luaL_checktype(lua, -2, LUA_TNUMBER);
    luaL_checktype(lua, -3, LUA_TNUMBER);
    double r = (double)lua_tonumber(lua, -3),
           g = (double)lua_tonumber(lua, -2),
           b = (double)lua_tonumber(lua, -1);
    char color[16];
    snprintf(color, sizeof(color) - 1, "%02x%02x%02x", (int)(r * 255.f), (int)(g * 255.f), (int)(b * 255.f));
    lua_pushstring(lua, color);
    return 1;
}

static int HexColorAlpha(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TNUMBER);
    luaL_checktype(lua, -2, LUA_TNUMBER);
    luaL_checktype(lua, -3, LUA_TNUMBER);
    luaL_checktype(lua, -4, LUA_TNUMBER);
    double r = (double)lua_tonumber(lua, -4),
           g = (double)lua_tonumber(lua, -3),
           b = (double)lua_tonumber(lua, -2),
           a = (double)lua_tonumber(lua, -1);
    char color[16];
    snprintf(color, sizeof(color) - 1, "%02x%02x%02x%02x", (int)(r * 255.f), (int)(g * 255.f), (int)(b * 255.f), (int)(a * 255.f));
    lua_pushstring(lua, color);
    return 1;
}

class LuaRandom // this is just a wrapper for core::Random that uses CLASS_NEW_DELETE()
{
public:

    CLASS_NEW_DELETE()

    core::Random random;

    inline LuaRandom(int seed) : random(seed) {}
};

static int Random_NextInt(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TNUMBER);
    int seed = (int)lua_tointeger(lua, -1);
    LuaRandom lr(seed);
    lua_pushinteger(lua, lr.random.integer());
    return 1;
}

static int Random_NextReal(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TNUMBER);
    int seed = (int)lua_tointeger(lua, -1);
    LuaRandom lr(seed);
    lua_pushnumber(lua, lr.random.real());
    return 1;
}

static int Random_Create(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TNUMBER);
    int seed = (int)lua_tointeger(lua, -1);
    lua_pushlightuserdata(lua, new LuaRandom(seed));
    return 1;
}

static int Random_Integer(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TLIGHTUSERDATA);
    LuaRandom *rng = reinterpret_cast<LuaRandom *>(lua_touserdata(lua, -1));
    if (rng) lua_pushinteger(lua, rng->random.integer());
    else     lua_pushnil(lua);    
    return 1;
}

static int Random_Real(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TLIGHTUSERDATA);
    LuaRandom *rng = reinterpret_cast<LuaRandom *>(lua_touserdata(lua, -1));
    if (rng) lua_pushnumber(lua, rng->random.real());
    else     lua_pushnil(lua);
    return 1;
}

static int Random_RealRange(lua_State *lua)
{
    luaL_checktype(lua, -3, LUA_TLIGHTUSERDATA);
    luaL_checktype(lua, -2, LUA_TNUMBER);
    luaL_checktype(lua, -1, LUA_TNUMBER);        
    LuaRandom *rng = reinterpret_cast<LuaRandom *>(lua_touserdata(lua, -3));
    if (rng)
    {
        double lo = (double)lua_tonumber(lua, -2),
               hi = (double)lua_tonumber(lua, -1);
        lua_pushnumber(lua, rng->random.real() * (hi - lo) + lo);
    }
    else lua_pushnil(lua);
    return 1;
}

static int Random_IntegerRange(lua_State *lua)
{
    luaL_checktype(lua, -3, LUA_TLIGHTUSERDATA);
    luaL_checktype(lua, -2, LUA_TNUMBER);
    luaL_checktype(lua, -1, LUA_TNUMBER);        
    LuaRandom *rng = reinterpret_cast<LuaRandom *>(lua_touserdata(lua, -3));
    if (rng)
    {
        int lo = (int)lua_tointeger(lua, -2),
            hi = (int)lua_tointeger(lua, -1);
        lua_pushnumber(lua, (rng->random.integer() * (hi - lo)) / core::Random::MAX_RANDOM + lo);
    }
    else lua_pushnil(lua);
    return 1;
}
    
static int Random_Destroy(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TLIGHTUSERDATA);
    LuaRandom *rng = reinterpret_cast<LuaRandom *>(lua_touserdata(lua, -1));
    if (rng) delete rng;
    return 0;
}

typedef void (*OnCancelCallback)(void *user);
typedef void (*OnPickerConnectCallback)(void *user);

extern "C" void SetBluetoothOnReceiveCallback(Connection::OnReceiveCallback callback, void *user);
extern "C" void SetBluetoothOnCancelCallback(OnCancelCallback callback, void *user);
extern "C" void SetBluetoothOnPickerConnectCallback(OnPickerConnectCallback callback, void *user);

extern "C" void StartBluetoothPicker();
extern "C" void StartWiFiPicker();
extern "C" void StartBonjour();

static int Lua_StartBluetoothPicker(lua_State *lua)
{
// $TODO currently disabled
//  SetBluetoothOnPickerConnectCallback(OnReceiveBluetoothConnectionLua, 0);
//  StartBluetoothPicker();
    return 0;
}

static int Lua_StartWiFiPicker(lua_State *lua)
{
// $TODO currently disabled	
//  StartWiFiPicker();
    return 0;
}

static int Lua_StartBonjour(lua_State *lua)
{
// $TODO currently disabled	
//  StartBonjour();
    return 0;
}

extern bool HasLuaErrorOccurred();

static int Lua_HasLuaErrorOccurred(lua_State *lua)
{
    lua_pushboolean(lua, HasLuaErrorOccurred() ? 1 : 0);
    return 1;
}

static int Lua_Compress(lua_State *lua)
{
    char buffer[Codec::MAX_DECODE_SIZE * 2];
    size_t insize = 0;
    luaL_checktype(lua, -1, LUA_TSTRING);
    const char *data = lua_tolstring(lua, -1, &insize);
    size_t outsize = Codec::Encode(data, insize, buffer);
    lua_pushlstring(lua, buffer, outsize);
    return 1;
}

static int Lua_Decompress(lua_State *lua)
{
    char buffer[Codec::MAX_DECODE_SIZE * 2];
    size_t insize = 0;
    luaL_checktype(lua, -1, LUA_TSTRING);
    const char *data = lua_tolstring(lua, -1, &insize); // we actually don't need insize here..
    size_t outsize = Codec::Decode(data, buffer);
    lua_pushlstring(lua, buffer, outsize);
    return 1;
}

void RegisterLuaFunctions(lua_State *lua)
{
    RegisterLuaMathFunctions(lua);
    Serialize::RegisterLuaSerializeFunctions(lua);
    RegisterLuaEventFunctions(lua);
    RegisterLuaUiFunctions(lua);

    lua_register(lua, "SetIsLowEndDevice",             SetIsLowEndDevice);
    lua_register(lua, "IsLowEndDevice",                IsLowEndDevice);
    lua_register(lua, "SendEmail",                     SendEmail);
	lua_register(lua, "SetAnimationFrameInterval",     SetAnimationFrameInterval);
	lua_register(lua, "ShouldCrystalActivate",         ShouldCrystalActivate);

    lua_register(lua, "PostAchievement",               PostAchievement);	
    lua_register(lua, "PostLeaderboard",               PostLeaderboard);	
	
    lua_register(lua, "breakpoint",                    LuaBreakpoint);
    lua_register(lua, "HasLuaErrorOccurred",           Lua_HasLuaErrorOccurred);

    lua_register(lua, "Compress",                      Lua_Compress);
    lua_register(lua, "Decompress",                    Lua_Decompress);

    lua_register(lua, "Visit",                         Visit);
    lua_register(lua, "GetRenderRange",                GetRenderRange);

    lua_register(lua, "Math_Log2",                     Math_Log2);
    lua_register(lua, "HexColor",                      HexColor);
    lua_register(lua, "HexColorAlpha",                 HexColorAlpha);
	
	lua_register(lua, "PrintLoadedTextures",           PrintLoadedTextures);
	lua_register(lua, "UnloadAllTextures",             UnloadAllTextures);
	lua_register(lua, "RemoveAllTileSets",             RemoveAllTileSets);		

    lua_register(lua, "Random_Create",                 Random_Create);
    lua_register(lua, "Random_Integer",                Random_Integer);
    lua_register(lua, "Random_Real",                   Random_Real);
    lua_register(lua, "Random_IntegerRange",           Random_IntegerRange);
    lua_register(lua, "Random_RealRange",              Random_RealRange);
    lua_register(lua, "Random_Destroy",                Random_Destroy);
    lua_register(lua, "Random_NextInt",                Random_NextInt);
    lua_register(lua, "Random_NextReal",               Random_NextReal);

    lua_register(lua, "StartBluetoothPicker",          Lua_StartBluetoothPicker);  	
    lua_register(lua, "StartWiFiPicker",               Lua_StartWiFiPicker);
	
    lua_register(lua, "StartBonjour",                  Lua_StartBonjour);	

    lua_register(lua, "Audio_StopAmbient",             Audio_StopAmbient);
    lua_register(lua, "Audio_StartAmbient",            Audio_StartAmbient);
    lua_register(lua, "Audio_SetAmbientVolume",        Audio_SetAmbientVolume);
    lua_register(lua, "Audio_Load",                    Audio_Load);
    lua_register(lua, "Audio_Play",                    Audio_Play);
    lua_register(lua, "Audio_PlayAt",                  Audio_PlayAt);
    lua_register(lua, "Audio_PlayTheme",               Audio_PlayTheme);
    lua_register(lua, "Audio_StopTheme",               Audio_StopTheme);
    lua_register(lua, "Audio_SetListenerPosition",     Audio_SetListenerPosition);

    lua_register(lua, "Trim",                          Trim);
    lua_register(lua, "TrimOf",                        TrimOf);
    lua_register(lua, "StartsWith",                    StartsWith);
    lua_register(lua, "EndsWith",                      EndsWith);

    lua_register(lua, "GetGameFrames",                 GetGameFrames);
    lua_register(lua, "GetGameStateFrames",            GetGameStateFrames);
	lua_register(lua, "GetFpsAverage",                 GetFpsAverage);
	lua_register(lua, "ResetFpsAverage",               ResetFpsAverage);	

    lua_register(lua, "Pack",                          LuaPack);
    lua_register(lua, "Unpack",                        LuaUnpack);
    lua_register(lua, "CompileScript",                 LuaCompileScript);
    lua_register(lua, "ParseNumbers",                  ParseNumbers);

    lua_register(lua, "GetVisitedData",                GetVisitedData);
    lua_register(lua, "SetVisitedData",                SetVisitedData);

    lua_register(lua, "PrintMemoryStatistics",         PrintMemoryStatistics);
    lua_register(lua, "DumpMemoryStatistics",          DumpMemoryStatistics);

    lua_register(lua, "GetMovableCount",               GetMovableCount);
    
    lua_register(lua, "RemoveTileSet",                 RemoveTileSet);
    lua_register(lua, "GetPlayerTileCenter",           GetPlayerTileCenter);

    lua_register(lua, "GetPlayerTileEventCount",       GetPlayerTileEventCount);
    lua_register(lua, "GetPlayerTileEvent",            GetPlayerTileEvent);
    lua_register(lua, "HasPlayerTileEvent",            HasPlayerTileEvent);

    lua_register(lua, "GetFreeMemory",                 GetFreeMemory);
    lua_register(lua, "FreeMemory",                    FreeMemory);	
    lua_register(lua, "ReceivedLowMemoryWarning",      ReceivedLowMemoryWarning);
    lua_register(lua, "GetPlatformUniqueIdentifier",   GetPlatformUniqueIdentifier);
    lua_register(lua, "GetPlatformDeviceName",         GetPlatformDeviceName);	
    lua_register(lua, "GetPlatformSystemName",         GetPlatformSystemName);	
    lua_register(lua, "GetPlatformSystemVersion",      GetPlatformSystemVersion);
    lua_register(lua, "GetPlatformModel",              GetPlatformModel);
    lua_register(lua, "GetPlatformMachine",            GetPlatformMachine);
    lua_register(lua, "GetPlatformLocalizedModel",     GetPlatformLocalizedModel);
    lua_register(lua, "GetPlatformIsFirstGen",         GetPlatformIsFirstGen);
    lua_register(lua, "GetPlatformIsSecondGen",        GetPlatformIsSecondGen);
	lua_register(lua, "GetPlatformIsiPad",             GetPlatformIsiPad);
	lua_register(lua, "IsCrystalActive",               IsCrystalActive);
	
    lua_register(lua, "AddContentBundle",              AddContentBundle);
    lua_register(lua, "RemoveContentBundle",           RemoveContentBundle);
    lua_register(lua, "PreloadTexture",                PreloadTexture);
    lua_register(lua, "UnloadTexture",                 UnloadTexture);
    lua_register(lua, "PreloadMesh",                   PreloadMesh);
    lua_register(lua, "UnloadMesh",                    UnloadMesh);

    lua_register(lua, "StartCrystal",                  StartCrystal);
    lua_register(lua, "ActivateCrystaliPad",           ActivateCrystaliPad);
    lua_register(lua, "DeactivateCrystaliPad",         DeactivateCrystaliPad);
	lua_register(lua, "ShowCrystalLeaderboards",       ShowCrystalLeaderboards);
	lua_register(lua, "ShowCrystalAchievements",       ShowCrystalAchievements);
    lua_register(lua, "StartGameSession",              StartGameSession);
    lua_register(lua, "EndGameSession",                EndGameSession);
    lua_register(lua, "StartGameState",                StartGameState);
    lua_register(lua, "LoadRenderables",               LoadRenderables);
    lua_register(lua, "Darken",                        Darken);
    lua_register(lua, "UnDarken",                      UnDarken);
    lua_register(lua, "Lighten",                       Lighten);
    lua_register(lua, "UnLighten",                     UnLighten);
    lua_register(lua, "Flash",                         Flash);
    lua_register(lua, "GetTime",                       GetTime);
    lua_register(lua, "IsIPhone",                      IsIPhone);
    lua_register(lua, "GoToLink",                      GoToLink);
    lua_register(lua, "IsHiResDevice",                 IsHiResDevice);
    lua_register(lua, "CaptureScreen",                 CaptureScreen);

    lua_register(lua, "SetCharacterPaused",            SetCharacterPaused);
    lua_register(lua, "IsCharacterPaused",             IsCharacterPaused);
    lua_register(lua, "SetPaused",                     SetPaused);
    lua_register(lua, "IsPaused",                      IsPaused);
    lua_register(lua, "SetRenderPaused",               SetRenderPaused);
    lua_register(lua, "IsRenderPaused",                IsRenderPaused);
    lua_register(lua, "LoadLevel",                     LoadLevel);
    lua_register(lua, "UnloadLevel",                   UnloadLevel);
    lua_register(lua, "ReloadLevel",                   ReloadLevel);
    lua_register(lua, "DestroyCharacter",              DestroyCharacter);
    lua_register(lua, "GetVectorInput",                GetVectorInput);
    lua_register(lua, "GetTouchPoint",                 GetTouchPoint);
    lua_register(lua, "GetTransformedTouchPoint",      GetTransformedTouchPoint);
    lua_register(lua, "IsButtonDown",                  IsButtonDown);
    lua_register(lua, "SetVectorInput",                SetVectorInput);
    lua_register(lua, "GetCameraFovDegrees",           GetCameraFovDegrees);
    lua_register(lua, "SetCameraFovDegrees",           SetCameraFovDegrees);
    lua_register(lua, "SetCameraTilt",                 SetCameraTilt);
    lua_register(lua, "GetPlayer",                     GetPlayer);
    lua_register(lua, "SetPlayer",                     SetPlayer);
    lua_register(lua, "CollideClampVector",            CollideClampVector);
    lua_register(lua, "CollideBounceVector",           CollideBounceVector);

    lua_register(lua, "SetBaseDepthScale",             SetBaseDepthScale);
    lua_register(lua, "SetSpeedDepth",                 SetSpeedDepth);
    lua_register(lua, "SetSpeedDepthScalar",           SetSpeedDepthScalar);

    lua_register(lua, "SetDefaultMovableRadius",       SetDefaultMovableRadius);

    lua_register(lua, "SetQuad",                       SetQuad);
    lua_register(lua, "DrawQuad",                      DrawQuad);
    lua_register(lua, "DrawQuadRotate",                DrawQuadRotate);
    lua_register(lua, "Draw9Slice",                    Draw9Slice);
    lua_register(lua, "RemoveQuad",                    RemoveQuad);
    lua_register(lua, "GetBackgroundOffsetSize",       GetBackgroundOffsetSize);
    lua_register(lua, "GetBackgroundSize",             GetBackgroundSize);
    lua_register(lua, "GetUiBounds",                   Lua_GetUiBounds);
    lua_register(lua, "GetLookAt",                     Lua_GetLookAt);
    lua_register(lua, "GetReferenceBounds",            Lua_GetReferenceBounds);
    lua_register(lua, "SetLight0OnPlayer",             Lua_SetLight0OnPlayer);

    lua_register(lua, "SetOverlayColor",               SetOverlayColor);
    lua_register(lua, "GetOverlayColor",               GetOverlayColor);

    lua_register(lua, "SetDefaultLightingType",        SetDefaultLightingType);

    lua_register(lua, "GetReadOnlyPath",               GetReadOnlyPath);
    lua_register(lua, "GetReadWritePath",              GetReadWritePath);

    lua_register(lua, "GetSpawnablePoint",             GetSpawnablePoint);
    lua_register(lua, "GetStartPoint",                 GetStartPoint);

    lua_register(lua, "GetNamedPointCount",            GetNamedPointCount);
    lua_register(lua, "GetNamedPoint",                 GetNamedPoint);
    lua_register(lua, "SetMapTileIdAt",                SetMapTileIdAt);
    lua_register(lua, "GetMapTileIdAt",                GetMapTileIdAt);
    lua_register(lua, "GetPlaceName",                  GetPlaceName);
    lua_register(lua, "GetLevelNumber",                GetLevelNumber);

    lua_register(lua, "MakeReadOnlyPath",              MakeReadOnlyPath);
    lua_register(lua, "MakeReadWritePath",             MakeReadWritePath);
    lua_register(lua, "Alert",                         Alert);
	lua_register(lua, "SendMemoryWarning",             SendMemoryWarning);

    lua_register(lua, "CreateCloud",                        CreateCloud);
    lua_register(lua, "CreatePerpetualCloud",               CreatePerpetualCloud);
    lua_register(lua, "CreateCloudIndependent",             CreateCloudIndependent);
    lua_register(lua, "CreatePerpetualCloudIndependent",    CreatePerpetualCloudIndependent);

    lua_register(lua, "CreateCloudLit",                     CreateCloudLit);
    lua_register(lua, "CreatePerpetualCloudLit",            CreatePerpetualCloudLit);
    lua_register(lua, "CreateCloudLitIndependent",          CreateCloudLitIndependent);
    lua_register(lua, "CreatePerpetualCloudLitIndependent", CreatePerpetualCloudLitIndependent);
    
    lua_register(lua, "CreateCircleCloud",             CreateCircleCloud);

    lua_register(lua, "CreateCollisionTest",           CreateCollisionTest);

    lua_register(lua, "Effect_Destroy",                Effect_Destroy);
    lua_register(lua, "Effect_SetPosition",            Effect_SetPosition);

    lua_register(lua, "Multiplayer_SetProfileName",    Multiplayer_SetProfileName);
    lua_register(lua, "Multiplayer_StartServer",       Multiplayer_StartServer);
    lua_register(lua, "Multiplayer_StartClient",       Multiplayer_StartClient);
    lua_register(lua, "Multiplayer_Join",              Multiplayer_Join);
    lua_register(lua, "Multiplayer_IsJoined",          Multiplayer_IsJoined);
//  lua_register(lua, "Multiplayer_GetId",             Multiplayer_GetId);
    lua_register(lua, "Multiplayer_IsClient",          Multiplayer_IsClient);
    lua_register(lua, "Multiplayer_IsServer",          Multiplayer_IsServer);
    lua_register(lua, "Multiplayer_StartENetServer",   Multiplayer_StartENetServer);
    lua_register(lua, "Multiplayer_StartENetClient",   Multiplayer_StartENetClient);
    lua_register(lua, "Multiplayer_GetConnectionCount",Multiplayer_GetConnectionCount);
    lua_register(lua, "Multiplayer_IsLosingConnection",Multiplayer_IsLosingConnection);
    lua_register(lua, "Multiplayer_Close",             Multiplayer_Close);
    lua_register(lua, "Multiplayer_GetIndexById",      Multiplayer_GetIndexById);
    lua_register(lua, "Multiplayer_GetIdByIndex",      Multiplayer_GetIdByIndex);
    lua_register(lua, "Multiplayer_ChangeId",          Multiplayer_ChangeId);

    lua_register(lua, "Multiplayer_StartBluetoothServer", Multiplayer_StartBluetoothServer);
    lua_register(lua, "Multiplayer_StartBluetoothClient", Multiplayer_StartBluetoothClient);

    lua_register(lua, "Sync_SendLuaMessage",           Sync_SendLuaMessage);
    lua_register(lua, "Sync_HasLuaMessage",            Sync_HasLuaMessage);
    lua_register(lua, "Sync_RemoveLuaMessage",         Sync_RemoveLuaMessage);
    lua_register(lua, "Sync_ClearLuaMessages",         Sync_ClearLuaMessages);
    lua_register(lua, "Sync_NextLuaMessage",           Sync_NextLuaMessage);
    lua_register(lua, "Sync_PopLuaMessage",            Sync_PopLuaMessage);

    lua_register(lua, "Sync_SendDataMessage",          Sync_SendDataMessage);
    lua_register(lua, "Sync_HasDataMessage",           Sync_HasDataMessage);
    lua_register(lua, "Sync_RemoveDataMessage",        Sync_RemoveDataMessage);
    lua_register(lua, "Sync_ClearDataMessages",        Sync_ClearDataMessages);
    lua_register(lua, "Sync_NextDataMessage",          Sync_NextDataMessage);
    lua_register(lua, "Sync_PopDataMessage",           Sync_PopDataMessage);

    // lights
    lua_register(lua, "Lux_EnableLighting",            Lua_LuxEnableLighting);
    lua_register(lua, "Lux_Enable",                    Lua_LuxEnable);

    lua_register(lua, "Lux_SetAmbient",                Lua_LuxSetAmbient);
    lua_register(lua, "Lux_SetDiffuse",                Lua_LuxSetDiffuse);
    lua_register(lua, "Lux_SetSpecular",               Lua_LuxSetSpecular);
    lua_register(lua, "Lux_SetWorldPosition",          Lua_LuxSetWorldPosition);
    lua_register(lua, "Lux_SetSpotDirection",          Lua_LuxSetSpotDirection);
    
    lua_register(lua, "Lux_GetAmbient",                Lua_LuxGetAmbient);
    lua_register(lua, "Lux_GetDiffuse",                Lua_LuxGetDiffuse);
    lua_register(lua, "Lux_GetSpecular",               Lua_LuxGetSpecular);
    lua_register(lua, "Lux_GetWorldPosition",          Lua_LuxGetWorldPosition);
    lua_register(lua, "Lux_GetSpotDirection",          Lua_LuxGetSpotDirection);

    lua_register(lua, "Lux_SetShininess",              Lua_LuxSetShininess);
    lua_register(lua, "Lux_SetSpotExponent",           Lua_LuxSetSpotExponent);
    lua_register(lua, "Lux_SetSpotCutoff",             Lua_LuxSetSpotCutoff);
    lua_register(lua, "Lux_SetConstantAttenuation",    Lua_LuxSetConstantAttenuation);
    lua_register(lua, "Lux_SetLinearAttenuation",      Lua_LuxSetLinearAttenuation);
    lua_register(lua, "Lux_SetQuadraticAttenuation",   Lua_LuxSetQuadraticAttenuation);
    
    lua_register(lua, "Lux_GetShininess",              Lua_LuxGetShininess);
    lua_register(lua, "Lux_GetSpotExponent",           Lua_LuxGetSpotExponent);
    lua_register(lua, "Lux_GetSpotCutoff",             Lua_LuxGetSpotCutoff);
    lua_register(lua, "Lux_GetConstantAttenuation",    Lua_LuxGetConstantAttenuation);
    lua_register(lua, "Lux_GetLinearAttenuation",      Lua_LuxGetLinearAttenuation);
    lua_register(lua, "Lux_GetQuadraticAttenuation",   Lua_LuxGetQuadraticAttenuation);

    lua_register(lua, "SetBaseUvTransform",            SetBaseUvTransform);
    lua_register(lua, "UnsetBaseUvTransform",          UnsetBaseUvTransform);
}
