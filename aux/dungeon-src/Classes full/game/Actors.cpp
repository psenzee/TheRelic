/*
 *  Actors.cpp
 *  GLGravity
 *
 *  Created by Paul Senzee on 12/30/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "Actors.h"
#include "objects/RollingBall.h"

#include <algorithm>

Actors::Actors() : player(0), previousplayer(0), transition(0.0f)
{
}

int Actors::GetIndex(const BallCharacter *p) const
{
    for (size_t i = 0, sz = list.size(); i < sz; i++)
        if (list[i] == p)
            return (int)i;
    return -1;
}

void Actors::AddCollisionNotifiable(notify_collision_fn_t notifiable, void *context)
{
    Notifiable n;
    n.function = notifiable; n.context = context;
    notifiables.push_back(n);
}

void Actors::RemoveCollisionNotifiable(notify_collision_fn_t notifiable, void *context)
{
    Notifiable n;
    n.function = notifiable; n.context = context;
    notifiables.erase(std::remove(notifiables.begin(), notifiables.end(), n), notifiables.end());
}

bool Actors::NotifyOfCollision(BallCharacter *a, BallCharacter *b, const Vector3 &velocity)
{
    bool handled = false;
    for (std::vector<Notifiable>::iterator i = notifiables.begin(), e = notifiables.end(); i != e; ++i)
    {
        if ((*i).function && ((*i).function)(a, b, velocity, (*i).context))
        {
            handled = true;
            break;
        }
    }
    return handled;
}

void Actors::Add(BallCharacter *p)
{
    list.push_back(p);
}

void Actors::SetForce(const Vector2 &force)
{
    for (std::vector<BallCharacter *>::iterator i = list.begin(), e = list.end(); i != e; ++i)
    {
        Vector2 total(force);
        if (strcmp((*i)->GetType(), "PositiveBall") == 0)
        {
            total += GetRepelForce(*i, "PositiveBall");
        //    total -= GetRepelForce(*i, "NegativeBall");
        }
        else if (strcmp((*i)->GetType(), "NegativeBall") == 0)
        {
            total += GetRepelForce(*i, "NegativeBall");
        //    total -= GetRepelForce(*i, "PositiveBall");
        }
        (*i)->SetForce(total);        
    }
}

Vector2 Actors::GetRepelForce(BallCharacter *ball, const char *type)
{
    //const float REPEL = 2500.0f;
    const float REPEL = 2000.0f;
    Vector2 force(0.0f, 0.0f), pos(v2(ball->GetPosition()));
    for (std::vector<BallCharacter *>::iterator i = list.begin(), e = list.end(); i != e; ++i)
    {
        if (ball != *i && !(*i)->IsDestroyed() && strcmp((*i)->GetType(), type) == 0)
        {
            Vector2 diff(pos - v2((*i)->GetPosition()));
            float   magnitude = diff.dot(diff);
            if (magnitude > 0.001f)
                force += diff.normalize() * math::clamp(((1.0f / magnitude) * REPEL), 0.0f, 100.0f);
        }
    }
    return force;
}

void Actors::Clear()
{
    for (std::vector<BallCharacter *>::iterator i = list.begin(), e = list.end(); i != e; ++i)
        delete *i;
    list.clear();
    player = previousplayer = 0;
}

void Actors::Update(const GameTime &time)
{
    enum { COUNT = 4 };
    GameTime div(time.elapsed / COUNT);
    for (int i = 0; i < COUNT; i++)
    {
        for (std::vector<BallCharacter *>::iterator i = list.begin(), e = list.end(); i != e; ++i)
            (*i)->Update(div, *this);
    }
}

void Actors::Draw(const GameTime &time)
{
    for (std::vector<BallCharacter *>::iterator i = list.begin(), e = list.end(); i != e; ++i)
        (*i)->Draw(time);
}

void Actors::SetPlayerIndex(int i)
{
    if (player == i)
        return;
    previousplayer = player;
    player = i;
    transition.Fade(1.0f, 1000);
    GetPlayer()->GetRollingBall()->halofader.Fade(1.0f, 500);
    if (previousplayer != player)
        list[previousplayer]->GetRollingBall()->halofader.Fade(0.0f, 1);
}

Vector3 Actors::GetLookAt()
{
    if (previousplayer != player)
    {
        BallCharacter *from = list[previousplayer],
                      *to   = list[player];
        float v = transition.GetValue();
        if (v < 1.0f)
            return (to->GetPosition() * v) + (from->GetPosition() * (1.0f - v));
        transition.Fade(0.0f, 1);
        previousplayer = player;
    }
    return GetPlayer()->GetPosition();
}

void Actors::DrawShadow() // assumes that Draw was called earlier in the frame
{
    for (std::vector<BallCharacter *>::iterator i = list.begin(), e = list.end(); i != e; ++i)
        (*i)->DrawShadow();
    GetPlayer()->DrawHalo();
}
