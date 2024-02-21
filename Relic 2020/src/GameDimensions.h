#ifndef _GAMEDIMENSIONS_H
#define _GAMEDIMENSIONS_H

#include "core/core.h"
#include "core/aabox.h"
#include "core/range2f.h"

class GameDimensions
{
public:
               
    GameDimensions(const Vector2 &renderFrameSize,
                   const Vector2 &touchFrameSize,
                   const Vector2 &referenceSize, bool landscape = true);
    
    inline const Vector2  &GetRenderFrameSize()    const { return mRenderFrameSize; }
    inline const Vector2  &GetTouchFrameSize()     const { return mTouchFrameSize; }
    inline const Vector2  &GetReferenceSize()      const { return mReferenceSize; }
    inline const Vector2  &GetViewportSize()       const { return mViewportSize; }
    inline bool            IsLandscape()           const { return mLandscape; }
    
    inline const Matrix4f &GetTouchTransform()     const { return mTouchTransform; }
    inline const AABox    &GetBounds()             const { return mBounds; }
    inline const AABox    &GetReferenceBounds()    const { return mReferenceBounds; }
    
    inline float           GetLeftEdge()           const { return mBounds.minimum.x; }
    inline float           GetRightEdge()          const { return mBounds.maximum.x; }
    inline float           GetTopEdge()            const { return mBounds.minimum.y; }
    inline float           GetBottomEdge()         const { return mBounds.maximum.y; }

    void                   Print()                 const;
    
private:

    AABox    mBounds;
    AABox    mReferenceBounds;
    
    Vector2  mRenderFrameSize;
    Range2f  mRenderFrameRange;
    float    mRenderFrameAspectRatio;
    
    Vector2  mTouchFrameSize;
    Range2f  mTouchFrameRange;
    float    mTouchFrameAspectRatio;
    
    Vector2  mReferenceSize;
    Range2f  mReferenceRange;
    float    mReferenceAspectRatio;
    
    Vector2  mViewportSize;
    
    bool     mLandscape;
   
    Matrix4f mTouchTransform;
    
    void PrintSize(const char *caption, const Vector2 &v) const;
    void PrintBounds(const char *caption, const AABox &ab) const;
};

#endif  // _GAMEDIMENSIONS_H
