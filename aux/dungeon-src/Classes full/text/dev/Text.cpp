/*
 *  Text.cpp
 *  GLGravity
 *
 *  Created by Paul Senzee on 12/31/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "Glyph.h"

#include "core/random.h"
#include "core/simplexml.h"
#include "core/global.h"

#include "xml/XmlUtil.h"
#include "xml/XmlObjectReader.h"

#include "render/OverheadCamera.h"
#include "render/DeviceTexture.h"
#include "render/LocalGameServices.h"
#include "render/Material.h"

Text::Text(GlyphWriter *writer, const char *s, const Vector2 &pos, const Vector2 &scale, const Vector3 &defaultColor, IAnimator *animator)
    : writer(writer), animator(animator)
{    
    Set(s, Vector3(pos.x + 0.5f * scale.x, pos.y, 0.0f), scale, defaultColor);
}

Text::~Text()
{
    delete list;
    list = 0;
}

void Text::MakePartyText(const char *s, char *buffer, int seed)
{
    if (!s || !*s)
        return;    
    const char *colors[] = { "\\#ff0000", "\\#00ff00", "\\#0000ff", "\\#ffff00" };
    size_t count = sizeof(colors) / sizeof(colors[0]);
    static core::Random r(0);
    if (seed != 0)
        r = core::Random(seed);
    char *pb = buffer;
    int prev = -1;
    for (; *s; s++)
    {
        int index = r.integer() % count;
        if (index == prev)
            index = (index + 1) % count; // make sure we don't have the same color twice adjacent
        pb += sprintf(pb, "%s%c", colors[index], *s);
        prev = index;
    }
}

void Text::Update(const GameTime &time)
{
    if (animator)
        animator->Update(time);
}
    
void Text::ForceUpdate(const GameTime &time)
{
    if (animator)
        animator->ForceUpdate(time);
}
    
void Text::Render(const Matrix &transform, const OverheadCamera &camera, const Vector4 &color)
{
    Matrix animated; animated.identity();
    Vector4 clr(color);
    if (animator)
    {
        animated  = animator->GetMatrix();
        clr      *= animator->GetColor();
    }
    Matrix nt(animated * transform);
    list->Render(nt, camera, clr);
}

void Text::Reset()
{
    if (animator)
        animator->Reset();
}

void Text::Set(const char *s, const Vector3 &pos, const Vector2 &scale, const Vector3 &color)
{
    if (s && *s)
        list = writer->Write(s, v3(initial), v3(pos), scale, Vector4(color.x, color.y, color.z, 1.0f)));
}