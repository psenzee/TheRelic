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

#include "math/MathUtil.h"

#include "fast/Allocator.h"

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
VectorPairToScalar       (VectorDotNormals,  u.normal().dot(v.normal()))
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
VectorPairToScalar       (Lua_GetAngle,      MathUtil::GetAngle(u, v, 0.0))

int Lua_NormalizeAngle2Pi(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TNUMBER);
    lua_pushnumber(lua, MathUtil::NormalizeAngle2Pi(static_cast<float>(lua_tonumber(lua, -1))));
    return 1;
}

int Lua_NormalizeAnglePiNegPi(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TNUMBER);
    lua_pushnumber(lua, MathUtil::NormalizeAnglePiNegPi(static_cast<float>(lua_tonumber(lua, -1))));
    return 1;
}

int Lua_AreAnglesClose(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TNUMBER);
    luaL_checktype(lua, -2, LUA_TNUMBER);
    luaL_checktype(lua, -3, LUA_TNUMBER);
    lua_pushboolean(lua,
        MathUtil::AreAnglesClose(
            static_cast<float>(lua_tonumber(lua, -3)), 
            static_cast<float>(lua_tonumber(lua, -2)), 
            static_cast<float>(lua_tonumber(lua, -1))));
    return 1;
}

int Lua_GetAngleDifference(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TNUMBER);
    luaL_checktype(lua, -2, LUA_TNUMBER);
    lua_pushnumber(lua,
        MathUtil::GetAngleDifference(
            static_cast<float>(lua_tonumber(lua, -2)), 
            static_cast<float>(lua_tonumber(lua, -1))));
    return 1;
}

void RegisterLuaMathFunctions(lua_State *lua)
{
    lua_register(lua, "v3Length",                VectorLength);
    lua_register(lua, "v3LengthSq",              VectorLengthSq);
    lua_register(lua, "v3Dot",                   VectorDot);
    lua_register(lua, "v3DotNormals",            VectorDotNormals);
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

    lua_register(lua, "AreAnglesClose",          Lua_AreAnglesClose);
    lua_register(lua, "GetAngle",                Lua_GetAngle);
    lua_register(lua, "NormalizeAnglePiNegPi",   Lua_NormalizeAnglePiNegPi);
    lua_register(lua, "NormalizeAngle2Pi",       Lua_NormalizeAngle2Pi);
    lua_register(lua, "GetAngleDifference",      Lua_GetAngleDifference);
}
