/*
 *  BallCharacter.cpp
 *  GLGravity
 *
 *  Created by Paul Senzee on 12/30/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "BallCharacter.h"
#include "GameState.h"
#include "Actors.h"

#include "core/xna.h"
#include "core/random.h"

#include "objects/RollingBall.h"
#include "input/GameInput.h"
#include "map/Map.h"

const float BallCharacter::BUMP_MAGNITUDE = 0.25f; /*2.5f*/
const float BallCharacter::VELOCITY_SCALE = 20.0f; /*200.0f*/

BallCharacter::BallCharacter(GameState *state, XmlElement *level, const Vector2 &position) : state(state), map(0), forceFactor(1.0f), destroyed(false), jitter((intptr_t)this)
{
    ball = new RollingBall(level, "Player");
    Reset(position);
}

BallCharacter::~BallCharacter()
{
    delete ball;
    ball = 0;
}

const char *BallCharacter::GetType() const
{
    return type.c_str();    
}

void BallCharacter::SetForceFactor(float v)
{
    forceFactor = v;    
}

void BallCharacter::SetForce(const Vector2 &force)
{
    this->Force = force * forceFactor;
}

void BallCharacter::SetMap(Map *map)
{
    this->map = map;    
}

Vector3 BallCharacter::GetPosition() const
{
    return Vector3(Position.x, Position.y, 0.0f);
}
        
void BallCharacter::LoadContent(const char *type)
{
    ball->Load(state->services, state->renderables, type);
    this->type = type;
}

void BallCharacter::Reset(const Vector2 &position)
{
    Position  = position;
    Velocity  = Vector2(0.0f, 0.0f);
    Force     = Vector2(0.0f, 0.0f);
    destroyed = false;
}
        
void BallCharacter::Update(const GameTime &time, Actors &actors)
{
    if (!destroyed)
        ApplyPhysics(time, actors);
}

bool BallCharacter::Collides(const Vector3 &p, Actors &actors, bool *which)
{
    memset(which, 0, sizeof(bool) * actors.list.size());
    float RADIUS = 16.0f;
    int count = 0;
    bool any = false;
    for (std::vector<BallCharacter *>::iterator i = actors.list.begin(), e = actors.list.end(); i != e; ++i, ++count)
    {
        if (*i == this || (*i)->IsDestroyed())
            continue;
        Vector3 position((*i)->GetPosition());
        if (position.distance(p) < RADIUS * 2.0f)
            which[count] = (any |= true);
    }
    return any;
}

void BallCharacter::ApplyPhysics(const GameTime &time, Actors &actors)
{
    const float acc = 0.1f, terminal = 100.0f;
    Velocity += Force * (acc * state->GetBallVelocityScale()) * (time.elapsed * (1.0f / 1000.0f));
    if (!HandleCollisions(Velocity, actors))
    {
        Vector2 v(Velocity.x + Velocity.y * ball->conservation, -Velocity.y * ball->bounciness),
                u(-Velocity.x * ball->bounciness, Velocity.y + Velocity.x * ball->conservation); // transfer half the velocity from the other axis - conservation of momentum
        if      (HandleCollisions(v, actors))
            Velocity = v;
        else if (HandleCollisions(u, actors))
            Velocity = u;
        else
        {
            Velocity = Vector2(0, 0);
            return;
        }
    }        
    if (Velocity.length() > terminal) // clamp the velocity
    {
        Velocity.normalize();
        Velocity *= terminal;
    }

    Position += Velocity;
}
        
bool BallCharacter::CollidesAt(const Vector2 &p)
{
    Vector2 negz(0, (float)-BALL_SIZE / 2),
            posz((float)BALL_SIZE / 2, 0);
    int id0 = map->GetTileIdAt(p),        id1 = map->GetTileIdAt(p + negz),
        id2 = map->GetTileIdAt(p + posz), id3 = map->GetTileIdAt(p + negz + posz);
    return Map::IsCollidableTile(id0) || Map::IsCollidableTile(id1) || Map::IsCollidableTile(id2) || Map::IsCollidableTile(id3);
}
        
bool BallCharacter::HandleCollisions(const Vector2 &move, Actors &actors)
{
    if (CollidesAt(Position))    
    {

         // $TODO stay inside map's bounding box
         AABox bounds = map->GetBounds();
         bounds.minimum += Vector3(128.0f, 128.0f, 0.0f);
         bounds.maximum -= Vector3(128.0f, 128.0f, 0.0f);        
         // Vector3 pos = v3(Position);
         // pos = bounds.minimum.maximum(pos);
         // pos = bounds.maximum.minimum(pos);
         // Position = v2(pos);
         if (Position.x < bounds.minimum.x)
         Position.x = bounds.minimum.x;
         if (Position.y < bounds.minimum.y)
         Position.y = bounds.minimum.y;
         if (Position.x > bounds.maximum.x)
         Position.x = bounds.maximum.x;
         if (Position.y > bounds.maximum.y)
         Position.y = bounds.maximum.y;            
        
        return true; // if it's currently in a collidable spot, allow exit
    }
    Vector2 mp(move + Position);
//    mp += Vector2(jitter.real() * 0.01f, jitter.real() * 0.01f);
    bool here[64], there[64], any = false;
    Collides(GetPosition(), actors, here);
    Collides(v3(mp, 0.0f),  actors, there);
    bool collisions[64];
    for (size_t i = 0, sz = actors.list.size(); i < sz; i++)
    {
        collisions[i] = !here[i] && there[i];
        if (collisions[i])
        {
            actors.NotifyOfCollision(this, actors.list[i], v3(move));
            any = true;
        }
    }
    if (any)
        return false;
    return !CollidesAt(mp);
}
        
void BallCharacter::Draw(const GameTime &time)
{
    if (!destroyed)
        ball->Render(*(state->camera), v3(Position, 0.0f), Vector3::zero());    
}

void BallCharacter::DrawShadow()
{
    if (!destroyed)    
        ball->RenderShadow(*(state->camera));
}

void BallCharacter::DrawHalo()
{
    if (!destroyed)    
        ball->RenderHalo(*(state->camera));
}