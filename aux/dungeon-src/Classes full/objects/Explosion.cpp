/*
 *  Explosion.cpp
 *  GLGravity
 *
 *  Created by Paul Senzee on 12/30/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "Explosion.h"

#include "xml/XmlReadContext.h"
#include "render/OverheadCamera.h"
#include "render/Renderable.h"
#include "render/LocalGameServices.h"
#include "render/Material.h"
#include "objects/RollingBall.h" // $BAD_DEPENDENCY eliminate
            
Explosion::Explosion(const Vector3 &position) : services(0), timer(0.0f)
{
    translate.translate(position + RollingBall::BALL_OFFSET); // RollingBall $BAD_DEPENDENCY eliminate
}
            
void Explosion::Load(LocalGameServices *services, XmlReadContext *context, const char *type)
{
    this->services = services;
    char tag[128];
    int part = 0;
    Renderable *object = 0;
    do
    {
        sprintf(tag, "%s.%u", type, part++);
        object = context->objects[tag];
        if (object)
            renderables.push_back(Item(object));
        else
            printf("Unable to load %s\n", tag);
    } while (object);
}

void Explosion::Start(int time)
{
    int count = 1;
    size_t sz = renderables.size();
    for (std::vector<Item>::iterator i = renderables.begin(), e = renderables.end(); i != e; ++i, count++)
        (*i).time.Fade(1.0f, int(time / float(sz) * count));
    timer.Fade(1.0f, time);    
}

void Explosion::Update(const GameTime &time)
{
    float interpolant = timer.GetValue();
    if (interpolant <= 0.0f || interpolant >= 1.0f)
        return;    
    for (std::vector<Item>::iterator i = renderables.begin(), e = renderables.end(); i != e; ++i)
        (*i).object->Update(time);
}

void Explosion::Render(const OverheadCamera &camera)
{
    float interpolant = timer.GetValue();
    if (interpolant <= 0.0f || interpolant >= 1.0f)
        return;
    static Material material;
    material.ambient = Vector4(0.25f, 0.25f, 0.25f, 1.0f);
    Matrix scale;
    const float MAX_SCALE = 16.0f;
    for (std::vector<Item>::reverse_iterator i = renderables.rbegin(), e = renderables.rend(); i != e; ++i)
    {
        interpolant = (*i).time.GetValue();
        if (interpolant <= 0.0f || interpolant >= 1.0f)
            continue;
        scale.scale(interpolant * MAX_SCALE);
        Matrix m = scale * translate;    
        material.diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f - interpolant);
        material.Set(*(services->device));        
        (*i).object->Render(*(services->device), m, camera, true);
    }
}