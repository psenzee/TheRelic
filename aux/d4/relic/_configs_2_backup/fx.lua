print "Lua:fx.lua"

function FireHit(x, y)
  return CreateCloud("fire1", x, y, -100, 500.0, 3.0)
end

function PerpetualFire(x, y)
  return CreatePerpetualCloud("fire1", x, y, -100, 500.0, 3.0)
end

function DestroyPerpetualEffect(data)
  if data.cloud ~= nil then
    Effect_Destroy(data.cloud)
    data.cloud = nil
  end
end

function DoorLockHit(x, y)
  return CreateCloud("dust_glow_shadow_desat", x, y, -150, 500.0, 3.0)
end

function DustHit(x, y)
  return CreateCloud("dust_glow_shadow_desat", x, y, -100, 500.0, 3.0)
end

function PoisonHit(x, y)
  return CreateCloud("green_glow_shadow", x, y, -100, 500.0, 3.0)
end

function DustCircle(x, y)
 -- return CreateCircleCloud("dust2_glow_shadow", x, y, -145.0, 1000.0, 10.0)
    return CreateCircleCloud("blue_glow_shadow", x, y, -145.0, 1000.0, 10.0) 
 -- return CreateCircleCloud("fire1", x, y, -145.0, 1000.0, 10.0) 
end

function CollisionTest(x, y)
--  return CreateCollisionTest("fire1", x, y, -100, 1000.0, 10.0)
  -- turn off collision test for now
  --return nil
end

function BlueGlow(x, y)
  return CreateCloud("blue_glow_shadow", x, y, -145.0, 500.0, 4.0)
end

function GreenGlow(x, y)
  return CreateParticleSpray("green_glow_shadow", x, y, -145.0, 500.0, 4.0)
end