#include "DeviceTexture.h"
#include "platform/GLUtils.h"
#include "platform/GLIncludes.h"
#include "render/GLStates.h"

DeviceTexture::DeviceTexture(const char *filename) : texid(0), size(0, 0)
{
    char newname[1024];
    sprintf(newname, "%s.png", filename);
    texid = LoadTexture(newname, &size);
}

DeviceTexture::~DeviceTexture()
{
}

bool DeviceTexture::Loaded() const
{
    return texid && glIsTexture(texid);
}

static bool IsAlphaType(int blend)
{
    switch (blend)
    {
    case GL_SRC_ALPHA:
    case GL_ONE_MINUS_SRC_ALPHA:
    case GL_DST_ALPHA:
    case GL_ONE_MINUS_DST_ALPHA:
    case GL_SRC_ALPHA_SATURATE:
#ifdef GL_CONSTANT_COLOR        
    case GL_CONSTANT_ALPHA:
    case GL_ONE_MINUS_CONSTANT_ALPHA:
#endif
        return true;
    default: break;
    }
    return false;
}

void DeviceTexture::SetMipMapped(bool value)
{
    glBindTexture(GL_TEXTURE_2D, texid);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, value ? GL_LINEAR_MIPMAP_NEAREST : GL_LINEAR);
}

void DeviceTexture::Set(GraphicsDevice &device, unsigned int blendsrc, unsigned int blenddst)
{
    if (!Loaded())
        return;
    GLStates::texture.Set(true);
    // Set a blending function to use
    glBlendFunc(blendsrc, blenddst);
    // Enable blending
    GLStates::blend.Set(IsAlphaType(blendsrc) || IsAlphaType(blenddst));
    glBindTexture(GL_TEXTURE_2D, texid);
}
