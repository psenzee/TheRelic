/*************************************************************************************************/
/*!
    \file   luafunctions.h

    $Header: //blaze/games/Madden/2010-NG/trunk/component/franchise/stress/luafunctions.h#2 $
    $Change: 44737 $
    $DateTime: 2009/06/04 14:33:39 $

    \attention
        (c) Electronic Arts Inc. 2009
*/
/*************************************************************************************************/

/*************************************************************************************************/
/*!
    luafunctions.h

    Header for machine generated Lua to C++ bindings and statistics file writing.

*/
/*************************************************************************************************/

#ifndef _LUAFUNCTIONS_H
#define _LUAFUNCTIONS_H

struct lua_State;

namespace Blaze
{
namespace Stress
{

// this is called to register all lua functions with the lua interpreter
void RegisterLuaFunctions(lua_State *lua);  // outside the Blaze::Stress::Lua namespace

} // Stress
} // Blaze

#endif // _LUAFUNCTIONS_H
