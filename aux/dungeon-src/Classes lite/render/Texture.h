#ifndef _TEXTURE_H
#define _TEXTURE_H

#include <string>

class ContentCache;
class DeviceTexture;
class GraphicsDevice;

class Texture
{
public:
        
    std::string    filename;
    DeviceTexture *texture;
    unsigned int   blendsrc, blenddst;
    
    Texture(ContentCache *content, const char *filename, const char *blendsrc, const char *blenddst);
    Texture(ContentCache *content, const char *filename, unsigned int blendsrc, unsigned int blenddst);
    
    void Set(GraphicsDevice &device);
};
    
#endif // _TEXTURE_H