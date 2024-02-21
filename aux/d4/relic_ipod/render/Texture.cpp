#include "Texture.h"
#include "ContentLoader.h"
#include "platform/GLUtils.h"
#include "platform/GLIncludes.h"
#include "DeviceTexture.h"

/*
 
 sfactor  Specifies  how  the  red,  green, blue, and alpha source blending factors are computed.  The
 following   symbolic   constants    are    accepted:    GL_ZERO,    GL_ONE,    GL_DST_COLOR,
 GL_ONE_MINUS_DST_COLOR,       GL_SRC_ALPHA,       GL_ONE_MINUS_SRC_ALPHA,      GL_DST_ALPHA,
 GL_ONE_MINUS_DST_ALPHA, and GL_SRC_ALPHA_SATURATE.  The initial value is GL_ONE.
 
 Additionally, if the GL_ARB_imaging extension is  supported,  the  following  constants  are
 accepted:       GL_CONSTANT_COLOR,      GL_ONE_MINUS_CONSTANT_COLOR,      GL_CONSTANT_ALPHA,
 GL_ONE_MINUS_CONSTANT_ALPHA.
 
 dfactor  Specifies how the red, green, blue, and alpha destination  blending  factors  are  computed.
 Eight     symbolic     constants    are    accepted:    GL_ZERO,    GL_ONE,    GL_SRC_COLOR,
 GL_ONE_MINUS_SRC_COLOR,    GL_SRC_ALPHA,    GL_ONE_MINUS_SRC_ALPHA,    GL_DST_ALPHA,     and
 GL_ONE_MINUS_DST_ALPHA.  The initial value is GL_ZERO.
 
 Additionally,  if  the  GL_ARB_imaging  extension  is supported, the following constants are
 accepted:      GL_CONSTANT_COLOR,      GL_ONE_MINUS_CONSTANT_COLOR,       GL_CONSTANT_ALPHA,
 GL_ONE_MINUS_CONSTANT_ALPHA.
 
 */

static unsigned int GetBlendFactorFromName(const char *s)
{
    if (!s)    return ~0u;
    
    struct Pair { const char *name; unsigned int value; };
    Pair pairs[] = { { "ZERO",                     GL_ZERO },
                     { "ONE",                      GL_ONE }, 
                     { "SRC_COLOR",                GL_SRC_COLOR },
                     { "ONE_MINUS_SRC_COLOR",      GL_ONE_MINUS_SRC_COLOR },
                     { "DST_COLOR",                GL_DST_COLOR },
                     { "ONE_MINUS_DST_COLOR",      GL_ONE_MINUS_DST_COLOR },
                     { "SRC_ALPHA",                GL_SRC_ALPHA },
                     { "ONE_MINUS_SRC_ALPHA",      GL_ONE_MINUS_SRC_ALPHA },
                     { "DST_ALPHA",                GL_DST_ALPHA },
                     { "ONE_MINUS_DST_ALPHA",      GL_ONE_MINUS_DST_ALPHA },
                     { "SRC_ALPHA_SATURATE",       GL_SRC_ALPHA_SATURATE },
#ifdef GL_CONSTANT_COLOR        
                     { "CONSTANT_COLOR",           GL_CONSTANT_COLOR },
                     { "ONE_MINUS_CONSTANT_COLOR", GL_ONE_MINUS_CONSTANT_COLOR },
                     { "CONSTANT_ALPHA",           GL_CONSTANT_ALPHA },
                     { "ONE_MINUS_CONSTANT_ALPHA", GL_ONE_MINUS_CONSTANT_ALPHA },
#endif
                     { 0, ~0u } };

    for (Pair *p = pairs; p->name; p++)
        if (strcmp(p->name, s) == 0)
            return p->value;
    return ~0u;
}

Texture::Texture(ContentLoader *content, const char *filename, const char *blendsrc, const char *blenddst)
    : filename(filename), texture(content->GetTexture(filename)), mRef(1)
{
    this->blendsrc = GetBlendFactorFromName(blendsrc);
    this->blenddst = GetBlendFactorFromName(blenddst);    
    if (this->blendsrc == ~0u) this->blendsrc = GL_ONE;
    if (this->blenddst == ~0u) this->blenddst = GL_ONE_MINUS_SRC_ALPHA;    
}

Texture::Texture(ContentLoader *content, const char *filename, unsigned int blendsrc, unsigned int blenddst) 
    : filename(filename), texture(content->GetTexture(filename)), blendsrc(blendsrc), blenddst(blenddst), mRef(1)
{
}

Texture::~Texture()
{
    // todo
}

void Texture::Set(GraphicsDevice &device)
{
    if (texture)
        texture->Set(device, blendsrc, blenddst);
}

void Texture::Retain()
{
    mRef++;        
}

void Texture::Release()
{
    if (--mRef == 0)
        delete this;
}
