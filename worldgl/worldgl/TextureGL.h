#pragma once

#include <array>

class TextureGL
{
public:

    typedef std::array<size_t, 2> size2_t;
    
    TextureGL(const char *filename);
    ~TextureGL();
    
    inline const size2_t &Size() const { return _size; }
    
    bool Loaded() const;
    void SetMipMapped(bool value);
    void SetBlend(unsigned source, unsigned destination);
    
    void SetSampler(unsigned programId, unsigned sampler);

    unsigned GetId() const { return _textureId; }
    
private:
    
    volatile unsigned _textureId;
    size2_t           _size;
};
