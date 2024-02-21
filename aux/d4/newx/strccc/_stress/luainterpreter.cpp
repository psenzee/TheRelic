/*************************************************************************************************/
/*!
    \file   luainterpreter.cpp

    $Header: //blaze/games/Madden/2010-NG/trunk/component/franchise/stress/luainterpreter.cpp#2 $
    $Change: 44737 $
    $DateTime: 2009/06/04 14:33:39 $

    \attention
        (c) Electronic Arts Inc. 2009
*/
/*************************************************************************************************/

/*************************************************************************************************/
/*!
    \class LuaInterpreter

    Wraps the Lua interpreter for the purposes of stress testing, and serves as a factory for
    Lua threads.  Note that there will be ONE instance of LuaInterpreter PER OS thread.  This is
    a requirement to maintain thread safety in Lua.

*/
/*************************************************************************************************/

#include "framework/blaze.h"
#include "framework/util/shared/blazestring.h"

#include "luainterpreter.h"
#include "luafunctions.h"
#include "luathread.h"
#include "luastats.h"

extern "C"
{
#include "external/lua/lua.h"
#include "external/lua/lualib.h"
#include "external/lua/lauxlib.h"
}

namespace Blaze
{
namespace Stress
{

/******************************************************************************/
/*! LuaInterpreter :: AppendTimeToFilename

    \brief  Appends date-time at end of filename before extension

    \param  fileName - [Put the description of the parameter 'fileName' here]
    \return          - [Describe the 'char8_t *' return result here.]

    Note : Caller needs pass enough big buffer to fit date-time string appended to filename.
*/
/******************************************************************************/
char8_t *LuaInterpreter::AppendTimeToFilename(char8_t * fileName)
{
    char8_t tempFileName[MAX_FILENAME_SIZE];
    char8_t timeBuffer[128];
    char8_t fileExt[16];

    blaze_strnzcpy(tempFileName, fileName, sizeof(tempFileName));
    char8_t* fileExtPtr = strrchr(tempFileName, '.');
    if(fileExtPtr != NULL)
    {
        // save of the file extension
        blaze_strnzcpy(fileExt, fileExtPtr, sizeof(fileExt));
    }

    TimeValue startTime = TimeValue::getTimeOfDay();

    uint32_t year;
    uint32_t month;
    uint32_t day;
    uint32_t hour;
    uint32_t minute;
    uint32_t second;
    uint32_t millis;

    TimeValue::getTimeComponents(startTime, &year, &month, &day, &hour, &minute, &second, &millis);
    blaze_snzprintf(timeBuffer, sizeof(timeBuffer), "-%d%02d%02d_%02d%02d%02d", year, month, day, hour, minute, second);

    if (fileExtPtr != NULL)
    {
        // append the timebuffer just before file-ext starts,
        blaze_strnzcpy(fileExtPtr, timeBuffer, sizeof(timeBuffer));
        // append the original file extension back to the buffer
        blaze_strnzcat(tempFileName, fileExt, sizeof(tempFileName)-1);
    }
    else
    {
        // otherwise just append date-time string
        blaze_strnzcat(tempFileName, timeBuffer, sizeof(timeBuffer)-1);
    }

    // copy new filename back to in-parameter
    blaze_strnzcpy(fileName, tempFileName, sizeof(tempFileName)-1);
    return fileName;
}

LuaInterpreter::LuaInterpreter() : mState(0)
{   
    Lua::LuaStats::GetInstance()->SetStartTime(TimeValue::getTimeOfDay());

    char filename[MAX_FILENAME_SIZE];
    strcpy(filename, "rpcstats.csv");
    AppendTimeToFilename(filename);
    Lua::LuaStats::GetInstance()->SetStatsFilename(filename);
    strcpy(filename, "errorstats.csv");
    AppendTimeToFilename(filename);
    Lua::LuaStats::GetInstance()->SetErrorsFilename(filename);
}

// GetState() returns the underlying lua_State instance.
lua_State *LuaInterpreter::GetState()
{
    return mState;
}

// CreateThread() creates a new lua 'fiber' and initializes it.
LuaThread *LuaInterpreter::CreateThread(void *userContainer)
{
    LuaThread *thread = 0;
    if (mState)
    {
        thread = new LuaThread(*this);
        thread->SetContainer(userContainer);
    }
    return thread;
}

// RunFile() initializes, runs the lua script and sets LuaStats::SetLuaFile(..)
bool LuaInterpreter::RunFile(const char *filename)
{
    if (!filename)
        return false;

    Lua::LuaStats::GetInstance()->SetLuaFile(filename);

    mState = lua_open(); 

    // load the libs  
    luaL_openlibs(mState);  

    int top = lua_gettop(mState);
    RegisterLuaFunctions(mState);

    top = lua_gettop(mState);

    if (luaL_dofile(mState, filename) != 0)
    {
        top = lua_gettop(mState);
        printf("Error: %s\n",lua_tostring(mState, -1));
        lua_pop(mState, lua_gettop(mState));
        //what should I do if the script is invalid?
        //how do I terminate the stress test tool?
        ASSERT(false);
        return false;
    }
    return true;
}

LuaInterpreter::~LuaInterpreter()
{
    if (mState)
    {
        // Verify the stack
        ASSERT(lua_gettop(mState) == 0);
        lua_close(mState);  
    }
}

} // Stress
} // Blaze
