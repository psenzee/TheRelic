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
    if (Loaded())
    {
        GLuint tid = texid;
        _GLv(glDeleteTextures(1, &tid));
        texid = 0;
    }
}

bool DeviceTexture::Loaded() const
{
    return texid && glIsTexture(texid);
}

void DeviceTexture::SetMipMapped(bool value)
{
    GLBindTexture2d(texid);
    _GLv(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, value ? GL_LINEAR_MIPMAP_NEAREST : GL_LINEAR));
}

void DeviceTexture::Set(GraphicsDevice &device, unsigned int blend_src, unsigned int blend_dst)
{
    if (!Loaded())
        return;
    if (texid != 0)
    {    
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
