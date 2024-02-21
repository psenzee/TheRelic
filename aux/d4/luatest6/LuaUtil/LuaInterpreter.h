#ifndef _LUAINTERPRETER_H
#define _LUAINTERPRETER_H

struct lua_State;

class LuaThread;

typedef int LuaTable;
typedef int LuaFunction;

class LuaInterpreter
{
public:
   
    bool                   RunFile(const char *filename);
    void                   SetGlobalData(const char *name, void *data);

    LuaThread             *CreateThread(void *userContainer);

    lua_State             *GetState();

    LuaFunction            CreateFunctionReference(const char *name);
    void                   ReleaseFunctionReference(LuaFunction function);

    void                   SetShareThreads(bool shareThreads) { mShareThreads = shareThreads; }
    
    int                    GetThreadCount() const { return mThreadCount; }
    LuaThread             *LastThreadCreated()    { return mLastThreadCreated; }

    static void            CreateInstance()  { if (!mInstance) mInstance = new LuaInterpreter; }
    static LuaInterpreter *GetInstance()     { return mInstance; }
    static void            DestroyInstance() { if (mInstance) delete mInstance; mInstance = 0; }
    
private:

    static LuaInterpreter *mInstance;

    LuaInterpreter();
    ~LuaInterpreter();

    enum { MAX_FILENAME_SIZE = 1024 };

    lua_State *mState;
    int        mThreadCount;
    LuaThread *mLastThreadCreated;
    bool       mShareThreads;
};

#endif // _LUAINTERPRETER_H
