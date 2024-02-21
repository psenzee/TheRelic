#ifndef _CONTENTLOADER_H
#define _CONTENTLOADER_H

#include <map>
#include "fast/FastString.h"

class DeviceMesh;
class DeviceTexture;

class ContentLoader
{
public:

    typedef FastString<64> String;
        
    DeviceMesh           *GetMesh(const char *name);
    DeviceTexture        *GetTexture(const char *name);

    static void           CreateInstance()   { if (!sInstance) sInstance = new ContentLoader; }
    static ContentLoader *GetInstance()      { return sInstance; }
    static void           DestroyInstance()  { if (sInstance) delete sInstance; sInstance = 0; }
        
private:

    static ContentLoader *sInstance;

    ContentLoader();
    ~ContentLoader();
        
    std::map<String, DeviceMesh   *>  mModels;
    std::map<String, DeviceTexture *> mTextures;
};

#endif // _CONTENTLOADER_H
