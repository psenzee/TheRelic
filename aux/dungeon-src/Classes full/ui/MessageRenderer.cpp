/*
 *  MessageRenderer.cpp
 *  GLGravity
 *
 *  Created by Paul Senzee on 12/31/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "MessageRenderer.h"
#include "core/xna.h"

#include "text/Glyph.h"
#include "text/Text.h"
#include "render/OverheadCamera.h"

#include <algorithm>

TextMessage::TextMessage(Text *text, const OverheadCamera &camera, const Vector3 &offset, const Vector2 &size, int delay, int persists) 
    : text(text), persists(persists), delay(delay), camera(camera), size(size), offset(offset), inited(false)
{
}
    
void TextMessage::Draw()
{
    if (inited)
    {
        Vector3 lookAt(camera.GetLookAt());
        lookAt.z  = 128.0f;
        lookAt.y += 16.0f;
        if (delay <= 0)
        {
            Matrix s, m;
            s.scale(v3(size, 1.0f));
            m.translate(lookAt + offset * v3(size, 1.0f));
            m = s * m;
            text->Render(m, camera, Vector4(1.0f, 1.0f, 1.0f, 1.0f));
        }
    }
}

float TextMessage::Depth() const
{
    return text->GetRenderAnchorPosition().z;
}
        
bool TextMessage::Update(const GameTime &time)
{
    if (!inited)
    {
         text->Reset();
         text->ForceUpdate(time);
         inited = true;
    }
    delay--;
    if (delay <= 0)
    {
        text->Update(time);
        persists--;
    }
    if (persists == 0)
        return false; // remove
    return true;
}

MessageRenderer::MessageRenderer(LocalGameServices *services) : services(services)
{
}
    
void MessageRenderer::Add(IMessage *message)
{
    messages.push_back(message);        
}

void MessageRenderer::Update(const GameTime &time)
{
    std::vector<IMessage *> list;
    for (std::vector<IMessage *>::iterator i = messages.begin(), e = messages.end(); i != e; ++i)
    {
        if (!(*i)->Update(time))
            delete *i;
        else list.push_back(*i);
    }
    messages = list;
}    

void MessageRenderer::Draw()
{
    for (std::vector<IMessage *>::iterator i = messages.begin(), e = messages.end(); i != e; ++i)
        (*i)->Draw();
}