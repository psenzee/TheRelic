#ifndef _DRIFTSTATE_H
#define _DRIFTSTATE_H

#include "core/core.h"

class DriftState
{
    enum { FRAMES = 10 };

    Vector3 mOrigin,
            mDirection;
    float   mOrientation;
    int     mFrames;
    float   mDistance;

public:

    DriftState() : mOrientation(0.f), mFrames(0), mDistance(0.f) {}

    DriftState(Character *character, Character *from, float distance)
    {
        Vector3 a = character->GetPosition(),
                b = from->GetPosition();
        mOrigin      = a;
        mDirection   = (b - a).normal();
        mFrames      = FRAMES;
        mOrientation = character->GetOrientationAngle();
        mDistance    = distance;
    }

    void Update(Character *character) // don't call if dead
    {
        if (character && mFrames > 0)
        {
            float back = mDistance / FRAMES * (FRAMES - mFrames);
            character->SetPosition(mOrigin - mDirection * back);
            character->SetOrientationAngle(mOrientation);
            mFrames -= 1;
        }
    }

private:

    float mDriftSpeed;
    float mRandomDrift;
    float mDriftRadius;

    void GoToCharacter(Character *self, Character *to)
    {
        if (!self || !self->IsVisible())
            return;

        if (to->IsDestroyed())
        {
            RandomDrift(self, mRandomDrift);
            return;
        }

        self->SetCollidable(false);
        Vector3 pos   = to->GetPosition(),
                toPos = to->GetPosition();
        Vector3 delta = toPos - pos;
        float   mag   = delta.length();

        if (mag > mDriftRadius)
        {
            RandomDrift(c, mRandomDrift);
            return;
        }
   
        if (mag <= playerRadius)
        {
    // this needs to be a callback
    Heal(GetPlayer(), 20.0)
            BlueGlow(xp, yp);
            self->SetVisible(false)
            return;
        }
  
        if (mag < 1.f)
        {
            RandomDrift(c, randomDrift);
            return;
        }
    
        float maxs = 1.f / mag * driftSpeed;    
        character->SetPosition(pos + delta * maxs);
    }

    void RandomDrift(Character *character, float degree)
    {
         Vector3 pos(character->GetPosition());
         character->SetPositionRaw(pos.x + math.random() * degree - math.random() * degree, pos.y + math.random() * degree - math.random() * degree, pos.z);
    }
};

/*

function GoToPlayer(c)

  if c == nil or not c:IsVisible() then
    return
  end

  local data = c:data()

  local driftSpeed = 5.0
  if data.driftSpeed ~= nil then
    driftSpeed = data.driftSpeed
  end
  local randomDrift = 3.0
  if data.randomDrift ~= nil then
    randomDrift = data.randomDrift
  end    
  local driftRadius = 1000.0
  if data.driftRadius ~= nil then
    driftRadius = data.driftRadius
  end  
  
  local player = GetPlayer()  

  if not IsAlive(player) then
    RandomDrift(c, randomDrift)
    return
  end

  c:SetCollidable(false)
  
  local  x,  y,  z = c:GetPosition()
  local xp, yp, zp = player:GetPosition()
  
  local dx, dy, dz = v3Sub(xp, yp, zp, x, y, z)
  local mag = v3Length(dx, dy, dz)
  
  if mag > driftRadius then
    RandomDrift(c, randomDrift)
    return  
  end
  
  if mag <= playerRadius then
    Heal(GetPlayer(), 20.0)
    BlueGlow(xp, yp)
    c:SetVisible(false)
    return
  end
  
  if mag < 1.0 then
    RandomDrift(c, randomDrift)
    return
  end  
    
  local maxs = 1 / mag * driftSpeed
  
  local xn = x + dx * maxs
  local yn = y + dy * maxs
  local zn = z + dz * maxs
    
  c:SetPosition(xn, yn, zn)  

end

function GoToPlayerAndExecute(c, fn, executeData)
  if c == nil or not c:IsVisible() then
    return
  end

  local data = c:data()

  local driftSpeed = 5.0
  if data.driftSpeed ~= nil then
    driftSpeed = data.driftSpeed
  end
  local randomDrift = 3.0
  if data.randomDrift ~= nil then
    randomDrift = data.randomDrift
  end    
  local driftRadius = 1000.0
  if data.driftRadius ~= nil then
    driftRadius = data.driftRadius
  end  
  
  local player = GetPlayer()  

  if not IsAlive(player) then
    RandomDrift(c, randomDrift)
    return
  end

  c:SetCollidable(false)
  
  local  x,  y,  z = c:GetPosition()
  local xp, yp, zp = player:GetPosition()
  
  local dx, dy, dz = v3Sub(xp, yp, zp, x, y, z)
  local mag = v3Length(dx, dy, dz)
  
  if mag > driftRadius then
    RandomDrift(c, randomDrift)
    return  
  end
  
  if mag <= playerRadius then
    fn(GetPlayer(), executeData)
    BlueGlow(xp, yp)
    c:SetVisible(false)
    return
  end
  
  if mag < 1.0 then
    RandomDrift(c, randomDrift)
    return
  end  
    
  local maxs = 1 / mag * driftSpeed
  
  local xn = x + dx * maxs
  local yn = y + dy * maxs
  local zn = z + dz * maxs
    
  c:SetPosition(xn, yn, zn)  

end
*/

#endif // _DRIFTSTATE_H