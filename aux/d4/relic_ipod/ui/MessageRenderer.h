/*
 *  MessageRenderer.h
 *  GLGravity
 *
 *  Created by Paul Senzee on 12/31/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _MESSAGERENDERER_H
#define _MESSAGERENDERER_H

class GameTime;
class Text;
class RenderContext;

#include <vector>
#include "core/core.h"

class IMessage
{
public:
        
    virtual ~IMessage() {}
        
    virtual void  Draw(RenderContext &context) = 0;
    virtual bool  Update(const GameTime &time) = 0;
    virtual float Depth() const                = 0;
};

class TextMessage : public IMessage
{
public:
        
    Text                 *text;
    int                   persists,
                          delay;
    Vector2               size;
    Vector3               offset;    
        
    TextMessage(Text *text, const Vector3 &offset, const Vector2 &size, int delay, int persists);
        
    void  Draw(RenderContext &context);        
    bool  Update(const GameTime &time);
    float Depth() const;
        
private:
        
    bool inited;
};

class MessageRenderer
{
public:
        
    MessageRenderer();
        
    void Add(IMessage *message);
    void Update(const GameTime &time);
    void Draw(RenderContext &context);
        
private:
        
    std::vector<IMessage *>  messages;    
};

#endif // _MESSAGERENDERER_H