/*
 *  Text.h
 *  GLGravity
 *
 *  Created by Paul Senzee on 12/31/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _TEXT_H
#define _TEXT_H

#include "core/xna.h"
#include "Glyph.h"
#include "Animator.h"

class OverheadCamera;

class Text
{
public:    
    
    static void MakePartyText(const char *s, char *buffer, int seed = 0);
        
    Text(GlyphWriter *writer, const char *s, const Vector2 &pos, const Vector2 &scale, const Vector3 &defaultColor, IAnimator *animator);
    ~Text();
        
    void Update(const GameTime &time);
    void ForceUpdate(const GameTime &time);
    
    void Render(const Matrix &transform, const OverheadCamera &camera, const Vector4 &color);        
    void Reset();
        
private:
        
    GlyphDrawList *list;
    GlyphWriter   *writer;
    IAnimator     *animator;

    void Set(const char *s, const Vector3 &pos, const Vector2 &scale, const Vector3 &color);
};

#endif // _TEXT_H