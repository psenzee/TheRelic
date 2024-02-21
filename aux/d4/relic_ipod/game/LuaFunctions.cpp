#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include <string>

extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#include "luautil/LuaUtils.h"
#include "luautil/LuaThread.h"
#include "luautil/LuaPack.h"

#include "Character.h"
#include "GameState.h"
#include "Multiplayer.h"

#include "render/OverheadCamera.h"
#include "render/QuadRenderer.h"
#include "render/LocalGameServices.h"
#include "render/ContentLoader.h"
#include "render/ParticleEffects.h"
#include "platform/GLUtils.h"
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

#include "fast/Allocator.h"

extern GameState *state;
extern float accel[3];
extern bool  buttons[32];
extern float _g_inputSensitivity;

#define VectorToScalar(NAME, EXPRESSION)       \
  int NAME(lua_State *lua)                     \
  {                                            \
      luaL_checktype(lua, -3, LUA_TNUMBER);    \
      luaL_checktype(lua, -2, LUA_TNUMBER);    \
      luaL_checktype(lua, -1, LUA_TNUMBER);    \
                                               \
      Vector3 v((float)lua_tonumber(lua, -3),  \
                (float)lua_tonumber(lua, -2),  \
                (float)lua_tonumber(lua, -1)); \
      lua_pushnumber(lua, EXPRESSION);         \
      return 1;                                \
  }

#define VectorPairToScalar(NAME, EXPRESSION)   \
  int NAME(lua_State *lua)                     \
  {                                            \
      luaL_checktype(lua, -6, LUA_TNUMBER);    \
      luaL_checktype(lua, -5, LUA_TNUMBER);    \
      luaL_checktype(lua, -4, LUA_TNUMBER);    \
                                               \
      luaL_checktype(lua, -3, LUA_TNUMBER);    \
      luaL_checktype(lua, -2, LUA_TNUMBER);    \
      luaL_checktype(lua, -1, LUA_TNUMBER);    \
                                               \
      Vector3 u((float)lua_tonumber(lua, -6),  \
                (float)lua_tonumber(lua, -5),  \
		        (float)lua_tonumber(lua, -4)), \
              v((float)lua_tonumber(lua, -3),  \
		        (float)lua_tonumber(lua, -2),  \
		        (float)lua_tonumber(lua, -1)); \
      lua_pushnumber(lua, EXPRESSION);         \
      return 1;                                \
  }

#define VectorPairToVector(NAME, EXPRESSION)   \
  int NAME(lua_State *lua)                     \
  {                                            \
      luaL_checktype(lua, -6, LUA_TNUMBER);    \
      luaL_checktype(lua, -5, LUA_TNUMBER);    \
      luaL_checktype(lua, -4, LUA_TNUMBER);    \
                                               \
      luaL_checktype(lua, -3, LUA_TNUMBER);    \
      luaL_checktype(lua, -2, LUA_TNUMBER);    \
      luaL_checktype(lua, -1, LUA_TNUMBER);    \
                                               \
      Vector3 u((float)lua_tonumber(lua, -6),  \
                (float)lua_tonumber(lua, -5),  \
		        (float)lua_tonumber(lua, -4)), \
              v((float)lua_tonumber(lua, -3),  \
		        (float)lua_tonumber(lua, -2),  \
		        (float)lua_tonumber(lua, -1)); \
                                               \
      v = EXPRESSION;                          \
                                               \
      lua_pushnumber(lua, v.x);                \
      lua_pushnumber(lua, v.y);                \
      lua_pushnumber(lua, v.z);                \
      return 3;                                \
  }

#define VectorToVector(NAME, EXPRESSION)       \
  int NAME(lua_State *lua)                     \
  {                                            \
      luaL_checktype(lua, -3, LUA_TNUMBER);    \
      luaL_checktype(lua, -2, LUA_TNUMBER);    \
      luaL_checktype(lua, -1, LUA_TNUMBER);    \
                                               \
      Vector3 v((float)lua_tonumber(lua, -3),  \
		        (float)lua_tonumber(lua, -2),  \
		        (float)lua_tonumber(lua, -1)); \
                                               \
      v = EXPRESSION;                          \
                                               \
      lua_pushnumber(lua, v.x);                \
      lua_pushnumber(lua, v.y);                \
      lua_pushnumber(lua, v.z);                \
      return 3;                                \
  }

#define VectorScalarPairToVector(NAME, EXPRESSION) \
  int NAME(lua_State *lua)                         \
  {                                                \
      luaL_checktype(lua, -5, LUA_TNUMBER);        \
      luaL_checktype(lua, -4, LUA_TNUMBER);        \
      luaL_checktype(lua, -3, LUA_TNUMBER);        \
                                                   \
      luaL_checktype(lua, -2, LUA_TNUMBER);        \
      luaL_checktype(lua, -1, LUA_TNUMBER);        \
                                                   \
      Vector3 v(  (float)lua_tonumber(lua, -5),    \
		          (float)lua_tonumber(lua, -4),    \
		          (float)lua_tonumber(lua, -3));   \
      float   a = (float)lua_tonumber(lua, -2),    \
              b = (float)lua_tonumber(lua, -1);    \
                                                   \
      v = EXPRESSION;                              \
                                                   \
      lua_pushnumber(lua, v.x);                    \
      lua_pushnumber(lua, v.y);                    \
      lua_pushnumber(lua, v.z);                    \
      return 3;                                    \
  }

#define VectorScalarToVector(NAME, EXPRESSION)     \
  int NAME(lua_State *lua)                         \
  {                                                \
      luaL_checktype(lua, -4, LUA_TNUMBER);        \
      luaL_checktype(lua, -3, LUA_TNUMBER);        \
      luaL_checktype(lua, -2, LUA_TNUMBER);        \
                                                   \
      luaL_checktype(lua, -1, LUA_TNUMBER);        \
                                                   \
      Vector3 v(  (float)lua_tonumber(lua, -4),    \
		          (float)lua_tonumber(lua, -3),    \
		          (float)lua_tonumber(lua, -2));   \
      float   a = (float)lua_tonumber(lua, -1);    \
                                                   \
      v = EXPRESSION;                              \
                                                   \
      lua_pushnumber(lua, v.x);                    \
      lua_pushnumber(lua, v.y);                    \
      lua_pushnumber(lua, v.z);                    \
      return 3;                                    \
  }

#define VectorPairScalarToVector(NAME, EXPRESSION) \
  int NAME(lua_State *lua)                         \
  {                                                \
      luaL_checktype(lua, -7, LUA_TNUMBER);        \
      luaL_checktype(lua, -6, LUA_TNUMBER);        \
      luaL_checktype(lua, -5, LUA_TNUMBER);        \
                                                   \
      luaL_checktype(lua, -4, LUA_TNUMBER);        \
      luaL_checktype(lua, -3, LUA_TNUMBER);        \
      luaL_checktype(lua, -2, LUA_TNUMBER);        \
                                                   \
      luaL_checktype(lua, -1, LUA_TNUMBER);        \
                                                   \
      Vector3 u  ((float)lua_tonumber(lua, -7),    \
		          (float)lua_tonumber(lua, -6),    \
		          (float)lua_tonumber(lua, -5));   \
      Vector3 v  ((float)lua_tonumber(lua, -4),    \
		          (float)lua_tonumber(lua, -3),    \
		          (float)lua_tonumber(lua, -2));   \
      float   a = (float)lua_tonumber(lua, -1);    \
                                                   \
      v = EXPRESSION;                              \
                                                   \
      lua_pushnumber(lua, v.x);                    \
      lua_pushnumber(lua, v.y);                    \
      lua_pushnumber(lua, v.z);                    \
      return 3;                                    \
  }

inline float Interpolate(float a, float b, float t)
{
    return b + (a - b) * t;
}

static Vector3 v3Interpolate(const Vector3 &u, const Vector3 &v, float t)
{
    return Vector3(Interpolate(u.x, v.x, t),
                   Interpolate(u.y, v.y, t),
                   Interpolate(u.z, v.z, t));
}

static Vector3 v3Clamp(const Vector3 &v, float lo, float hi)
{
    return Vector3(math::clamp(v.x, lo, hi), math::clamp(v.y, lo, hi), math::clamp(v.z, lo, hi));
}

VectorToScalar           (VectorLength,      v.length())
VectorToScalar           (VectorLengthSq,    v.lengthsq())
VectorPairToScalar       (VectorDot,         u.dot(v))
VectorPairToScalar       (VectorDistance,    u.distance(v))
VectorPairToScalar       (VectorDistanceSq,  u.distancesq(v))
VectorPairToVector       (VectorCross,       u.cross(v))
VectorPairToVector       (VectorAdd,         u + v)
VectorPairToVector       (VectorSub,         u - v)
VectorPairToVector       (VectorMul,         u * v)
VectorPairToVector       (VectorDiv,         u / v)
VectorPairToVector       (VectorMax,         u.maximum(v))
VectorPairToVector       (VectorMin,         u.minimum(v))
VectorToVector           (VectorNormal,      v.normal())
VectorScalarPairToVector (VectorClamp,       v3Clamp(v, a, b))
VectorPairScalarToVector (VectorInterpolate, v3Interpolate(u, v, a))

void RegisterLuaVectorFunctions(lua_State *lua)
{
    lua_register(lua, "v3Length",                VectorLength);
    lua_register(lua, "v3LengthSq",              VectorLengthSq);
    lua_register(lua, "v3Dot",                   VectorDot);
    lua_register(lua, "v3Distance",              VectorDistance);
    lua_register(lua, "v3DistanceSq",            VectorDistanceSq);
    lua_register(lua, "v3Cross",                 VectorCross);
    lua_register(lua, "v3Add",                   VectorAdd);
    lua_register(lua, "v3Sub",                   VectorSub);
    lua_register(lua, "v3Mul",                   VectorMul);
    lua_register(lua, "v3Div",                   VectorDiv);
    lua_register(lua, "v3Min",                   VectorMin);
    lua_register(lua, "v3Max",                   VectorMax);
    lua_register(lua, "v3Normal",                VectorNormal);
    lua_register(lua, "v3Clamp",                 VectorClamp);
    lua_register(lua, "v3Interpolate",           VectorInterpolate);
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

static int GetVectorInput(lua_State *lua)
{
    lua_pushnumber(lua, accel[1] / _g_inputSensitivity); // y // x and y are swapped in the accel vector
    lua_pushnumber(lua, accel[0] / _g_inputSensitivity); // x // retarded but true
    lua_pushnumber(lua, accel[2] / _g_inputSensitivity); // z
    return 3;
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
    luaL_checktype(lua, -1, LUA_TNUMBER); // z
    accel[1] = (float)lua_tonumber(lua, -3) * _g_inputSensitivity; // x and y are swapped in the accel vector
	accel[0] = (float)lua_tonumber(lua, -2) * _g_inputSensitivity; // retarded but true
	accel[2] = (float)lua_tonumber(lua, -1) * _g_inputSensitivity;
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

    enum { UP_MASK    =   2 |  4 |  8,
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

static int GetSpawnablePoint(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TNUMBER);
    Level *level = state->levels->GetLevel();
    int index = (int)lua_tointeger(lua, -1);
    ReturnVector3(lua, level->GetSpawnablePoint(index));
    return 3;
}

static int GetNamedPoint(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TNUMBER);
    luaL_checktype(lua, -2, LUA_TSTRING);
    Level *level = state->levels->GetLevel();
    int         index = (int)lua_tointeger(lua, -1);
    const char *name  = lua_tostring(lua, -2);
    ReturnVector3(lua, level->GetNamedPoint(name, index));
    return 3;
}

static int SetMapTileIdAt(lua_State *lua)
{
    Level *level = state->levels->GetLevel();
    Map *map = level->GetMap(0);
    map->SetTileIdAt(v2(GetVector3(lua, -2)), GetInt(lua, -1));
    return 0;
}

static int GetMapTileIdAt(lua_State *lua)
{
    Level *level = state->levels->GetLevel();
    Map *map = level->GetMap(0);
    int id = map->GetTileIdAt(v2(GetVector3(lua, -1)));
    lua_pushinteger(lua, id);
    return 1;
}

static int GetNamedPointCount(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TSTRING);
    Level *level = state->levels->GetLevel();
    const char *name  = lua_tostring(lua, -1);
    lua_pushinteger(lua, level->GetNamedPointCount(name));
    return 1;
}

static int GetStartPoint(lua_State *lua)
{
    Level *level = state->levels->GetLevel();
    ReturnVector3(lua, level->GetStartPoint());
    return 3;
}

static int GetPlayerTileCenter(lua_State *lua)
{
    ReturnVector3(lua, state->GetPlayerTileCenter());
    return 3;
}

static int GetCameraFovDegrees(lua_State *lua)
{
    lua_pushnumber(lua, state->GetCamera()->GetFovDegrees()); // fov
    return 1;
}

static int SetPlayer(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TUSERDATA);
    state->SetPlayer(*(Character **)lua_touserdata(lua, -1));
    return 0;
}

static int GetPlayer(lua_State *lua)
{
    lua_rawgeti(lua, LUA_REGISTRYINDEX, state->GetPlayer()->GetLuaReference());
    return 1;
}

static int SetCameraFovDegrees(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TNUMBER); // fov
    float fov = (float)lua_tonumber(lua, -1);
    state->GetCamera()->SetFovDegrees(fov);
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
    state->camera->SetDepthScale(BASE_SCALE + v.length() * scale);
}

static void SetSpeedDepthScalar(GameState *state, float length, float scale)
{
    state->camera->SetDepthScale(BASE_SCALE + length * scale);
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

    SetSpeedDepth(state, v, (float)lua_tonumber(lua, -1));
    return 0;
}

static int SetSpeedDepthScalar(lua_State *lua)
{
    luaL_checktype(lua, -2,  LUA_TNUMBER); // length
    luaL_checktype(lua, -1,  LUA_TNUMBER); // scale

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

    DeviceTexture *texture = state->services->loader->GetTexture(lua_tostring(lua, -12));
    Vector4        color   = Vector4(1.0, 1.0, 1.0, (float)lua_tonumber(lua, -11));
    Vector3        start   = Vector3((float)lua_tonumber(lua, -10), (float)lua_tonumber(lua, -9), (float)lua_tonumber(lua, -8));
    Vector3        end     = Vector3((float)lua_tonumber(lua, -7),  (float)lua_tonumber(lua, -6), (float)lua_tonumber(lua, -5));
    Vector2        t0      = Vector2((float)lua_tonumber(lua, -4),  (float)lua_tonumber(lua, -3)),
                   t1      = Vector2((float)lua_tonumber(lua, -2),  (float)lua_tonumber(lua, -1));

    state->quads->RenderScreenAlignedQuad(texture, color, start, end, t0, t1);
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
    quad.texture = state->services->loader->GetTexture(lua_tostring(lua, -12));

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

    state->quads->Set(lua_tostring(lua, -13), quad);
    return 0;
}

static int RemoveQuad(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TSTRING); // quad name
    state->quads->Remove(lua_tostring(lua, -1));
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
    DeviceTexture  *texture = state->services->loader->GetTexture(GetString(lua, -6));
    ParticleSystem *ps = ParticleEffects::GetInstance()->CreateCloud(texture, position, time, speed, false, true);
    lua_pushlightuserdata(lua, ps);
    return 1;
}

// particle systems
static int CreatePerpetualCloud(lua_State *lua)
{
    float           speed = GetFloat(lua, -1),
                    time  = GetFloat(lua, -2);
    Vector3         position(GetVector3(lua, -3));
    DeviceTexture  *texture = state->services->loader->GetTexture(GetString(lua, -6));
    ParticleSystem *ps = ParticleEffects::GetInstance()->CreateCloud(texture, position, time, speed, true, false);
    lua_pushlightuserdata(lua, ps);
    return 1;
}

// particle systems
static int CreatePerpetualCircle(lua_State *lua)
{
    float           speed = GetFloat(lua, -1),
                    time  = GetFloat(lua, -2);
    Vector3         position(GetVector3(lua, -3));
    DeviceTexture  *texture = state->services->loader->GetTexture(GetString(lua, -6));
    ParticleSystem *ps = ParticleEffects::GetInstance()->CreateCircle(texture, position, time, speed, true, false);
    lua_pushlightuserdata(lua, ps);
    return 1;
}

static int CreateCircleCloud(lua_State *lua)
{
    float           speed = GetFloat(lua, -1),
                    time  = GetFloat(lua, -2);
    Vector3         position(GetVector3(lua, -3));
    DeviceTexture  *texture = state->services->loader->GetTexture(GetString(lua, -6));
    ParticleSystem *ps = ParticleEffects::GetInstance()->CreateCircle(texture, position, time, speed, false, true);
    lua_pushlightuserdata(lua, ps);
    return 1;
}

static int CreateCollisionTest(lua_State *lua)
{
    float           speed = GetFloat(lua, -1),
                    time  = GetFloat(lua, -2);
    Vector3         position(GetVector3(lua, -3));
    DeviceTexture  *texture = state->services->loader->GetTexture(GetString(lua, -6));
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
    state->effects.Darken(static_cast<int>(lua_tointeger(lua, -1)));
    return 0;
}

static int UnDarken(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TNUMBER);
    state->effects.UnDarken(static_cast<int>(lua_tointeger(lua, -1)));
    return 0;
}

static int Lighten(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TNUMBER);
    state->effects.Lighten(static_cast<int>(lua_tointeger(lua, -1)));
    return 0;
}

static int UnLighten(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TNUMBER);
    state->effects.UnLighten(static_cast<int>(lua_tointeger(lua, -1)));
    return 0;
}

static int Flash(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TNUMBER);
    state->effects.Flash(static_cast<int>(lua_tointeger(lua, -1)));
    return 0;
}

static int GetPlayerTileEventCount(lua_State *lua)
{
    lua_pushinteger(lua, state->GetLevel()->GetTileEventCountAt(state->GetPlayer()->GetPosition()));
    return 1;
}

static int GetPlayerTileEvent(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TNUMBER);
    int index = static_cast<int>(lua_tointeger(lua, -1));
    lua_pushstring(lua, state->GetLevel()->GetTileEventAt(state->GetPlayer()->GetPosition(), index));
    return 1;
}

static int HasPlayerTileEvent(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TSTRING);
    const char *prefix = lua_tostring(lua, -1);
    lua_pushboolean(lua, state->GetLevel()->HasTileEventAt(prefix, state->GetPlayer()->GetPosition()) != 0);
    return 1;
}

static int SendRisingMessage(lua_State *lua)
{
    luaL_checktype(lua, -2, LUA_TSTRING);
    luaL_checktype(lua, -1, LUA_TNUMBER);
    state->ui->SendBallMessage(lua_tostring(lua, -2), static_cast<int>(lua_tointeger(lua, -1)));
    return 0;
}

static int Multiplayer_SetProfileName(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TSTRING);
    state->multiplayer.SetProfileName(lua_tostring(lua, -1));
    return 0;
}

static int Multiplayer_StartServer(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TSTRING);
    lua_pushboolean(lua, state->multiplayer.StartServer(NETGAME_PORT, lua_tostring(lua, -1)) ? 1 : 0);
    return 1;
}

static int Multiplayer_StartClient(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TSTRING);
    lua_pushboolean(lua, state->multiplayer.StartClient(lua_tostring(lua, -1)) ? 1 : 0);
    return 1;
}

static int Multiplayer_Join(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TSTRING);
    lua_pushboolean(lua, state->multiplayer.Join(lua_tostring(lua, -1), NETGAME_PORT) ? 1 : 0);
    return 1;
}

static int Multiplayer_IsJoined(lua_State *lua)
{
    lua_pushboolean(lua, state->multiplayer.IsJoined());
    return 1;
}

static int Multiplayer_GetId(lua_State *lua)
{
    lua_pushinteger(lua, state->multiplayer.GetId());
    return 1;
}

// CharacterSync *LuaMessage functions
static int Sync_SendMessage(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TSTRING);
    size_t length = 0;
    const char *str = lua_tolstring(lua, -1, &length);
    CharacterSync *sync = state->GetLevel()->GetCharacterSync();
    sync->SendLuaMessage(str, static_cast<int>(length));
    return 0;
}

static int Sync_HasMessage(lua_State *lua)
{
    CharacterSync *sync = state->GetLevel()->GetCharacterSync();
    lua_pushboolean(lua, sync->HasLuaMessage());
    return 1;
}

static int Sync_RemoveMessage(lua_State *lua)
{
    CharacterSync *sync = state->GetLevel()->GetCharacterSync();
    sync->RemoveLuaMessage();
    return 0;
}

static int Sync_ClearMessages(lua_State *lua)
{
    CharacterSync *sync = state->GetLevel()->GetCharacterSync();
    sync->ClearLuaMessages();
    return 0;
}

static int Sync_NextMessage(lua_State *lua)
{
    CharacterSync *sync = state->GetLevel()->GetCharacterSync();
    const std::string &s = sync->NextLuaMessage();
    lua_pushlstring(lua, s.data(), s.size());
    return 1;
}

static int Sync_PopMessage(lua_State *lua)
{
    CharacterSync *sync = state->GetLevel()->GetCharacterSync();
    const std::string &s = sync->NextLuaMessage();
    lua_pushlstring(lua, s.data(), s.size());
    sync->RemoveLuaMessage();
    return 1;
}

static int GetTime(lua_State *lua)
{
    lua_pushnumber(lua, GetCurrentTimeMs());
    return 1;
}

static int Text_DrawString(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TNUMBER);
    luaL_checktype(lua, -2, LUA_TNUMBER);
    luaL_checktype(lua, -3, LUA_TNUMBER);
    luaL_checktype(lua, -4, LUA_TNUMBER);
    luaL_checktype(lua, -5, LUA_TSTRING);
    const char *text  = lua_tostring(lua, -5);
    float x    = static_cast<float>(lua_tonumber(lua, -4)), y     = static_cast<float>(lua_tonumber(lua, -3)),
          size = static_cast<float>(lua_tonumber(lua, -2)), alpha = static_cast<float>(lua_tonumber(lua, -1));
    state->ui->DrawString(GetGlobalRenderContext(), text, Vector2(x, y), alpha, size);
    return 0;
}

static int Text_DrawStringCentered(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TNUMBER);
    luaL_checktype(lua, -2, LUA_TNUMBER);
    luaL_checktype(lua, -3, LUA_TNUMBER);
    luaL_checktype(lua, -4, LUA_TNUMBER);
    luaL_checktype(lua, -5, LUA_TSTRING);
    const char *text  = lua_tostring(lua, -5);
    float x    = static_cast<float>(lua_tonumber(lua, -4)), y     = static_cast<float>(lua_tonumber(lua, -3)),
          size = static_cast<float>(lua_tonumber(lua, -2)), alpha = static_cast<float>(lua_tonumber(lua, -1));
    state->ui->DrawStringCentered(GetGlobalRenderContext(), text, Vector2(x, y), alpha, size);
    return 0;
}

static int Text_DrawTextCentered(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TNUMBER);
    luaL_checktype(lua, -2, LUA_TNUMBER);
    luaL_checktype(lua, -3, LUA_TNUMBER);
    luaL_checktype(lua, -4, LUA_TNUMBER);
    luaL_checktype(lua, -5, LUA_TSTRING);
    const char *text  = lua_tostring(lua, -5);
    float x    = static_cast<float>(lua_tonumber(lua, -4)), y     = static_cast<float>(lua_tonumber(lua, -3)),
          size = static_cast<float>(lua_tonumber(lua, -2)), alpha = static_cast<float>(lua_tonumber(lua, -1));
    state->ui->DrawTextCentered(GetGlobalRenderContext(), text, Vector2(x, y), alpha, size);
    return 0;
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

void RegisterLuaFunctions(lua_State *lua)
{
    RegisterLuaVectorFunctions(lua);
    Serialize::RegisterLuaSerializeFunctions(lua);

    lua_register(lua, "Pack",                          LuaPack);
    lua_register(lua, "Unpack",                        LuaUnpack);
    lua_register(lua, "CompileScript",                 LuaCompileScript);

    lua_register(lua, "PrintMemoryStatistics",         PrintMemoryStatistics);
    lua_register(lua, "DumpMemoryStatistics",          DumpMemoryStatistics);

    lua_register(lua, "GetPlayerTileCenter",           GetPlayerTileCenter);

    lua_register(lua, "GetPlayerTileEventCount",       GetPlayerTileEventCount);
    lua_register(lua, "GetPlayerTileEvent",            GetPlayerTileEvent);
    lua_register(lua, "HasPlayerTileEvent",            HasPlayerTileEvent);

    lua_register(lua, "SendRisingMessage",             SendRisingMessage);
    lua_register(lua, "Darken",                        Darken);
    lua_register(lua, "UnDarken",                      UnDarken);
    lua_register(lua, "Lighten",                       Lighten);
    lua_register(lua, "UnLighten",                     UnLighten);
    lua_register(lua, "Flash",                         Flash);
    lua_register(lua, "GetTime",                       GetTime);

    lua_register(lua, "GetVectorInput",                GetVectorInput);
    lua_register(lua, "IsButtonDown",                  IsButtonDown);
    lua_register(lua, "SetVectorInput",                SetVectorInput);
    lua_register(lua, "GetCameraFovDegrees",           GetCameraFovDegrees);
    lua_register(lua, "SetCameraFovDegrees",           SetCameraFovDegrees);
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
    lua_register(lua, "RemoveQuad",                    RemoveQuad);

    lua_register(lua, "SetDefaultLightingType",        SetDefaultLightingType);

    lua_register(lua, "GetReadOnlyPath",               GetReadOnlyPath);
    lua_register(lua, "GetReadWritePath",              GetReadWritePath);

    lua_register(lua, "GetSpawnablePoint",             GetSpawnablePoint);
    lua_register(lua, "GetStartPoint",                 GetStartPoint);

    lua_register(lua, "GetNamedPointCount",            GetNamedPointCount);
    lua_register(lua, "GetNamedPoint",                 GetNamedPoint);
    lua_register(lua, "SetMapTileIdAt",                SetMapTileIdAt);
    lua_register(lua, "GetMapTileIdAt",                GetMapTileIdAt);

    lua_register(lua, "MakeReadOnlyPath",              MakeReadOnlyPath);
    lua_register(lua, "MakeReadWritePath",             MakeReadWritePath);

    lua_register(lua, "CreateCloud",                   CreateCloud);
    lua_register(lua, "CreatePerpetualCloud",          CreatePerpetualCloud);
    lua_register(lua, "CreateCircleCloud",             CreateCircleCloud);

    lua_register(lua, "CreateCollisionTest",           CreateCollisionTest);

    lua_register(lua, "Effect_Destroy",                Effect_Destroy);
    lua_register(lua, "Effect_SetPosition",            Effect_SetPosition);

    lua_register(lua, "Multiplayer_SetProfileName",    Multiplayer_SetProfileName);
    lua_register(lua, "Multiplayer_StartServer",       Multiplayer_StartServer);
    lua_register(lua, "Multiplayer_StartClient",       Multiplayer_StartClient);
    lua_register(lua, "Multiplayer_Join",              Multiplayer_Join);
    lua_register(lua, "Multiplayer_IsJoined",          Multiplayer_IsJoined);
    lua_register(lua, "Multiplayer_GetId",             Multiplayer_GetId);

    lua_register(lua, "Sync_SendMessage",              Sync_SendMessage);
    lua_register(lua, "Sync_HasMessage",               Sync_HasMessage);
    lua_register(lua, "Sync_RemoveMessage",            Sync_RemoveMessage);
    lua_register(lua, "Sync_ClearMessages",            Sync_ClearMessages);
    lua_register(lua, "Sync_NextMessage",              Sync_NextMessage);
    lua_register(lua, "Sync_PopMessage",               Sync_PopMessage);

    lua_register(lua, "Text_DrawString",               Text_DrawString);
    lua_register(lua, "Text_DrawStringCentered",       Text_DrawStringCentered);
    lua_register(lua, "Text_DrawTextCentered",         Text_DrawTextCentered);
}
