print "Lua:fx.lua"

function _FireHit(x, y)
  return CreateCloud("fire1", x, y, -100, 500.0, 3.0) 
end

function FireHit(x, y)
  local r = _FireHit(x, y)
  Rpc(_FireHit, x, y)  
  return r  
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

function _DoorLockHit(x, y)
  return CreateCloud("dust_glow_shadow_desat", x, y, -150, 500.0, 3.0)
end

function DoorLockHit(x, y)
  local r = _DoorLockHit(x, y)
  Rpc(_DoorLockHit, x, y)  
  return r  
end

function _DustHit(x, y)
  return CreateCloud("dust_glow_shadow_desat", x, y, -100, 500.0, 3.0)
end

function DustHit(x, y)
  local r = _DustHit(x, y)
  Rpc(_DustHit, x, y)
  return r  
end

function _PoisonHit(x, y)
  return CreateCloud("green_glow_shadow", x, y, -100, 500.0, 3.0)
end

function PoisonHit(x, y)
  local r = _PoisonHit(x, y)
  Rpc(_PoisonHit, x, y)
  return r  
end

function _DustCircle(x, y)
  return CreateCircleCloud("blue_glow_shadow", x, y, -145.0, 1000.0, 10.0) 
end

function DustCircle(x, y)
  local r = _DustCircle(x, y)
  Rpc(_DustCircle, x, y)  
  return r  
end

function _BlueGlow(x, y)
  return CreateCloud("blue_glow_shadow", x, y, -145.0, 500.0, 4.0)
end

function BlueGlow(x, y)
  local r = _BlueGlow(x, y)
  Rpc(_BlueGlow, x, y)
  return r  
end

function _GreenGlow(x, y)
  return CreateParticleSpray("green_glow_shadow", x, y, -145.0, 500.0, 4.0)
end

function GreenGlow(x, y)
  local r = _GreenGlow(x, y)
  Rpc(_GreenGlow, x, y)
  return r
end