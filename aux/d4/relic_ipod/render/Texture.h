#ifndef _TEXTURE_H
#define _TEXTURE_H

#include <string>

class ContentLoader;
class DeviceTexture;
class GraphicsDevice;

class Texture
{
public:

    std::string    filename;
    DeviceTexture *texture;
    unsigned int   blendsrc, blenddst;
    
    Texture(ContentLoader *content, const char *filename, const char *blendsrc, const char *blenddst);
    Texture(ContentLoader *content, const char *filename, unsigned int blendsrc, unsigned int blenddst);
    ~Texture();
    
    void Set(GraphicsDevice &device);

    void Retain();
    void Release();

private:

    int mRef;
};
    
#endif // _TEXTURE_H