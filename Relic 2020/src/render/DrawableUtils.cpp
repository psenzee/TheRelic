#include "core/core.h"
#include "core/random.h"

#include "render/CreateGeometry.h"
#include "render/Drawable.h"
#include "render/ContentLoader.h"
#include "render/Texture.h"

struct ImmediateVertex;
 
Drawable *CreateRing(const char *renderTag,
                     const char *textureName,                      
                     int segments, float innerRadius, float outerRadius, float z)
{
    Texture *texture = new Texture(ContentLoader::GetInstance(),
                                   textureName, "SRC_ALPHA", "ONE_MINUS_SRC_ALPHA");
    int vertices = segments * 6;
    Vertex *data = new Vertex [vertices];
    CreateRing2d(innerRadius, outerRadius, segments, data);
    Matrix transform;
    transform.translate(Vector3(0.f, 0.f, z));
    RenderStates  states;
    states.CullFace = false;
    states.DepthWrite = false;
    states.TestVisible = false;
    Material material;
    Drawable *drawable = CreateImmediate(renderTag, (const ImmediateVertex *)data,
                                             vertices, texture, transform, states, material);
    delete [] data;
    return drawable;
}

Drawable *CreateRingTransform(const char *renderTag,
                     const char *textureName,                      
                     int segments, float innerRadius, float outerRadius, const Matrix &transform)
{
    Texture *texture = new Texture(ContentLoader::GetInstance(),
                                   textureName, "SRC_ALPHA", "ONE_MINUS_SRC_ALPHA");
    int vertices = segments * 6;
    Vertex *data = new Vertex [vertices];
    CreateRing2d(innerRadius, outerRadius, segments, data);
	Transform(data, vertices, transform);
    RenderStates  states;
    states.CullFace = false;
    states.DepthWrite = false;
    states.TestVisible = false;
    Material      material;
    Drawable     *drawable = CreateImmediate(renderTag, (const ImmediateVertex *)data,
                                             vertices, texture, Matrix(), states, material);
    delete [] data;
    return drawable;
}

Drawable *CreateCylinder(const char *renderTag,
                         const char *textureName,                      
                         int segments, float radius, float height, float heightUv)
{
    Texture *texture = new Texture(ContentLoader::GetInstance(),
                                   textureName, "SRC_ALPHA", "ONE_MINUS_SRC_ALPHA");
    int vertices = segments * 6;
    Vertex *data = new Vertex [vertices];
    CreateCylinder(radius, height, heightUv, segments, data);
    Matrix        identity;
    RenderStates  states;
    states.CullFace = false;
    states.DepthWrite = false;
    states.TestVisible = true;
    Material      material;
    Drawable     *drawable = CreateImmediate(renderTag, (const ImmediateVertex *)data,
                                             vertices, texture, identity, states, material);
    delete [] data;
    return drawable;
}

