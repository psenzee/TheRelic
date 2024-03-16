#include "TextureGL.h"
#include "gl_utils.h"
#include "read_pvr.h"
#include "read_png.h"
#include <iostream>

TextureGL::TextureGL(const char *filename) : _textureId(0), _size { 0, 0 }
{
    //_textureId = LoadPVRTexture(filename, &_size);
    _textureId = LoadPNGTexture(filename, &_size);
}

TextureGL::~TextureGL()
{
    printf("Deleting texture %p\n", this);
    if (Loaded()) {
        GLuint tid = _textureId;
        _GL(glDeleteTextures(1, &tid));
        _textureId = 0;
    }
}

bool TextureGL::Loaded() const
{
    return _textureId && glIsTexture(_textureId);
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

void TextureGL::SetMipMapped(bool value)
{
    _GL(glBindTexture(GL_TEXTURE_2D, _textureId));
    _GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, value ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR));
    _GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, value ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR));
}

void TextureGL::SetSampler(unsigned programId, unsigned sampler)
{
    _GL(glActiveTexture(GL_TEXTURE0 + sampler))
    _GL(glBindTexture(GL_TEXTURE_2D, _textureId))
    auto samplerLocationId = _GL(glGetUniformLocation(programId, "u_sampler"));
    //std::cerr << "sampler location id " << samplerLocationId << " texture id " << _textureId << std::endl;
    _GL(glUniform1i(samplerLocationId, sampler));
}

void TextureGL::SetBlend(unsigned source, unsigned destination)
{
    if (!Loaded())
        return;
    if (_textureId != 0) {
        _GL(glEnable(GL_TEXTURE_2D));
        //if (device.HasGlobalAlpha()) {
        //    destination = GL_ONE_MINUS_SRC_ALPHA;
        //}
        // Set a blending function to use
        _GL(glBlendFunc(source, destination));
        // Enable blending
        if (IsAlphaType(source) || IsAlphaType(destination)) {
            _GL(glEnable(GL_BLEND));
        } else {
            _GL(glDisable(GL_BLEND));
        }
        _GL(glBindTexture(GL_TEXTURE_2D, _textureId));
    }
}
