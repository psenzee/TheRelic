#ifndef _CORETILEMAP_H
#define _CORETILEMAP_H

#include "core/core.h"
#include "fast/Allocator.h"
#include "ITileMap.h"

class CoreTileMap : public ITileMap
{
public:
    
    CLASS_NEW_DELETE()
        
    CoreTileMap(const core::Size &size);
    CoreTileMap(const core::Size &size, int initialValue);
    ~CoreTileMap();
    
    bool       Read(const char *filename);
    
    int        GetTileId(int x, int y)         const;
    bool       SetTileId(int x, int y, int id);
    core::Size GetSizeInTiles()                const;
    
    void       Reverse();
        
private:
        
    core::Size  size;
    int        *data;
        
    void Create(const core::Size &size, int value);
};

#endif // _CORETILEMAP_H