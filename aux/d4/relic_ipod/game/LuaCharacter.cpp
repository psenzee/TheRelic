#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#include "Character.h"
#include "GameState.h"
#include "Sync.h"
#include "level/Level.h"
#include "LuaCharacter.h"
#include "luautil/LuaThread.h"
#include "luautil/LuaUtils.h"
#include "luautil/LuaValue.h"
#include "Selector.h"
#include "render/DrawableUtils.h"
#include "render/Drawable.h"

extern GameState *state;

static Character *CreateCharacter(int id, const Vector3 &position, const char *type, const char *luaFunctionName)
{
    Character *ch = new Character(position, state->GetRenderables(), type, luaFunctionName); 
    if (id == -1) id = state->GetLevel()->GetFreeCharacterId();
    ch->SetId(id);
    state->GetLevel()->AddCharacter(ch);
    /*
    Drawable *ring = CreateRing("selector", "RunicRing2", 16, 25.0f, 40.0f);
    Selector *s = new Selector(ring, rand() / float(RAND_MAX) * 0.040f + 0.025f);
    ch->SetSelector(s);
    ring->Release();
    */
    return ch;
}

static void DestroyCharacter(Character *ch)
{
    if (ch)
    {
        state->GetLevel()->RemoveCharacter(ch);
        delete ch;
    }
}

// here we're going to use a pointer and allocate the actual object in C++
static int NewCharacter(lua_State *lua)
{
    enum { OFFSET_ID = -6, OFFSET_LUA_FUNCTION = -5, OFFSET_TYPE = -4, OFFSET_V3 = -1 };

    int id = GetInt(lua, OFFSET_ID);
    const char *luaFunction = GetString(lua, OFFSET_LUA_FUNCTION);
    const char *type = GetString(lua, OFFSET_TYPE);
    Vector3 position(GetVector3(lua, OFFSET_V3));
    Character **data = (Character **)lua_newuserdata(lua, sizeof(Character *));    
    luaL_getmetatable(lua, "D.Character");
    lua_setmetatable(lua, -2);
    
    lua_State *global = state->GetInterpreter()->GetState();

    *data = CreateCharacter(id, position, type, luaFunction);
    (*data)->SetLuaTable(LuaTable::New(global));

    lua_pushvalue(lua, -1);
    lua_xmove(lua, global, 1);
    int ref = luaL_ref(global, LUA_REGISTRYINDEX);
    (*data)->SetLuaReference(ref);
    (*data)->GetLuaThread()->SetUserData(ref);

    return 1; // new userdatum is already on the stack
}

static Character *GetCharacter(lua_State *lua)
{
    void *ud = luaL_checkudata(lua, 1, "D.Character");
    luaL_argcheck(lua, ud != NULL, 1, "'Character' expected");
    return *(Character **)ud;
}

static int DestroyCharacter(lua_State *lua)
{
    DestroyCharacter(GetCharacter(lua));
    return 0;
}

static int DestroyCharacterById(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TNUMBER);    
    DestroyCharacter(state->GetLevel()->GetCharacter(static_cast<int>(lua_tointeger(lua, -1))));
    return 0;
}

static int GetCharacterLuaTable(lua_State *lua)
{
    Character *instance = GetCharacter(lua);
    lua_State *global = state->GetInterpreter()->GetState();
    instance->GetLuaTable().Push();
    lua_xmove(global, lua, 1);
    return 1;
}

static int SetSelector(lua_State *lua)
{
    Character *instance = GetCharacter(lua);

    luaL_checktype(lua, -5, LUA_TSTRING);
    luaL_checktype(lua, -4, LUA_TNUMBER);
    luaL_checktype(lua, -3, LUA_TNUMBER);
    luaL_checktype(lua, -2, LUA_TNUMBER);
    luaL_checktype(lua, -1, LUA_TNUMBER);

    int   segments    = (int)  lua_tointeger(lua, -4);
    float innerRadius = (float)lua_tonumber (lua, -3),
          outerRadius = (float)lua_tonumber (lua, -2),
          speed       = (float)lua_tonumber (lua, -1);
    Drawable *ring = CreateRing("selector", lua_tostring(lua, -5), segments, innerRadius, outerRadius);
    Selector *s = new Selector(ring, speed);
    instance->SetSelector(s);
    ring->Release();
    return 0;
}

static int RemoveSelector(lua_State *lua)
{
    Character *instance = GetCharacter(lua);
    Selector *s = instance->GetSelector();
    if (s)
    {
        delete s;
        instance->SetSelector(0);
    }
    return 0;
}

// Animation will be lightuserdata
static int GetAnimation(lua_State *lua)
{
    Character *instance = GetCharacter(lua);
    lua_pushlightuserdata(lua, instance->GetAnimation());
    return 1;
}

int CharacterToString(lua_State *lua)
{
    Character *instance = GetCharacter(lua);
    lua_pushfstring(lua, "Character(%p)", instance);
    return 1;
}

static void LuaPushCharacter(lua_State *lua, Character *character)
{
    if (!character)
        lua_pushnil(lua);
    else
    {
        int ref = character->GetLuaReference();
        lua_State *global = state->GetInterpreter()->GetState();
        lua_rawgeti(global, LUA_REGISTRYINDEX, ref);
        lua_xmove(global, lua, 1);
    }
}

// functions:

static int GetPlayer(lua_State *lua)
{
    Character *instance = state->GetPlayer();
    LuaPushCharacter(lua, instance);
    return 1;
}

static int GetClosest(lua_State *lua)
{
    Character *instance = GetCharacter(lua);
    Character *closest = static_cast<Character *>(instance->GetDynamicMap()->GetClosest(instance, GetFloat(lua, -1)));
    LuaPushCharacter(lua, closest);
    return 1;
}

static int GetClosestAttackable(lua_State *lua)
{
    Character *instance = GetCharacter(lua);
    Character *closest = instance->GetClosestAttackable(GetFloat(lua, -1));
    LuaPushCharacter(lua, closest);
    return 1;
}

static int GetPosition(lua_State *lua)
{
    ReturnVector3(lua, GetCharacter(lua)->GetPosition());
    return 3;
}

static int GetId(lua_State *lua)
{
    lua_pushinteger(lua, GetCharacter(lua)->GetId());
    return 1;
}

static int SetId(lua_State *lua)
{
    Character *instance = GetCharacter(lua);
    state->GetLevel()->GetCharacterSync()->SetCharacterId(instance, GetInt(lua, -1));
    return 0;
}

static int SetPosition(lua_State *lua)
{
    Character *instance = GetCharacter(lua);
    instance->SetPosition(GetVector3(lua, -1));
    return 0;
}

static int HasChanged(lua_State *lua)
{
    lua_pushboolean(lua, GetCharacter(lua)->HasChanged());
    return 1;
}

static int MarkChanged(lua_State *lua)
{
    Character *instance = GetCharacter(lua);
    instance->MarkChanged();
    return 0;
}

static int SetPositionRaw(lua_State *lua)
{
    Character *instance = GetCharacter(lua);
    instance->SetPositionRaw(GetVector3(lua, -1));
    return 0;
}

static int SetTranslate(lua_State *lua)
{
    Character *instance = GetCharacter(lua);
    instance->SetTranslate(GetVector3(lua, -1));
    return 0;
}

static int SetScale(lua_State *lua)
{
    Character *instance = GetCharacter(lua);
    instance->SetScale(GetFloat(lua, -1));
    return 0;
}

static int SetRadius(lua_State *lua)
{
    Character *instance = GetCharacter(lua);
    instance->SetRadius(GetFloat(lua, -1));
    return 0;
}

static int GetVelocity(lua_State *lua)
{
    ReturnVector3(lua, GetCharacter(lua)->GetVelocity());
    return 3;
}

static int GetSpeed(lua_State *lua)
{
    lua_pushnumber(lua, GetCharacter(lua)->GetVelocity().length());
    return 1;
}
/*
static int CollidesAt(lua_State *lua)
{
    Character *instance = GetCharacter(lua);
    Vector3 v(GetVector3(lua, -1));
    lua_pushboolean(lua, instance->CollidesAt(v) ? 1 : 0);
    return 1;
}
*/

static int CollidesAtRadius(lua_State *lua)
{
    Character *instance = GetCharacter(lua);
    Vector3 v(GetVector3(lua, -2));
    float r = GetFloat(lua, -1);
    lua_pushinteger(lua, instance->CollidesAt(v, r));
    return 1;
}

static int StaticCollides(lua_State *lua)
{
    Character *instance = GetCharacter(lua);
    Vector3 v(GetVector3(lua, -2)), resolve;
    float r = GetFloat(lua, -1);
    lua_pushboolean(lua, instance->StaticCollides(v, r, resolve));
    lua_pushnumber(lua, resolve.x);
    lua_pushnumber(lua, resolve.y);
    lua_pushnumber(lua, resolve.z);
    return 4;
}

static int IsFacing(lua_State *lua)
{
    lua_pushboolean(lua, GetCharacter(lua)->IsFacing(GetVector3(lua, -2), GetFloat(lua, -1)));
    return 1;
}

// getter float
static int GetOrientation(lua_State *lua)
{
    lua_pushnumber(lua, GetCharacter(lua)->GetOrientationAngle());
    return 1;
}

// getter float
static int CalculateOrientation(lua_State *lua)
{
    lua_pushnumber(lua, GetCharacter(lua)->CalculateOrientationAngle());
    return 1;
}

// setter float
static int SetOrientation(lua_State *lua)
{
    Character *instance = GetCharacter(lua);
    instance->SetOrientationAngle(GetFloat(lua, -1));
    return 0;
}

// setter float
static int SetRotation(lua_State *lua)
{
    Character *instance = GetCharacter(lua);
    instance->SetRotation(GetFloat(lua, -1));
    return 0;
}

// getter float
static int GetAngleOffset(lua_State *lua)
{
    lua_pushnumber(lua, GetCharacter(lua)->GetAngleOffset());
    return 1;
}

static int SetAngleOffset(lua_State *lua)
{
    Character *instance = GetCharacter(lua);
    instance->SetAngleOffset(GetFloat(lua, -1));
    return 0;
}

static int IsVisible(lua_State *lua)
{
    lua_pushboolean(lua, GetCharacter(lua)->IsVisible() ? 1 : 0);
    return 1;
}

static int GetAnimationName(lua_State *lua)
{
    lua_pushstring(lua, GetCharacter(lua)->GetAnimationName());
    return 1;
}

static int SetVisible(lua_State *lua)
{
    Character *instance = GetCharacter(lua);
    instance->SetVisible(GetBool(lua, -1));
    return 0;
}

static int IsMovable(lua_State *lua)
{
    lua_pushboolean(lua, GetCharacter(lua)->IsMovable() ? 1 : 0);
    return 1;
}

static int SetMovable(lua_State *lua)
{
    Character *instance = GetCharacter(lua);
    instance->SetMovable(GetBool(lua, -1));
    return 0;
}

static int IsFixed(lua_State *lua)
{
    lua_pushboolean(lua, GetCharacter(lua)->IsFixed() ? 1 : 0);
    return 1;
}

static int SetFixed(lua_State *lua)
{
    Character *instance = GetCharacter(lua);
    instance->SetFixed(GetBool(lua, -1));
    return 0;
}

static int IsCollidable(lua_State *lua)
{
    lua_pushboolean(lua, GetCharacter(lua)->IsCollidable() ? 1 : 0);
    return 1;
}

static int SetCollidable(lua_State *lua)
{
    Character *instance = GetCharacter(lua);
    instance->SetCollidable(GetBool(lua, -1));
    return 0;
}

static int SetVelocity(lua_State *lua)
{
    Character *instance = GetCharacter(lua);
    Vector3 p(GetVector3(lua, -1));
    instance->SetVelocity(p);
    return 0;
}

static int ResetAnimation(lua_State *lua)
{
    GetCharacter(lua)->ResetAnimation();
    return 0;
}

static int SetAnimation(lua_State *lua)
{
    GetCharacter(lua)->SetAnimation(GetString(lua, -1));
    return 0;
}

static int Uncollide(lua_State *lua)
{
    Character *instance = GetCharacter(lua);
    lua_pushboolean(lua, instance->GetDynamicMap()->Uncollide(instance) ? 1 : 0);
    return 1;
}

static int UncollideAll(lua_State *lua)
{
    lua_pushboolean(lua, state->GetLevel()->UncollideVisible() ? 1 : 0);
    return 1;
}

static int GetCharacterById(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TNUMBER);    
    LuaPushCharacter(lua, state->GetLevel()->GetCharacter(static_cast<int>(lua_tointeger(lua, -1))));
    return 1;
}

static int GetAllCharacters(lua_State *lua)
{
    LuaTable table = LuaTable::New(lua);
    MovableRegistry &registry = state->GetLevel()->GetDynamicMap()->GetRegistry();
    int id = registry.GetNext(-1);
    while (id != -1)
    {
        Character *c = static_cast<Character *>(registry.GetAt(id));
        LuaPushCharacter(lua, c);
        table.AddFromStack();
        id = registry.GetNext(id);
    }
    table.Push();
    return 1;
}

static int GetFreeCharacterId(lua_State *lua)
{ 
    lua_pushinteger(lua, state->GetLevel()->GetFreeCharacterId());
    return 1;
}

static int GetCharacterIdOwner(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TNUMBER);    
    lua_pushinteger(lua, Level::GetCharacterIdOwner(static_cast<int>(lua_tointeger(lua, -1))));
    return 1;
}

// Animation code - these are not members, but they take an animation lightuserdata instance
// as their first arg
static int Animation_Reset(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TLIGHTUSERDATA);
    Animation *animation = reinterpret_cast<Animation *>(lua_touserdata(lua, -1));
    if (animation)
        animation->Reset();
    return 0;
}

static int Animation_SetFrame(lua_State *lua)
{
    luaL_checktype(lua, -2, LUA_TLIGHTUSERDATA);
    luaL_checktype(lua, -1, LUA_TNUMBER);
    Animation *animation = reinterpret_cast<Animation *>(lua_touserdata(lua, -2));
    int frame = (int)lua_tointeger(lua, -1);
    if (animation)
        animation->SetFrame(frame);
    return 0;
}

static int Animation_GetFrame(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TLIGHTUSERDATA);
    Animation *animation = reinterpret_cast<Animation *>(lua_touserdata(lua, -2));
    int n = 0;
    if (animation)
        n = animation->GetFrameNumber();
    lua_pushinteger(lua, n);
    return 1;
}

static int Animation_SetLoopMode(lua_State *lua)
{
    luaL_checktype(lua, -2, LUA_TLIGHTUSERDATA);
    luaL_checktype(lua, -1, LUA_TBOOLEAN);
    Animation *animation = reinterpret_cast<Animation *>(lua_touserdata(lua, -2));
    bool loop = lua_toboolean(lua, -1) != 0;
    if (animation)
        animation->SetLoopMode(loop);
    return 0;
}

static int Animation_SetFrameTime(lua_State *lua)
{
    luaL_checktype(lua, -2, LUA_TLIGHTUSERDATA);
    luaL_checktype(lua, -1, LUA_TNUMBER);
    Animation *animation = reinterpret_cast<Animation *>(lua_touserdata(lua, -2));
    int time = (int)lua_tointeger(lua, -1);
    if (animation)
        animation->SetFrameTime(time);
    return 0;
}

static int Animation_SetAlpha(lua_State *lua)
{
    luaL_checktype(lua, -2, LUA_TLIGHTUSERDATA);
    luaL_checktype(lua, -1, LUA_TNUMBER);
    Animation *animation = reinterpret_cast<Animation *>(lua_touserdata(lua, -2));
    float alpha = (float)lua_tonumber(lua, -1);
    if (animation)
        animation->SetAlpha(alpha);
    return 0;
}

// REGISTRATION CODE
static const struct luaL_reg CharacterLib_f[] =
{
    { "new", NewCharacter },
    { NULL, NULL }
};

static const struct luaL_reg CharacterLib_m[] =
{
    { "__tostring",           CharacterToString },

    { "data",                 GetCharacterLuaTable },

    { "GetId",                GetId },
    { "SetId",                SetId },
    { "MarkChanged",          MarkChanged },
    { "HasChanged",           HasChanged },
    { "SetSelector",          SetSelector },
    { "RemoveSelector",       RemoveSelector },
    { "GetPosition",          GetPosition },
    { "SetPosition",          SetPosition },
    { "SetPositionRaw",       SetPositionRaw },
    { "GetOrientation",       GetOrientation },    
    { "CalculateOrientation", CalculateOrientation },
    { "SetOrientation",       SetOrientation },
    { "SetRotation",          SetRotation },
    { "GetVelocity",          GetVelocity },
    { "SetVelocity",          SetVelocity },
    { "GetSpeed",             GetSpeed },
//  { "CollidesAt",           CollidesAt },
    { "CollidesAtRadius",     CollidesAtRadius },
    { "StaticCollides",       StaticCollides },
    { "ResetAnimation",       ResetAnimation },
    { "SetAnimation",         SetAnimation },
    { "GetAnimation",         GetAnimation },
    { "GetAnimationName",     GetAnimationName },
    { "SetTranslate",         SetTranslate },
    { "SetScale",             SetScale },
    { "SetRadius",            SetRadius },
    { "SetAngleOffset",       SetAngleOffset },
    { "GetAngleOffset",       GetAngleOffset },
    { "Uncollide",            Uncollide },
    { "UncollideAll",         UncollideAll },
    { "IsVisible",            IsVisible },
    { "SetVisible",           SetVisible },
    { "IsMovable",            IsMovable },
    { "SetMovable",           SetMovable },
    { "IsFixed",              IsFixed },
    { "SetFixed",             SetFixed },
    { "IsCollidable",         IsCollidable },
    { "SetCollidable",        SetCollidable },
    { "GetClosest",           GetClosest },
    { "GetClosestAttackable", GetClosestAttackable },
    { "GetPlayer",            GetPlayer },

    { "IsFacing",             IsFacing },
        
    // end
    { NULL, NULL }
};

int luaopen_Character(lua_State *L)
{
    luaL_newmetatable(L, "D.Character");
    
    lua_pushstring(L, "__index");
    lua_pushvalue (L, -2);  // pushes the metatable 
    lua_settable  (L, -3);  // metatable.__index = metatable 
    
    luaL_openlib(L, 0,           CharacterLib_m, 0);
    luaL_openlib(L, "Character", CharacterLib_f, 0);

    // register non-member functions
    lua_register(L, "Animation_Reset",          Animation_Reset);
    lua_register(L, "Animation_SetFrame",       Animation_SetFrame);
    lua_register(L, "Animation_GetFrame",       Animation_GetFrame);
    lua_register(L, "Animation_SetLoopMode",    Animation_SetLoopMode);
    lua_register(L, "Animation_SetFrameTime",   Animation_SetFrameTime);
    lua_register(L, "Animation_SetAlpha",       Animation_SetAlpha);

    lua_register(L, "GetCharacterById",         GetCharacterById);
    lua_register(L, "GetAllCharacters",         GetAllCharacters);
    lua_register(L, "DestroyCharacter",         DestroyCharacter);
    lua_register(L, "DestroyCharacterById",     DestroyCharacterById);    
    lua_register(L, "GetFreeCharacterId",       GetFreeCharacterId);
    lua_register(L, "GetCharacterIdOwner",      GetCharacterIdOwner);
    
    return 1;
}