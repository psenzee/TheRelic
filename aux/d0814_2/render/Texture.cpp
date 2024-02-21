#include "Texture.h"
#include "ContentLoader.h"
#include "render/GLUtils.h"
#include "render/GLStates.h"
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

static unsigned int GetCombineOpFromName(const char *s)
{
    if (!s)    return ~0u;
    
    struct Pair { const char *name; unsigned int value; };
    Pair pairs[] = { { "INTERPOLATE",              GL_INTERPOLATE },
                     { "MODULATE",                 GL_MODULATE }, 
                     { "ADD",                      GL_ADD },

                     { 0, ~0u } };

    for (Pair *p = pairs; p->name; p++)
        if (strcmp(p->name, s) == 0)
            return p->value;
    return ~0u;
}

Texture::Texture(ContentLoader *content, const char *filename, const char *blendSrc, const char *blendDst)
//    : filename(filename), texture(content->GetTexture(filename)), mRef(1)
: mRef(1)
{
    Initialize(content, filename, 0, 0, 0, blendSrc, blendDst);
    /*
    this->blendSrc = GetBlendFactorFromName(blendSrc);
    this->blendDst = GetBlendFactorFromName(blendDst);
    if (this->blendSrc == ~0u)
    {
        if (blendSrc)
            printf("Unrecognized blend-source name '%s'!\n", blendSrc);        
        this->blendSrc = GL_ONE;
    }
    if (this->blendDst == ~0u)
    {
        if (blendDst)
            printf("Unrecognized blend-destination name '%s'!\n", blendDst);
        this->blendDst = GL_ZERO;//GL_ONE_MINUS_SRC_ALPHA;    
    }
    */
}

Texture::Texture(ContentLoader *content, const char *filename, unsigned blendSrc, unsigned blendDst)
//    : filename(filename), texture(content->GetTexture(filename)), combineTexture(0),
//      colorOp(0), alphaOp(0), blendSrc(blendSrc), blendDst(blendDst), mRef(1)
: mRef(1)
{
    Initialize(content, filename, 0, ~0u, ~0u, blendSrc, blendDst);
}

Texture::Texture(ContentLoader *content, const char *filename, const char *combine,
                 unsigned colorOp, unsigned alphaOp, unsigned blendSrc, unsigned blendDst) : mRef(1)
{
    Initialize(content, filename, combine, colorOp, alphaOp, blendSrc, blendDst);
}

Texture::Texture(ContentLoader *content, const char *filename, const char *combine,
                 const char *colorOp, const char *alphaOp, const char *blendSrc, const char *blendDst) : mRef(1)
{
    Initialize(content, filename, combine, colorOp, alphaOp, blendSrc, blendDst);
}

void Texture::Initialize(ContentLoader *content, const char *filename, const char *combine,
                         unsigned colorOp, unsigned alphaOp, unsigned blendSrc, unsigned blendDst)
{
    this->filename        = filename;
    this->combineFilename = combine ? combine : "";
    this->texture         = content->GetTexture(filename);
    this->combineTexture  = combine ? content->GetTexture(combine) : 0;
    this->colorOp         = colorOp;
    this->alphaOp         = alphaOp;
    this->blendSrc        = blendSrc;
    this->blendDst        = blendDst;
}

void Texture::Initialize(ContentLoader *content, const char *filename, const char *combine,
                         const char *colorOp, const char *alphaOp, const char *blendSrc, const char *blendDst)
{
    unsigned blendSrcValue = GetBlendFactorFromName(blendSrc),
             blendDstValue = GetBlendFactorFromName(blendDst),
             colorOpValue  = GetCombineOpFromName(colorOp),
             alphaOpValue  = GetCombineOpFromName(alphaOp);
    if (blendSrcValue == ~0u)
    {
        if (blendSrc)
            printf("Unrecognized blend-source name '%s'!\n", blendSrc);        
        blendSrcValue = GL_ONE;
    }
    if (blendDstValue == ~0u)
    {
        if (blendDst)
            printf("Unrecognized blend-destination name '%s'!\n", blendDst);
        blendDstValue = GL_ZERO;
    }
    if (colorOpValue == ~0u) colorOpValue = GL_MODULATE;
    if (alphaOpValue == ~0u) alphaOpValue = GL_MODULATE;
    Initialize(content, filename, combine, colorOpValue, alphaOpValue, blendSrcValue, blendDstValue);
}

Texture::~Texture()
{
    printf("Deleting texture %s\n", filename.c_str());
    // todo
    if (texture)
        delete texture;
    texture = 0;
}

void Texture::Set(GraphicsDevice &device)
{
    static bool textureStage1Cleared = false;
    if (texture)
    {
        if (!combineTexture)
        {
//            SetActiveTexture(GL_TEXTURE0);
//            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
            texture->Set(device, blendSrc, blendDst);
//SetActiveTexture(GL_TEXTURE1);
//            glDisable(GL_TEXTURE_2D);
//            SetActiveTexture(GL_TEXTURE0);
            // clear out second texture stage only if necessary
            //if (!textureStage1Cleared)
            {
                //SetAsTexture1(0);
                textureStage1Cleared = true;
            }
        }
        else
        {
            textureStage1Cleared = false;
            //GLStates::texture.Set(true);
            // Set a blending function to use
            glBlendFunc(blendSrc, blendDst);

            SetActiveTexture(GL_TEXTURE0);
            glEnable(GL_TEXTURE_2D);
            //texture->Set(device, blendSrc, blendDst);
            glBindTexture(GL_TEXTURE_2D, texture->GetId());
            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

            SetActiveTexture(GL_TEXTURE1);
            glEnable(GL_TEXTURE_2D);
            //combineTexture->Set(device, blendSrc, blendDst);
            glBindTexture(GL_TEXTURE_2D, combineTexture->GetId());
            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);

            SetColorCombineMode(colorOp);
            SetAlphaCombineMode(alphaOp);

            SetActiveTexture(GL_TEXTURE0);
        }
    }
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
