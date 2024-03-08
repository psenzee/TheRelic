#pragma once

#include "core/core.h"
#include <string>
#include <utility>
#include <cassert>
#include "GLIncludes.h"

struct GLBuffer
{
    size_t      elementCount;
    size_t      componentSize;
    size_t      elementStride;
    unsigned    bufferId;
    GLenum      target;

    GLBuffer(GLenum target, const void *data, size_t elementStride, size_t elementCount, size_t componentSize)
        : elementCount(elementCount), componentSize(componentSize), elementStride(elementStride)
    {
        glGenBuffers(1, &bufferId);
        if (bufferId > 0) {
            glBindBuffer(target, bufferId); // bind
            glBufferData(target, elementCount * elementStride, data, GL_STATIC_DRAW); // copy data
            glBindBuffer(target, 0); // unbind
        }
    }
    
    void Bind()
    {
        glBindBuffer(target, bufferId);         // unbind
    }

    void Unbind()
    {
        glBindBuffer(target, 0);         // unbind
    }

    ~GLBuffer()
    {
        glDeleteBuffers(1, &bufferId);
    }
/*
    void BindToDrawAsPositionWithIndices(unsigned uPositionId)
    {
        glBindBuffer(GL_ARRAY_BUFFER, bufferId);
        assert(componentSize == sizeof(float));
        size_t componentCount = elementStride / componentSize;
        glVertexAttribPointer(uPositionId, componentCount, GL_FLOAT, false, 0, 0); // <----- 0, because deviceId is bound
        glEnableVertexAttribArray(uPositionId);
    }

    void DrawIndices()
    {
        glBindBuffer(GL_ARRAY_BUFFER, bufferId); // I think this should have been GL_ELEMENT_ARRAY_BUFFER anyway
        assert(componentSize == sizeof(unsigned short));
        glDrawElements(GL_TRIANGLES, elementCount, GL_UNSIGNED_SHORT, 0);     // <----- 0, because deviceId is bound
    }

    void UnbindPosition(unsigned uPositionId)
    {
        glDisableVertexAttribArray(uPositionId);
        glBindBuffer(target, 0);         // unbind
    }

    void UnbindIndices()
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // unbind
    }
*/
};
