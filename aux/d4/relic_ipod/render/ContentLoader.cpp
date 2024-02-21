#include "ContentLoader.h"
#include "DeviceMesh.h"
#include "DeviceTexture.h"

ContentLoader *ContentLoader::sInstance = 0;

ContentLoader::ContentLoader()
{
}

ContentLoader::~ContentLoader()
{
}

DeviceMesh *ContentLoader::GetMesh(const char *name)
{
    DeviceMesh *dm = mModels[name];
    if (!dm)
    {    
        dm = new DeviceMesh(name);
        mModels[name] = dm;
    }
    return dm;
}

DeviceTexture *ContentLoader::GetTexture(const char *name)
{
    DeviceTexture *dt = mTextures[name];
    if (!dt)
    {    
        dt = new DeviceTexture(name);
        mTextures[name] = dt;
    }
    return dt;
}