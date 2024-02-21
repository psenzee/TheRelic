/*
 *  Explosions.cpp
 *  GLGravity
 *
 *  Created by Paul Senzee on 12/30/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "Explosions.h"
#include "Explosion.h"

#include <algorithm>

Explosions::Explosions(LocalGameServices *services, XmlReadContext *context, int duration) : services(services), context(context), duration(duration)
{
}

Explosions::~Explosions()
{
    Clear();    
}

void Explosions::Explode(const Vector3 &at)
{
    Explosion *e = new Explosion(at);
    e->Load(services, context, "Explosion");
    explosions.push_back(e);
    e->Start(duration);
}

void Explosions::Update(const GameTime &time)
{
    for (std::vector<Explosion *>::iterator i = explosions.begin(), e = explosions.end(); i != e; ++i)
        (*i)->Update(time);
    ClearCompleted();
}

void Explosions::Render(const OverheadCamera &camera)
{
    for (std::vector<Explosion *>::iterator i = explosions.begin(), e = explosions.end(); i != e; ++i)
        (*i)->Render(camera);
}

void Explosions::ClearCompleted()
{
    bool cleared = false;
    for (std::vector<Explosion *>::iterator i = explosions.begin(), e = explosions.end(); i != e; ++i)
    {
        if ((*i)->IsDone()) { delete *i; *i = 0; cleared = true; }
    }
    if (cleared)
        explosions.erase(std::remove(explosions.begin(), explosions.end(), (Explosion *)0), explosions.end());    
}

void Explosions::Clear()
{
    for (std::vector<Explosion *>::iterator i = explosions.begin(), e = explosions.end(); i != e; ++i)
        delete *i;
    explosions.clear();
}