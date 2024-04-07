#include "ContentLoader.h"
#include "DeviceMesh.h"
#include "DeviceTexture.h"

#include "core/bundle.h"

#include <vector>
#include <algorithm>

ContentLoader *ContentLoader::sInstance = 0;

extern unsigned int GetCurrentTimeMs();

ContentLoader::ContentLoader() : mLoadLog(0), mAssetLog(0), mTextureLog(0), mMeshLog(0), mStartTime(0), mEventTime(0), mLastFlush(0)
{
    mLoadLog = fopen(globalTranslateReadWritePath("load.log"), "w");
    mAssetLog = fopen(globalTranslateReadWritePath("asset.log"), "w");
    mMeshLog = fopen(globalTranslateReadWritePath("meshes.log"), "w");
    mTextureLog = fopen(globalTranslateReadWritePath("textures.log"), "w");
    mStartTime = GetCurrentTimeMs();
    mLastFlush = mStartTime;
}

ContentLoader::~ContentLoader()
{
    if (mLoadLog)
        fclose(mLoadLog);
    mLoadLog = 0;
    if (mAssetLog)
        fclose(mAssetLog);
    mAssetLog = 0;
    if (mMeshLog)
        fclose(mMeshLog);
    mMeshLog = 0;
    if (mTextureLog)
        fclose(mTextureLog);
    mTextureLog = 0;
}

void ContentLoader::LogFlush()
{
    int time = GetCurrentTimeMs();
    if (time - mLastFlush >= 250)
    {
        if (mLoadLog)
            fflush(mLoadLog);
        if (mAssetLog)
            fflush(mAssetLog);
        if (mTextureLog)
            fflush(mTextureLog);
        if (mMeshLog)
            fflush(mMeshLog);
        mLastFlush = time;
    }
}

void ContentLoader::LogLoadStart(const char *name, const char *type)
{
    if (mLoadLog)
    {
        mEventTime = GetCurrentTimeMs();
        fprintf(mLoadLog, "%08u: [------] Start  Load '%-25s' : type %s\n", mEventTime - mStartTime, name, type);
        fprintf(mAssetLog, "%s\n", name);
        if (strcmp(type, "MESH") == 0)
            fprintf(mMeshLog, "%s\n", name);
        else if (strcmp(type, "TEXTURE") == 0)
            fprintf(mTextureLog, "%s\n", name);
        LogFlush();
    }
}

void ContentLoader::LogLoadEnd(const char *name, bool found, bool bundle)
{
    if (mLoadLog)
    {
        unsigned time = GetCurrentTimeMs();
        fprintf(mLoadLog, "%08u: [%06u] Finish Load '%-25s' : %s %s\n", time - mStartTime, time - mEventTime, name, bundle ? "bundle" : "loose", found ? "" : "MISSING!");
        LogFlush();
    }
}

bool ContentLoader::AddBundle(const char *name)
{
    LogLoadStart(name, "BUNDLE");
    Bundle *bundle = new Bundle;
    unsigned time = GetCurrentTimeMs();
    if (!bundle->Load(name))
    {
        printf("Unable to read bundle '%s'!\n", name);
        delete bundle;
        LogLoadEnd(name, false, false);
        return false;
    }
    printf("Loaded bundle '%s' in %dms\n", name, GetCurrentTimeMs() - time);
    mBundles.push_back(bundle);
    LogLoadEnd(name, true, false);
    return true;
}

ContentLoader::FileFromBundle ContentLoader::GetFileFromBundle(const char *name)
{
    FileFromBundle file; file.size = 0; file.data = 0;
    for (std::vector<Bundle *>::iterator i = mBundles.begin(), e = mBundles.end(); i != e; ++i)
    {
        const char *data = (*i)->GetFile(name);
        if (data)
        {
            // found it!
            file.data = data;
            file.size = (*i)->GetSize(name);
            return file;
        }
    }
    return file;
}

Bundle *ContentLoader::FindBundle(const char *name)
{
    for (std::vector<Bundle *>::iterator i = mBundles.begin(), e = mBundles.end(); i != e; ++i)
        if (*i && strcmp((*i)->GetFileName(), name) == 0)
            return *i;
    return 0;
}

void ContentLoader::PrintLoadedTextures() const
{
	printf("\n\n----------- LOADED TEXTURES ---------------\n");
	int count = 0;
	for (std::map<String, DeviceTexture *>::const_iterator i = mTextures.begin(), e = mTextures.end(); i != e; ++i)
    {
	    if ((*i).second != 0)
		{
		    printf("%s\n", (*i).first.c_str());
	        count++;
		}
	}
	printf(    "----- %d textures -------------------------\n\n", count);	
}

void ContentLoader::RemoveBundle(const char *name)
{
    Bundle *bundle = FindBundle(name);
    if (bundle) {
        mBundles.erase(
           std::remove_if(mBundles.begin(), mBundles.end(), [bundle](auto a) { return a == bundle; }), mBundles.end());
    }
}

DeviceMesh *ContentLoader::LoadMesh(const char *name)
{
    char fname[1024];
    snprintf(fname, sizeof(fname) - 1, "%s.ips", name);
    LogLoadStart(fname, "MESH");
    FileFromBundle file = GetFileFromBundle(fname);
    if (file.data)
    {
        printf(".");
        DeviceMesh *mesh = new DeviceMesh(file.data, file.size);
        LogLoadEnd(name, mesh->IsLoaded(), true);
        return mesh;
    }
    printf("x");
    DeviceMesh *mesh = new DeviceMesh(name, false);
    LogLoadEnd(fname, mesh->IsLoaded(), false);
    return mesh;
}

DeviceTexture *ContentLoader::LoadTexture(const char *name)
{
    /* $TODO - currently can't construct a texture from memory
    FileFromBundle file = GetFileFromBundle(name);
    if (file.data)
        return new DeviceTexture(file.data, file.size);
    */
    LogLoadStart(name, "TEXTURE");
    printf("-");
    DeviceTexture *texture = new DeviceTexture(name);
    LogLoadEnd(name, texture->Loaded(), false);
    return texture;
}

DeviceMesh *ContentLoader::GetMesh(const char *name)
{
    DeviceMesh *dm = mModels[name];
    if (!dm)
    {    
        dm = LoadMesh(name);
        mModels[name] = dm;
    }
    LogFlush();
    return dm;
}

DeviceTexture *ContentLoader::GetTexture(const char *name)
{
    DeviceTexture *dt = mTextures[name];
    if (!dt)
    {    
        dt = LoadTexture(name);
        mTextures[name] = dt;
    }
    LogFlush();
    return dt;
}

void ContentLoader::PreloadTexture(const char *name)
{
    // no magic here
    GetTexture(name);
}

void ContentLoader::UnloadTexture(const char *name)
{
    DeviceTexture *dt = mTextures[name];
    if (dt)
    {
        mTextures.erase(name);
        delete dt;
    }
}

void ContentLoader::UnloadAllTextures()
{
	for (std::map<String, DeviceTexture *>::iterator i = mTextures.begin(), e = mTextures.end(); i != e; ++i)
	{
		DeviceTexture *dt = (*i).second;
		delete dt;
    }
	mTextures.clear();
}

void ContentLoader::PreloadMesh(const char *name)
{
    // no magic here
    GetMesh(name);
}

void ContentLoader::UnloadMesh(const char *name)
{
    DeviceMesh *dm = mModels[name];
    if (dm)
    {
        mModels.erase(name);
        delete dm;
    }
}
