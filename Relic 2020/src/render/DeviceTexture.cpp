#include "DeviceTexture.h"
#include "GraphicsDevice.h"
#include "render/GLUtils.h"
#include "platform/LoadTexture.h"
#include "render/GLIncludes.h"
#include "render/GLStates.h"
#include "GLAbstract.h"

DeviceTexture::DeviceTexture(const char *filename) : texid(0), size(0, 0)
{
    char name[1024];
    snprintf(name, sizeof(name) - 1, "%s.png", filename);
    texid = LoadTexture(name, &size);
}

DeviceTexture::~DeviceTexture()
{
    printf("Deleting texture %p\n", this);
    if (Loaded()) {
        GLDestroyTexture(texid);
        texid = 0;
    }
}

bool DeviceTexture::Loaded() const
{
    return GLIsTexture(texid);
}

void DeviceTexture::SetMipMapped(bool value)
{
    GLTextureSetMipMapFilters(texid, value);
}

void DeviceTexture::Set(GraphicsDevice &device, unsigned int blend_src, unsigned int blend_dst)
{
    if (Loaded() && texid != 0) {
        GLStates::texture.Set(true);
        if (device.HasGlobalAlpha()) {
            blend_dst = GL_ONE_MINUS_SRC_ALPHA;
        }
        // Set a blending function to use
        GLBlendFunc(blend_src, blend_dst);
        // Enable blending
        GLStates::blend.Set(GLIsBlendAlphaType(blend_src) || GLIsBlendAlphaType(blend_dst));
        GLBindTexture2d(texid);
    }
}
