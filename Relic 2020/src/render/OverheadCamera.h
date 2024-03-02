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
        
    inline const Vector3   &GetLookAt()                    const { return mLookAt; }
    inline OverheadCamera  &SetLookAt(const Vector3 &v)          { mLookAt = v; Update(); return *this; }
    inline float            GetFovDegrees()                const { return float(mFov * RAD_TO_DEG); }
    inline OverheadCamera  &SetFovDegrees(float v)               { mFov = float(v * DEG_TO_RAD); Update(); return *this; }
    inline float            GetFovRadians()                const { return mFov; }
    inline OverheadCamera  &SetFovRadians(float v)               { mFov = v; Update(); return *this; }
    inline float            GetDepthScale()                const { return mDepthScale; }
    inline OverheadCamera  &SetDepthScale(float v)               { mDepthScale = v; Update(); return *this; }
    inline const Vector3   &GetUp()                        const { return mUp; }
    inline OverheadCamera  &SetUp(const Vector3 &up)             { mUp = up; return *this; }
    inline const Vector2   &GetTilt()                      const { return mTilt; }
    inline OverheadCamera  &SetTilt(const Vector2 &v)            { mTilt = v; Update(); return *this; }
    inline const Vector2   &GetTiltScale()                 const { return mTiltScale; }
    inline OverheadCamera  &SetTiltScale(const Vector2 &v)       { mTiltScale = v; Update(); return *this; }

    inline Vector3          GetViewVector()                const { return (mLookAt - mPosition).normal(); }

    inline const Vector3   &GetPosition()                  const { return mPosition; }
    inline const Matrix    &GetView()                      const { return mView; }
    inline const Matrix    &GetProjection()                const { return mProjection; }
    inline const Matrix    &GetViewProjection()            const { return mViewProjection; }
    inline const Matrix    &GetInverseViewProjection()     const { return mInvViewProjection; }
    inline const Matrix    &GetInverseView()               const { return mInvView; }
    inline const Matrix    &GetInverseProjection()         const { return mInvProjection; }
    

    inline OverheadCamera  &_SetView(const Matrix &m)            { mView = m; return *this; } // *hack* for tweaking
    inline OverheadCamera  &_SetProjection(const Matrix &m)      { mProjection = m; return *this; } // *hack* for tweaking
    
    Vector3                 GetLightPositionFromWorld(const Vector3 &world) const;
    
    Ray                     GetRayFromNormalizedScreen(const Vector2 &at) const;
    bool                    PickPlanePoint(const Vector4 &plane, const Vector2 &from, Vector3 &at) const;
        
private:
        
    Vector3            mPosition,
                       mLookAt,
                       mUp;
    Vector2            mTilt,
                       mTiltScale;
    Matrix             mView,
                       mProjection,
                       mViewProjection,
                       mInvView,
                       mInvProjection,
                       mInvViewProjection;
    float              mFov,
                       mDepthScale;
    GraphicsDevice    &mDevice;
        
    void               Update();
    
    Vector3            GetCameraPosition(const Vector3 &lookAt, float depthScale);
    Vector3            GetCameraPositionTilt(const Vector3 &lookAt, const Vector2 &tilt, const Vector2 &tiltScale, float depthScale);
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
