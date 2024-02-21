#ifndef _UIINSTANCE_H
#define _UIINSTANCE_H

class GameTime;
class RenderContext;
class LuaInterpreter;

#include "luautil/LuaThread.h"
#include "luautil/ILuaObject.h"
#include "fast/FixedString.h"

class UiInstance : public ILuaObject
{
public:

    UiInstance();
    ~UiInstance();

    void             SetLuaReference(int id)              { mLuaReference = id; }
    int              GetLuaReference()              const { return mLuaReference; }

    LuaThread       *GetLuaThread()                       { return mLuaThread; }
    LuaTable         GetLuaTable()                  const { return mLuaTable; }
    void             SetLuaTable(const LuaTable &t)       { mLuaTable = t; }

    void             UpdateAndRender(const GameTime &time, RenderContext &context);
    
private:    
    
	LuaThread      *mLuaThread;
    int             mLuaReference;
    LuaTable        mLuaTable;
};

#endif // _UIINSTANCE_H