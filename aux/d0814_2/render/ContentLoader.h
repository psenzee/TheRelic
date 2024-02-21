#ifndef _CONTENTLOADER_H
#define _CONTENTLOADER_H

#include "core/core.h"
#include <map>
#include <vector>

class DeviceMesh;
class DeviceTexture;
class Bundle;

class ContentLoader
{
public:

    bool                  AddBundle(const char *name);
    void                  RemoveBundle(const char *name);

    DeviceMesh           *GetMesh(const char *name);
    void                  PreloadMesh(const char *name);
    void                  UnloadMesh(const char *name); // use with caution to avoid dangling pointers
        
    DeviceTexture        *GetTexture(const char *name);
    void                  PreloadTexture(const char *name);
    void                  UnloadTexture(const char *name); // use with caution to avoid dangling pointers

    static void           CreateInstance()   { if (!sInstance) sInstance = new ContentLoader; }
    static ContentLoader *GetInstance()      { return sInstance; }
    static void           DestroyInstance()  { if (sInstance) delete sInstance; sInstance = 0; }
        
private:

    static ContentLoader *sInstance;

    ContentLoader();
    ~ContentLoader();
 
    struct FileFromBundle
    {
        const char *data;
        int         size;
    };

    Bundle        *FindBundle(const char *name);
    DeviceMesh    *LoadMesh(const char *name);
    DeviceTexture *LoadTexture(const char *name);
    FileFromBundle GetFileFromBundle(const char *name);
    void           LogLoadStart(const char *name, const char *type);
    void           LogLoadEnd(const char *name, bool found, bool bundle);
    void           LogFlush();

    std::map<String, DeviceMesh   *>   mModels;
    std::map<String, DeviceTexture *>  mTextures;
    std::vector<Bundle *>              mBundles;
    FILE                              *mLoadLog;
    FILE                              *mAssetLog;
    FILE                              *mMeshLog;
    FILE                              *mTextureLog;
    unsigned                           mStartTime;
    unsigned                           mEventTime;
    unsigned                           mLastFlush;
};

#endif // _CONTENTLOADER_H
