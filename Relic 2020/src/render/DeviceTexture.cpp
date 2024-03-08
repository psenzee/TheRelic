#include "DeviceTexture.h"
#include "GraphicsDevice.h"
#include "render/GLUtils.h"
#include "platform/LoadTexture.h"
#include "platform/GLIncludes.h"
#include "render/GLStates.h"

DeviceTexture::DeviceTexture(const char *filename) : texid(0), size(0, 0)
{
    char newname[1024];
    snprintf(newname, sizeof(newname) - 1, "%s.png", filename);
    texid = LoadTexture(newname, &size);
}

DeviceTexture::~DeviceTexture()
{
    printf("Deleting texture %p\n", this);
    if (Loaded())
    {
        GLuint tid = texid;
        glDeleteTextures(1, &tid);
        texid = 0;
    }
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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, value ? GL_LINEAR_MIPMAP_NEAREST : GL_LINEAR);
}

void DeviceTexture::Set(GraphicsDevice &device, unsigned int blendsrc, unsigned int blenddst)
{
    if (!Loaded())
        return;
    if (texid != 0)
    {    
    GLStates::texture.Set(true);
        if (device.HasGlobalAlpha()) {
            blenddst = GL_ONE_MINUS_SRC_ALPHA;
        }
        // Set a blending function to use
        glBlendFunc(blendsrc, blenddst);
        // Enable blending
        GLStates::blend.Set(IsAlphaType(blendsrc) || IsAlphaType(blenddst));
        glBindTexture(GL_TEXTURE_2D, texid);
    }
}
