#include "core/core.h"
#include "core/random.h"

#include "render/CreateGeometry.h"
#include "render/Drawable.h"
#include "render/ContentLoader.h"
#include "render/Texture.h"

struct ImmediateVertex;

Drawable *CreateRing(const char *renderTag,
                     const char *textureName,                      
                     int segments, float innerRadius, float outerRadius)
{
    Texture *texture = new Texture(ContentLoader::GetInstance(),
                                   textureName, "SRC_ALPHA", "ONE_MINUS_SRC_ALPHA");
    int vertices = segments * 6;
    Vertex *data = new Vertex [vertices];
    CreateRing2d(innerRadius, outerRadius, segments, data);
    Matrix        identity;
    RenderStates  states;
    states.CullFace = false;
    states.DepthWrite = false;
    Material      material;
    Drawable     *drawable = CreateImmediate(renderTag, (const ImmediateVertex *)data,
                                             vertices, texture, identity, states, material);
    delete [] data;
    return drawable;
}
