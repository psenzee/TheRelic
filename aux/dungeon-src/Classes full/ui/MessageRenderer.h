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
class OverheadCamera;
class LocalGameServices;

#include <vector>
#include "core/xna.h"

class IMessage
{
public:
        
    virtual ~IMessage() {}
        
    virtual void  Draw()                       = 0;
    virtual bool  Update(const GameTime &time) = 0;
    virtual float Depth() const                = 0;
};

class TextMessage : public IMessage
{
public:
        
    Text                 *text;
    int                   persists,
                          delay;
    const OverheadCamera &camera;
    Vector2               size;
    Vector3               offset;    
        
    TextMessage(Text *text, const OverheadCamera &camera, const Vector3 &offset, const Vector2 &size, int delay, int persists);
        
    void  Draw();        
    bool  Update(const GameTime &time);
    float Depth() const;
        
private:
        
    bool inited;
};

class MessageRenderer
{
public:
        
    MessageRenderer(LocalGameServices *services);
        
    void Add(IMessage *message);
    void Update(const GameTime &time);
    void Draw();
        
private:
        
    LocalGameServices       *services;
    std::vector<IMessage *>  messages;    
};

#endif // _MESSAGERENDERER_H