#ifndef _OVERHEADCAMERA_H
#define _OVERHEADCAMERA_H

#include "core/core.h"
#include "math/MathUtil.h"
#include "fast/Allocator.h"

class GraphicsDevice;

class OverheadCamera
{
public:
    
    CLASS_NEW_DELETE()
        
    OverheadCamera(GraphicsDevice &device, float fovDegrees);
        
    inline const Vector3   &GetLookAt()                    const { return lookAt; }
    inline void             SetLookAt(const Vector3 &v)          { lookAt = v; Update(); }
    inline float            GetFovDegrees()                const { return float(fov * RAD_TO_DEG); }
    inline void             SetFovDegrees(float v)               { fov = float(v * DEG_TO_RAD); Update(); }
    inline float            GetFovRadians()                const { return fov; }    
    inline void             SetFovRadians(float v)               { fov = v; Update(); }
    inline float            GetDepthScale()                const { return depthScale; }
    inline void             SetDepthScale(float v)               { depthScale = v; Update(); }
    inline const Vector2   &GetTilt()                      const { return tilt; }
    inline void             SetTilt(const Vector2 &v)            { tilt = v; Update(); }
    inline const Vector2   &GetTiltScale()                 const { return tiltScale; }
    inline void             SetTiltScale(const Vector2 &v)       { tiltScale = v; Update(); }

    inline Vector3          GetViewVector()                const { return (lookAt - position).normal(); }

    inline const Vector3   &GetPosition()                  const { return position; }
    inline const Matrix    &GetView()                      const { return view; }        
    inline const Matrix    &GetProjection()                const { return projection; }
    inline const Matrix    &GetViewProjection()            const { return viewProjection; }
    inline const Matrix    &GetInverseViewProjection()     const { return inverse; }        
    
    Ray                     GetRayFromNormalizedScreen(const Vector2 &at) const;
    bool                    PickPlanePoint(const Vector4 &plane, const Vector2 &from, Vector3 &at) const;
    
    Vector3                 GetCameraPosition(const Vector3 &lookAt, float depthScale);
    Vector3                 GetCameraPositionTilt(const Vector3 &lookAt, const Vector2 &tilt, const Vector2 &tiltScale, float depthScale);
        
private:
        
    Vector3            position,
                       lookAt;
    Vector2            tilt,
                       tiltScale;
    Matrix             view,
                       projection,
                       viewProjection,
                       inverse;
    float              fov,
                       depthScale;
    GraphicsDevice    &device;
        
    void               Update();
};

class VisibleBounds
{    
public:
        
    bool          valid[4];
    Vector3       at[4];
        
    VisibleBounds(const Vector4 &plane, const OverheadCamera &camera);
    
    void Print() const;
    
private:
    const Vector2 LO, HI;    
};

#endif // _OVERHEADCAMERA_H
