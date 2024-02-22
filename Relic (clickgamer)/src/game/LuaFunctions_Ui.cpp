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

#include "GameState.h"


#include "render/QuadRenderer.h"
#include "render/GraphicsDevice.h"
#include "ui/GameUI.h"
#include "ui/UiCore.h"
#include "ui/Ui.h"
#include "ui/IUiControl.h"

#include "fast/Allocator.h"

static int Ui_SendRisingMessage(lua_State *lua)
{
    luaL_checktype(lua, -2, LUA_TSTRING);
    luaL_checktype(lua, -1, LUA_TNUMBER);
    state->GetGameUi().SendBallMessage(lua_tostring(lua, -2), static_cast<int>(lua_tointeger(lua, -1)));
    return 0;
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
    state->GetGameUi().DrawString(GetGlobalRenderContext(), text, Vector2(x, y), Vector4(1.f, 1.f, 1.f, alpha), size);
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
    state->GetGameUi().DrawString(GetGlobalRenderContext(), text, Vector2(x, y), Vector4(1.f, 1.f, 1.f, alpha), size, true, GlyphWriter::TEXT_JUSTIFY_CENTER);
    return 0;
}

static int Text_DrawTextCentered(lua_State *lua)
{
    return Text_DrawStringCentered(lua);
}

static int Text_GetSize(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TNUMBER);
    luaL_checktype(lua, -2, LUA_TSTRING);
    const char *text  = lua_tostring(lua, -2);
    float scale = static_cast<float>(lua_tonumber(lua, -1));
    Vector2 size(state->GetGameUi().GetSize(text, Vector2(scale, scale)));
    lua_pushnumber(lua, size.x);
    lua_pushnumber(lua, size.y);
    return 2;
}

static IUiControl *GetUiControl(lua_State *lua, int index)
{
    luaL_checktype(lua, index, LUA_TLIGHTUSERDATA); // control name, used for event handler
    return static_cast<IUiControl *>(lua_touserdata(lua, index));
}

static int Ui_DrawString(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TNUMBER);
    luaL_checktype(lua, -2, LUA_TNUMBER);
    luaL_checktype(lua, -3, LUA_TNUMBER);
    luaL_checktype(lua, -4, LUA_TNUMBER);
    luaL_checktype(lua, -5, LUA_TSTRING);
    const char *text  = lua_tostring(lua, -5);
    float x    = static_cast<float>(lua_tonumber(lua, -4)), y     = static_cast<float>(lua_tonumber(lua, -3)),
          size = static_cast<float>(lua_tonumber(lua, -2)), alpha = static_cast<float>(lua_tonumber(lua, -1));
    state->GetUiCore()->DrawString(text, Vector2(x, y), JUSTIFY_LEFT, alpha, size);
    return 0;
}

static int Ui_DrawStringGlow(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TNUMBER);
    luaL_checktype(lua, -2, LUA_TNUMBER);
    luaL_checktype(lua, -3, LUA_TNUMBER);
    luaL_checktype(lua, -4, LUA_TNUMBER);
    luaL_checktype(lua, -5, LUA_TSTRING);
    const char *text  = lua_tostring(lua, -5);
    float x    = static_cast<float>(lua_tonumber(lua, -4)), y     = static_cast<float>(lua_tonumber(lua, -3)),
          size = static_cast<float>(lua_tonumber(lua, -2)), alpha = static_cast<float>(lua_tonumber(lua, -1));
    state->GetUiCore()->DrawStringGlow(text, Vector2(x, y), JUSTIFY_LEFT, alpha, size);
    return 0;
}

static int Ui_DrawStringCentered(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TNUMBER);
    luaL_checktype(lua, -2, LUA_TNUMBER);
    luaL_checktype(lua, -3, LUA_TNUMBER);
    luaL_checktype(lua, -4, LUA_TNUMBER);
    luaL_checktype(lua, -5, LUA_TSTRING);
    const char *text  = lua_tostring(lua, -5);
    float x    = static_cast<float>(lua_tonumber(lua, -4)), y     = static_cast<float>(lua_tonumber(lua, -3)),
          size = static_cast<float>(lua_tonumber(lua, -2)), alpha = static_cast<float>(lua_tonumber(lua, -1));
    state->GetUiCore()->DrawString(text, Vector2(x, y), JUSTIFY_CENTER, alpha, size);
    return 0;
}

static int Ui_DrawStringCenteredGlow(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TNUMBER);
    luaL_checktype(lua, -2, LUA_TNUMBER);
    luaL_checktype(lua, -3, LUA_TNUMBER);
    luaL_checktype(lua, -4, LUA_TNUMBER);
    luaL_checktype(lua, -5, LUA_TSTRING);
    const char *text  = lua_tostring(lua, -5);
    float x    = static_cast<float>(lua_tonumber(lua, -4)), y     = static_cast<float>(lua_tonumber(lua, -3)),
          size = static_cast<float>(lua_tonumber(lua, -2)), alpha = static_cast<float>(lua_tonumber(lua, -1));
    state->GetUiCore()->DrawStringGlow(text, Vector2(x, y), JUSTIFY_CENTER, alpha, size);
    return 0;
}

static int Ui_GetStringSize(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TNUMBER);
    luaL_checktype(lua, -2, LUA_TSTRING);
    const char *text  = lua_tostring(lua, -2);
    float scale = static_cast<float>(lua_tonumber(lua, -1));
    Vector2 size(state->GetUiCore()->GetStringSize(text, scale));
    lua_pushnumber(lua, size.x);
    lua_pushnumber(lua, size.y);
    return 2;
}

static int Ui_DrawQuad(lua_State *lua)
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

    Vector4        color   = Vector4(1.0, 1.0, 1.0, (float)lua_tonumber(lua, -11));
    Vector3        start   = Vector3((float)lua_tonumber(lua, -10), (float)lua_tonumber(lua, -9), (float)lua_tonumber(lua, -8));
    Vector3        end     = Vector3((float)lua_tonumber(lua, -7),  (float)lua_tonumber(lua, -6), (float)lua_tonumber(lua, -5));
    Vector2        t0      = Vector2((float)lua_tonumber(lua, -4),  (float)lua_tonumber(lua, -3)),
                   t1      = Vector2((float)lua_tonumber(lua, -2),  (float)lua_tonumber(lua, -1));

    state->GetUiCore()->DrawQuad(lua_tostring(lua, -12), color, v2(start), v2(end), t0, t1);
    return 0;
}

static int Ui_DrawRing(lua_State *lua)
{
    luaL_checktype(lua, -8, LUA_TSTRING); // texture name
    luaL_checktype(lua, -7, LUA_TNUMBER); // alpha
    luaL_checktype(lua, -6, LUA_TNUMBER); // at.x
    luaL_checktype(lua, -5, LUA_TNUMBER); // at.y
    luaL_checktype(lua, -4, LUA_TNUMBER); // at.z	
    luaL_checktype(lua, -3, LUA_TNUMBER); // inner radius
    luaL_checktype(lua, -2, LUA_TNUMBER); // outer radius
    luaL_checktype(lua, -1, LUA_TNUMBER); // angle
	
    Vector4 color = Vector4(1.0, 1.0, 1.0, (float)lua_tonumber(lua, -7));
	
    state->GetUiCore()->DrawRing(lua_tostring(lua, -8), color, 
								 Vector3((float)lua_tonumber(lua, -6), (float)lua_tonumber(lua, -5), (float)lua_tonumber(lua, -4)),
								 (float)lua_tonumber(lua, -3),   // inner radius
								 (float)lua_tonumber(lua, -2),   // outer radius
								 (float)lua_tonumber(lua, -1));  // angle
    return 0;
}

static int Ui_DrawQuadRotate(lua_State *lua)
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

    Vector4        color   = Vector4(1.0, 1.0, 1.0, (float)lua_tonumber(lua, -12));
    Vector3        start   = Vector3((float)lua_tonumber(lua, -11), (float)lua_tonumber(lua, -10), (float)lua_tonumber(lua, -9));
    Vector3        end     = Vector3((float)lua_tonumber(lua, -8),  (float)lua_tonumber(lua, -7),  (float)lua_tonumber(lua, -6));
    Vector2        t0      = Vector2((float)lua_tonumber(lua, -5),  (float)lua_tonumber(lua, -4)),
                   t1      = Vector2((float)lua_tonumber(lua, -3),  (float)lua_tonumber(lua, -2));
    float          angle   = (float)lua_tonumber(lua, -1);

    state->GetUiCore()->DrawQuad(lua_tostring(lua, -13), color, v2(start), v2(end), t0, t1, angle);

    return 0;
}

static int Ui_DrawPanorama(lua_State *lua)
{
    luaL_checktype(lua, -9,  LUA_TSTRING); // texture name
    luaL_checktype(lua, -8,  LUA_TNUMBER); // alpha
    luaL_checktype(lua, -7,  LUA_TNUMBER); // start.x
    luaL_checktype(lua, -6,  LUA_TNUMBER); // start.y
    luaL_checktype(lua, -5,  LUA_TNUMBER); // start.z

    luaL_checktype(lua, -4,  LUA_TNUMBER); // end.x 
    luaL_checktype(lua, -3,  LUA_TNUMBER); // end.y
    luaL_checktype(lua, -2,  LUA_TNUMBER); // end.z

    luaL_checktype(lua, -1,  LUA_TNUMBER); // angle

    Vector4        color   = Vector4(1.0, 1.0, 1.0, (float)lua_tonumber(lua, -8));
    Vector3        start   = Vector3((float)lua_tonumber(lua, -7), (float)lua_tonumber(lua, -6), (float)lua_tonumber(lua, -5));
    Vector3        end     = Vector3((float)lua_tonumber(lua, -4), (float)lua_tonumber(lua, -3), (float)lua_tonumber(lua, -2));
    float          angle   = (float)lua_tonumber(lua, -1);

    state->GetUiCore()->DrawPanorama(lua_tostring(lua, -9), color, v2(start), v2(end), angle);

    return 0;
}

static int Ui_Draw9Slice(lua_State *lua)
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

    Vector4        color       = Vector4(1.0, 1.0, 1.0, (float)lua_tonumber(lua, -9));
    Vector3        start       = Vector3((float)lua_tonumber(lua, -8), (float)lua_tonumber(lua, -7), (float)lua_tonumber(lua, -6));
    Vector3        end         = Vector3((float)lua_tonumber(lua, -5), (float)lua_tonumber(lua, -4), (float)lua_tonumber(lua, -3));
    float          cornerSize  = (float)lua_tonumber(lua, -2);
    float          middleRange = (float)lua_tonumber(lua, -1);

    state->GetUiCore()->Draw9SliceQuad(lua_tostring(lua, -10), color, v2(start), v2(end), cornerSize, middleRange);
    return 0;
}

static int Ui_SetRootControl(lua_State *lua)
{
    IUiControl *control = GetUiControl(lua, -1);
    state->GetUiCore()->SetRootControl(control);
    return 0;
}

static int Ui_GetRootControl(lua_State *lua)
{
    IUiControl *control = state->GetUiCore()->GetRootControl();
    lua_pushlightuserdata(lua, control);
    return 1;
}

static int Ui_CreateText(lua_State *lua)
{  
    luaL_checktype(lua, -9,  LUA_TSTRING); // control name, used for event handler
    luaL_checktype(lua, -8,  LUA_TSTRING); // control text - this is a text after all

    luaL_checktype(lua, -7,  LUA_TNUMBER); // start.x
    luaL_checktype(lua, -6,  LUA_TNUMBER); // start.y

    luaL_checktype(lua, -5,  LUA_TNUMBER); // end.x 
    luaL_checktype(lua, -4,  LUA_TNUMBER); // end.y

    luaL_checktype(lua, -3,  LUA_TNUMBER); // alpha
    luaL_checktype(lua, -2,  LUA_TNUMBER); // size

    luaL_checktype(lua, -1,  LUA_TBOOLEAN); // center

    UiBounds bounds;
    bounds.minimum = Vector2((float)lua_tonumber(lua, -7), (float)lua_tonumber(lua, -6));
    bounds.maximum = Vector2((float)lua_tonumber(lua, -5), (float)lua_tonumber(lua, -4));
    IUiControl *control = state->GetUiCore()->CreateText(
                              lua_tostring(lua, -9), lua_tostring(lua, -8), bounds,
                              (float)lua_tonumber(lua, -3), (float)lua_tonumber(lua, -2), 
                              lua_toboolean(lua, -1) != 0);
    lua_pushlightuserdata(lua, control);
    return 1;
}

static int Ui_CreateDialogText(lua_State *lua)
{  
    luaL_checktype(lua, -9,  LUA_TSTRING); // control name, used for event handler
    luaL_checktype(lua, -8,  LUA_TSTRING); // control text - this is a text after all

    luaL_checktype(lua, -7,  LUA_TNUMBER); // start.x
    luaL_checktype(lua, -6,  LUA_TNUMBER); // start.y

    luaL_checktype(lua, -5,  LUA_TNUMBER); // end.x 
    luaL_checktype(lua, -4,  LUA_TNUMBER); // end.y

    luaL_checktype(lua, -3,  LUA_TNUMBER); // alpha
    luaL_checktype(lua, -2,  LUA_TNUMBER); // size

    luaL_checktype(lua, -1,  LUA_TBOOLEAN); // center

    UiBounds bounds;
    bounds.minimum = Vector2((float)lua_tonumber(lua, -7), (float)lua_tonumber(lua, -6));
    bounds.maximum = Vector2((float)lua_tonumber(lua, -5), (float)lua_tonumber(lua, -4));
    IUiControl *control = state->GetUiCore()->CreateDialogText(
                              lua_tostring(lua, -9), lua_tostring(lua, -8), bounds,
                              (float)lua_tonumber(lua, -3), (float)lua_tonumber(lua, -2), 
                              lua_toboolean(lua, -1) != 0);
    lua_pushlightuserdata(lua, control);
    return 1;
}

static int Ui_CreateButton(lua_State *lua)
{  
    luaL_checktype(lua, -9,  LUA_TSTRING); // control name, used for event handler
    luaL_checktype(lua, -8,  LUA_TSTRING); // button 9-slice image filename
    luaL_checktype(lua, -7,  LUA_TSTRING); // control text - this is a text after all

    luaL_checktype(lua, -6,  LUA_TNUMBER); // start.x
    luaL_checktype(lua, -5,  LUA_TNUMBER); // start.y

    luaL_checktype(lua, -4,  LUA_TNUMBER); // end.x 
    luaL_checktype(lua, -3,  LUA_TNUMBER); // end.y

    luaL_checktype(lua, -2,  LUA_TNUMBER); // alpha
    luaL_checktype(lua, -1,  LUA_TNUMBER); // size

    UiBounds bounds;
    bounds.minimum = Vector2((float)lua_tonumber(lua, -6), (float)lua_tonumber(lua, -5));
    bounds.maximum = Vector2((float)lua_tonumber(lua, -4), (float)lua_tonumber(lua, -3));
    IUiControl *control = state->GetUiCore()->CreateButton(
                              lua_tostring(lua, -9), lua_tostring(lua, -8), lua_tostring(lua, -7), JUSTIFY_CENTER, bounds,
                              (float)lua_tonumber(lua, -2), (float)lua_tonumber(lua, -1));
    lua_pushlightuserdata(lua, control);
    return 1;
}

static int Ui_CreateImageButton(lua_State *lua)
{  
    luaL_checktype(lua, -14, LUA_TSTRING); // control name, used for event handler
    luaL_checktype(lua, -13, LUA_TSTRING); // image filename
    luaL_checktype(lua, -12, LUA_TSTRING); // control text - this is a text after all

    luaL_checktype(lua, -11, LUA_TNUMBER); // start.x
    luaL_checktype(lua, -10, LUA_TNUMBER); // start.y

    luaL_checktype(lua, -9,  LUA_TNUMBER); // end.x 
    luaL_checktype(lua, -8,  LUA_TNUMBER); // end.y

    luaL_checktype(lua, -7,  LUA_TNUMBER); // alpha
    luaL_checktype(lua, -6,  LUA_TNUMBER); // size

    luaL_checktype(lua, -5,  LUA_TNUMBER); // t0.u
    luaL_checktype(lua, -4,  LUA_TNUMBER); // t0.v

    luaL_checktype(lua, -3,  LUA_TNUMBER); // t1.u
    luaL_checktype(lua, -2,  LUA_TNUMBER); // t1.v

    luaL_checktype(lua, -1,  LUA_TNUMBER); // angle

    UiBounds bounds;
    bounds.minimum = Vector2((float)lua_tonumber(lua, -11), (float)lua_tonumber(lua, -10));
    bounds.maximum = Vector2((float)lua_tonumber(lua, -9),  (float)lua_tonumber(lua, -8));
    Vector2               t0((float)lua_tonumber(lua, -5),  (float)lua_tonumber(lua, -4)),
                          t1((float)lua_tonumber(lua, -3),  (float)lua_tonumber(lua, -2));
    IUiControl *control = state->GetUiCore()->CreateImageButton(
                              lua_tostring(lua, -14), lua_tostring(lua, -13), lua_tostring(lua, -12), JUSTIFY_CENTER, bounds,
                              (float)lua_tonumber(lua, -7), (float)lua_tonumber(lua, -6),
                              t0, t1,                              
                              (float)lua_tonumber(lua, -1));
    lua_pushlightuserdata(lua, control);
    return 1;
}

static int Ui_CreateRenderableButton(lua_State *lua)
{  
    luaL_checktype(lua, -11, LUA_TSTRING); // control name, used for event handler
    luaL_checktype(lua, -10, LUA_TSTRING); // renderable item name
    luaL_checktype(lua, -9, LUA_TSTRING); // control text - this is a text after all

    luaL_checktype(lua, -8, LUA_TNUMBER); // start.x
    luaL_checktype(lua, -7, LUA_TNUMBER); // start.y

    luaL_checktype(lua, -6,  LUA_TNUMBER); // end.x 
    luaL_checktype(lua, -5,  LUA_TNUMBER); // end.y

    luaL_checktype(lua, -4,  LUA_TNUMBER); // alpha
    luaL_checktype(lua, -3,  LUA_TNUMBER); // size

    luaL_checktype(lua, -2,  LUA_TNUMBER); // scale
    luaL_checktype(lua, -1,  LUA_TNUMBER); // angle

    UiBounds bounds;
    bounds.minimum = Vector2((float)lua_tonumber(lua, -8), (float)lua_tonumber(lua, -7));
    bounds.maximum = Vector2((float)lua_tonumber(lua, -6),  (float)lua_tonumber(lua, -5));
    IUiControl *control = state->GetUiCore()->CreateRenderableButton(
                              lua_tostring(lua, -11), lua_tostring(lua, -10), lua_tostring(lua, -9), JUSTIFY_CENTER, bounds,
                              (float)lua_tonumber(lua, -4), (float)lua_tonumber(lua, -3),
                              (float)lua_tonumber(lua, -2), (float)lua_tonumber(lua, -1));
    lua_pushlightuserdata(lua, control);
    return 1;
}

static int Ui_CreateButtonJustified(lua_State *lua)
{  
    luaL_checktype(lua, -10, LUA_TSTRING); // control name, used for event handler
    luaL_checktype(lua, -9,  LUA_TSTRING); // button 9-slice image filename
    luaL_checktype(lua, -8,  LUA_TSTRING); // control text - this is a text after all

    luaL_checktype(lua, -7,  LUA_TNUMBER); // justify

    luaL_checktype(lua, -6,  LUA_TNUMBER); // start.x
    luaL_checktype(lua, -5,  LUA_TNUMBER); // start.y

    luaL_checktype(lua, -4,  LUA_TNUMBER); // end.x 
    luaL_checktype(lua, -3,  LUA_TNUMBER); // end.y

    luaL_checktype(lua, -2,  LUA_TNUMBER); // alpha
    luaL_checktype(lua, -1,  LUA_TNUMBER); // size

    UiBounds bounds;
    bounds.minimum = Vector2((float)lua_tonumber(lua, -6), (float)lua_tonumber(lua, -5));
    bounds.maximum = Vector2((float)lua_tonumber(lua, -4), (float)lua_tonumber(lua, -3));
    IUiControl *control = state->GetUiCore()->CreateButton(
                              lua_tostring(lua, -10), lua_tostring(lua, -9), lua_tostring(lua, -8), 
							  (UiTextJustify)lua_tointeger(lua, -7), bounds,
                              (float)lua_tonumber(lua, -2), (float)lua_tonumber(lua, -1));
    lua_pushlightuserdata(lua, control);
    return 1;
}

static int Ui_CreateImageButtonJustified(lua_State *lua)
{  
    luaL_checktype(lua, -15, LUA_TSTRING); // control name, used for event handler
    luaL_checktype(lua, -14, LUA_TSTRING); // image filename
    luaL_checktype(lua, -13, LUA_TSTRING); // control text - this is a text after all

    luaL_checktype(lua, -12, LUA_TNUMBER); // justify

    luaL_checktype(lua, -11, LUA_TNUMBER); // start.x
    luaL_checktype(lua, -10, LUA_TNUMBER); // start.y

    luaL_checktype(lua, -9,  LUA_TNUMBER); // end.x 
    luaL_checktype(lua, -8,  LUA_TNUMBER); // end.y

    luaL_checktype(lua, -7,  LUA_TNUMBER); // alpha
    luaL_checktype(lua, -6,  LUA_TNUMBER); // size

    luaL_checktype(lua, -5,  LUA_TNUMBER); // t0.u
    luaL_checktype(lua, -4,  LUA_TNUMBER); // t0.v

    luaL_checktype(lua, -3,  LUA_TNUMBER); // t1.u
    luaL_checktype(lua, -2,  LUA_TNUMBER); // t1.v

    luaL_checktype(lua, -1,  LUA_TNUMBER); // angle

    UiBounds bounds;
    bounds.minimum = Vector2((float)lua_tonumber(lua, -11), (float)lua_tonumber(lua, -10));
    bounds.maximum = Vector2((float)lua_tonumber(lua, -9),  (float)lua_tonumber(lua, -8));
    Vector2               t0((float)lua_tonumber(lua, -5),  (float)lua_tonumber(lua, -4)),
                          t1((float)lua_tonumber(lua, -3),  (float)lua_tonumber(lua, -2));
    IUiControl *control = state->GetUiCore()->CreateImageButton(
                              lua_tostring(lua, -15), lua_tostring(lua, -14), lua_tostring(lua, -13), 
							  (UiTextJustify)lua_tointeger(lua, -12), bounds,
                              (float)lua_tonumber(lua, -7), (float)lua_tonumber(lua, -6),
                              t0, t1,                              
                              (float)lua_tonumber(lua, -1));
    lua_pushlightuserdata(lua, control);
    return 1;
}

static int Ui_CreateRenderableButtonJustified(lua_State *lua)
{  
    luaL_checktype(lua, -12, LUA_TSTRING); // control name, used for event handler
    luaL_checktype(lua, -11, LUA_TSTRING); // renderable item name
    luaL_checktype(lua, -10, LUA_TSTRING); // control text - this is a text after all

    luaL_checktype(lua, -9,  LUA_TNUMBER); // justify

    luaL_checktype(lua, -8,  LUA_TNUMBER); // start.x
    luaL_checktype(lua, -7,  LUA_TNUMBER); // start.y

    luaL_checktype(lua, -6,  LUA_TNUMBER); // end.x 
    luaL_checktype(lua, -5,  LUA_TNUMBER); // end.y

    luaL_checktype(lua, -4,  LUA_TNUMBER); // alpha
    luaL_checktype(lua, -3,  LUA_TNUMBER); // size

    luaL_checktype(lua, -2,  LUA_TNUMBER); // scale
    luaL_checktype(lua, -1,  LUA_TNUMBER); // angle

    UiBounds bounds;
    bounds.minimum = Vector2((float)lua_tonumber(lua, -8), (float)lua_tonumber(lua, -7));
    bounds.maximum = Vector2((float)lua_tonumber(lua, -6), (float)lua_tonumber(lua, -5));
    IUiControl *control = state->GetUiCore()->CreateRenderableButton(
                              lua_tostring(lua, -12), lua_tostring(lua, -11), lua_tostring(lua, -10), 
							  (UiTextJustify)lua_tointeger(lua, -9), bounds,
                              (float)lua_tonumber(lua, -4), (float)lua_tonumber(lua, -3),
                              (float)lua_tonumber(lua, -2), (float)lua_tonumber(lua, -1));
    lua_pushlightuserdata(lua, control);
    return 1;
}

static int Ui_CreateMenu(lua_State *lua)
{  
    luaL_checktype(lua, -9,  LUA_TSTRING); // control name, used for event handler
    luaL_checktype(lua, -8,  LUA_TSTRING); // button 9-slice image filename
    luaL_checktype(lua, -7,  LUA_TSTRING); // control text - this is a text after all

    luaL_checktype(lua, -6,  LUA_TNUMBER); // start.x
    luaL_checktype(lua, -5,  LUA_TNUMBER); // start.y

    luaL_checktype(lua, -4,  LUA_TNUMBER); // end.x 
    luaL_checktype(lua, -3,  LUA_TNUMBER); // end.y

    luaL_checktype(lua, -2,  LUA_TNUMBER); // alpha
    luaL_checktype(lua, -1,  LUA_TNUMBER); // size

    UiBounds bounds;
    bounds.minimum = Vector2((float)lua_tonumber(lua, -6), (float)lua_tonumber(lua, -5));
    bounds.maximum = Vector2((float)lua_tonumber(lua, -4), (float)lua_tonumber(lua, -3));
    IUiControl *control = state->GetUiCore()->CreateMenu(
                              lua_tostring(lua, -9), lua_tostring(lua, -8), lua_tostring(lua, -7), bounds,
                              (float)lua_tonumber(lua, -2), (float)lua_tonumber(lua, -1));
    lua_pushlightuserdata(lua, control);
    return 1;
}

static int Ui_NotifyInput(lua_State *lua)
{
    luaL_checktype(lua, -4,  LUA_TSTRING); // event type string
    luaL_checktype(lua, -3,  LUA_TNUMBER); // touch index
    luaL_checktype(lua, -2,  LUA_TNUMBER); // at.x
    luaL_checktype(lua, -1,  LUA_TNUMBER); // at.y
    UiEvent event(UiEvent::GetEventTypeFromName(lua_tostring(lua, -4)), /*(int)lua_tointeger(lua, -3),*/
                  Vector2((float)lua_tonumber(lua, -2), (float)lua_tonumber(lua, -1)), "");
    state->GetUiCore()->Notify(event);
    return 0;
}

static int Ui_ScreenToDrawPosition(lua_State *lua)
{
    luaL_checktype(lua, -2,  LUA_TNUMBER); // at.x
    luaL_checktype(lua, -1,  LUA_TNUMBER); // at.y
    Vector2 pos = state->GetUiCore()->ScreenToDrawPosition(Vector2((float)lua_tonumber(lua, -2), (float)lua_tonumber(lua, -1)));
    lua_pushnumber(lua, pos.x);
    lua_pushnumber(lua, pos.y);
    return 2;
}

static int Ui_DrawToScreenPosition(lua_State *lua)
{
    luaL_checktype(lua, -2,  LUA_TNUMBER); // at.x
    luaL_checktype(lua, -1,  LUA_TNUMBER); // at.y
    Vector2 pos = state->GetUiCore()->DrawToScreenPosition(Vector2((float)lua_tonumber(lua, -2), (float)lua_tonumber(lua, -1)));
    lua_pushnumber(lua, pos.x);
    lua_pushnumber(lua, pos.y);
    return 2;
}

static int UiControl_GetName(lua_State *lua)
{
    IUiControl *control = GetUiControl(lua, -1);
    lua_pushstring(lua, control->GetName());
    return 1;
}

static int UiControl_SetName(lua_State *lua)
{
    IUiControl *control = GetUiControl(lua, -2);
    luaL_checktype(lua, -1,  LUA_TSTRING); // event type string
    control->SetName(lua_tostring(lua, -1));
    return 0;
}

static int UiControl_Destroy(lua_State *lua)
{
    IUiControl *control = GetUiControl(lua, -1);
    UiCore::Destroy(control);
    return 0;
}

static int UiControl_GetText(lua_State *lua)
{
    IUiControl *control = GetUiControl(lua, -1);
    lua_pushstring(lua, control->GetText());
    return 1;
}

static int UiControl_SetText(lua_State *lua)
{
    IUiControl *control = GetUiControl(lua, -2);
    luaL_checktype(lua, -1,  LUA_TSTRING); // event type string
    control->SetText(lua_tostring(lua, -1));
    return 0;
}

static int UiControl_GetData(lua_State *lua)
{
    IUiControl *control = GetUiControl(lua, -1);
    lua_pushlightuserdata(lua, control->GetData());
    return 1;
}

static int UiControl_SetData(lua_State *lua)
{
    IUiControl *control = GetUiControl(lua, -2);
    luaL_checktype(lua, -1,  LUA_TLIGHTUSERDATA);
    control->SetData(lua_touserdata(lua, -1));
    return 0;
}

static int UiControl_IsVisible(lua_State *lua)
{
    IUiControl *control = GetUiControl(lua, -1);
    lua_pushboolean(lua, control->IsVisible() ? 1 : 0);
    return 1;
}

static int UiControl_SetVisible(lua_State *lua)
{
    IUiControl *control = GetUiControl(lua, -2);
    luaL_checktype(lua, -1, LUA_TBOOLEAN); 
    control->SetVisible(lua_toboolean(lua, -1) != 0);
    return 0;
}

static int UiControl_IsActive(lua_State *lua)
{
    IUiControl *control = GetUiControl(lua, -1);
    lua_pushboolean(lua, control->IsActive() ? 1 : 0);
    return 1;
}

static int UiControl_SetActive(lua_State *lua)
{
    IUiControl *control = GetUiControl(lua, -2);
    luaL_checktype(lua, -1, LUA_TBOOLEAN);
    control->SetActive(lua_toboolean(lua, -1) != 0);
    return 0;
}

static int UiControl_GetAnimationTime(lua_State *lua)
{
    IUiControl *control = GetUiControl(lua, -1);
    lua_pushnumber(lua, control->GetAnimationTime());
    return 1;
}

static int UiControl_SetAnimationTime(lua_State *lua)
{
    IUiControl *control = GetUiControl(lua, -2);
    luaL_checktype(lua, -1, LUA_TNUMBER);
    control->SetAnimationTime((float)lua_tonumber(lua, -1));
    return 0;
}

static int UiControl_IsCollection(lua_State *lua)
{
    IUiControl *control = GetUiControl(lua, -1);
    lua_pushboolean(lua, control->IsCollection() ? 1 : 0);
    return 1;
}

static int UiControl_AddChild(lua_State *lua)
{
    IUiControl *control = GetUiControl(lua, -2),
               *add     = GetUiControl(lua, -1);
    control->AddChild(add);
    return 0;
}

static int UiControl_RemoveChild(lua_State *lua)
{
    IUiControl *control = GetUiControl(lua, -2),
               *remove  = GetUiControl(lua, -1);
    lua_pushboolean(lua, control->RemoveChild(remove) ? 1 : 0);
    return 1;
}

static int UiControl_GetParent(lua_State *lua)
{
    IUiControl *control = GetUiControl(lua, -1);
    lua_pushlightuserdata(lua, control->GetParent());
    return 1;
}

static int UiControl_GetChildrenCount(lua_State *lua)
{
    IUiControl *control = GetUiControl(lua, -1);
    lua_pushinteger(lua, control->GetChildrenCount());
    return 1;
}

static int UiControl_GetChildAt(lua_State *lua)
{
    IUiControl *control = GetUiControl(lua, -2);
    luaL_checktype(lua, -1, LUA_TNUMBER);
    lua_pushlightuserdata(lua, control->GetChildAt((int)lua_tointeger(lua, -1)));
    return 1;
}

static int UiControl_GetBounds(lua_State *lua)
{
    IUiControl *control = GetUiControl(lua, -1);
    UiBounds bounds = control->GetBounds();
    lua_pushnumber(lua, bounds.minimum.x);
    lua_pushnumber(lua, bounds.minimum.y);
    lua_pushnumber(lua, bounds.maximum.x);
    lua_pushnumber(lua, bounds.maximum.y);
    return 4;
}

static int UiControl_GetMargin(lua_State *lua)
{
    IUiControl *control = GetUiControl(lua, -1);
    Vector2 margin = control->GetMargin();
    lua_pushnumber(lua, margin.x);
    lua_pushnumber(lua, margin.y);
    return 2;
}

static int UiControl_SetMargin(lua_State *lua)
{
    IUiControl *control = GetUiControl(lua, -5);
    luaL_checktype(lua, -2, LUA_TNUMBER); // margin.x
    luaL_checktype(lua, -1, LUA_TNUMBER); // margin.y
    Vector2 margin;
    margin.x = (float)lua_tonumber(lua, -2);
    margin.y = (float)lua_tonumber(lua, -1);
    control->SetMargin(margin);
    return 0;
}

static int UiControl_RequestBounds(lua_State *lua)
{
    IUiControl *control = GetUiControl(lua, -5);
    luaL_checktype(lua, -4,  LUA_TNUMBER); // bounds.min.x
    luaL_checktype(lua, -3,  LUA_TNUMBER); // bounds.min.y
    luaL_checktype(lua, -2,  LUA_TNUMBER); // bounds.max.x
    luaL_checktype(lua, -1,  LUA_TNUMBER); // bounds.max.y
    UiBounds bounds;
    bounds.minimum.x = (float)lua_tonumber(lua, -4);
    bounds.minimum.y = (float)lua_tonumber(lua, -3);
    bounds.maximum.x = (float)lua_tonumber(lua, -2);
    bounds.maximum.y = (float)lua_tonumber(lua, -1);
    control->RequestBounds(bounds);
    return 0;
}

static int UiControl_LayoutVertical(lua_State *lua)
{
    IUiControl *control = GetUiControl(lua, -1);
    UiCore::LayoutLinear(control, true);
    return 0;
}

static int UiControl_LayoutHorizontal(lua_State *lua)
{
    IUiControl *control = GetUiControl(lua, -1);
    UiCore::LayoutLinear(control, false);
    return 0;
}

static int UiControl_LayoutCircle(lua_State *lua)
{
    IUiControl *control = GetUiControl(lua, -2);
    luaL_checktype(lua, -1,  LUA_TNUMBER);
    UiCore::LayoutCircle(control, float(lua_tonumber(lua, -1)));
    return 0;
}

static int UiControl_CenterAt(lua_State *lua)
{
    IUiControl *control = GetUiControl(lua, -3);
    luaL_checktype(lua, -2,  LUA_TNUMBER);
    luaL_checktype(lua, -1,  LUA_TNUMBER);
    UiCore::Center(control, Vector2(float(lua_tonumber(lua, -2)), float(lua_tonumber(lua, -1))));
    return 0;
}

void RegisterLuaUiFunctions(lua_State *lua)
{
    lua_register(lua, "SendRisingMessage",             Ui_SendRisingMessage);

    lua_register(lua, "Text_DrawString",               Text_DrawString);
    lua_register(lua, "Text_DrawStringCentered",       Text_DrawStringCentered);
    lua_register(lua, "Text_DrawTextCentered",         Text_DrawTextCentered);
    lua_register(lua, "Text_GetSize",                  Text_GetSize);

    lua_register(lua, "Ui_DrawString",                 Ui_DrawString);
    lua_register(lua, "Ui_DrawStringGlow",             Ui_DrawStringGlow);
    lua_register(lua, "Ui_DrawStringCentered",         Ui_DrawStringCentered);
    lua_register(lua, "Ui_DrawStringCenteredGlow",     Ui_DrawStringCenteredGlow);
    lua_register(lua, "Ui_GetStringSize",              Ui_GetStringSize);
    lua_register(lua, "Ui_DrawQuad",                   Ui_DrawQuad);
    lua_register(lua, "Ui_DrawRing",                   Ui_DrawRing);	
    lua_register(lua, "Ui_DrawQuadRotate",             Ui_DrawQuadRotate);
    lua_register(lua, "Ui_DrawPanorama",               Ui_DrawPanorama);
    lua_register(lua, "Ui_Draw9Slice",                 Ui_Draw9Slice);

    lua_register(lua, "Ui_SetRootControl",             Ui_SetRootControl);
    lua_register(lua, "Ui_GetRootControl",             Ui_GetRootControl);
    lua_register(lua, "Ui_CreateText",                 Ui_CreateText);
    lua_register(lua, "Ui_CreateDialogText",           Ui_CreateDialogText);
    lua_register(lua, "Ui_CreateButton",               Ui_CreateButton);
    lua_register(lua, "Ui_CreateImageButton",          Ui_CreateImageButton);
    lua_register(lua, "Ui_CreateRenderableButton",     Ui_CreateRenderableButton);
    lua_register(lua, "Ui_CreateButtonJustified",           Ui_CreateButtonJustified);
    lua_register(lua, "Ui_CreateImageButtonJustified",      Ui_CreateImageButtonJustified);
    lua_register(lua, "Ui_CreateRenderableButtonJustified", Ui_CreateRenderableButtonJustified);
    lua_register(lua, "Ui_CreateMenu",                 Ui_CreateMenu);
    lua_register(lua, "Ui_NotifyInput",                Ui_NotifyInput);

    lua_register(lua, "Ui_ScreenToDrawPosition",       Ui_ScreenToDrawPosition);
    lua_register(lua, "Ui_DrawToScreenPosition",       Ui_DrawToScreenPosition);

    lua_register(lua, "UiControl_GetName",             UiControl_GetName);
    lua_register(lua, "UiControl_SetName",             UiControl_SetName);
    lua_register(lua, "UiControl_GetText",             UiControl_GetText);
    lua_register(lua, "UiControl_SetText",             UiControl_SetText);
    lua_register(lua, "UiControl_GetData",             UiControl_GetData);
    lua_register(lua, "UiControl_SetData",             UiControl_SetData);
    lua_register(lua, "UiControl_IsVisible",           UiControl_IsVisible);
    lua_register(lua, "UiControl_SetVisible",          UiControl_SetVisible);
    lua_register(lua, "UiControl_IsActive",            UiControl_IsActive);
    lua_register(lua, "UiControl_SetActive",           UiControl_SetActive);
    lua_register(lua, "UiControl_GetAnimationTime",    UiControl_GetAnimationTime);
    lua_register(lua, "UiControl_SetAnimationTime",    UiControl_SetAnimationTime);
    lua_register(lua, "UiControl_IsCollection",        UiControl_IsCollection);
    lua_register(lua, "UiControl_AddChild",            UiControl_AddChild);
    lua_register(lua, "UiControl_RemoveChild",         UiControl_RemoveChild);
    lua_register(lua, "UiControl_Destroy",             UiControl_Destroy);
    lua_register(lua, "UiControl_GetParent",           UiControl_GetParent);
    lua_register(lua, "UiControl_GetChildrenCount",    UiControl_GetChildrenCount);
    lua_register(lua, "UiControl_GetChildAt",          UiControl_GetChildAt);
    lua_register(lua, "UiControl_GetBounds",           UiControl_GetBounds);
    lua_register(lua, "UiControl_GetMargin",           UiControl_GetMargin);
    lua_register(lua, "UiControl_SetMargin",           UiControl_SetMargin);
    lua_register(lua, "UiControl_RequestBounds",       UiControl_RequestBounds);
    lua_register(lua, "UiControl_LayoutHorizontal",    UiControl_LayoutHorizontal);
    lua_register(lua, "UiControl_LayoutVertical",      UiControl_LayoutVertical);
    lua_register(lua, "UiControl_LayoutCircle",        UiControl_LayoutCircle);
    lua_register(lua, "UiControl_CenterAt",            UiControl_CenterAt);
}
