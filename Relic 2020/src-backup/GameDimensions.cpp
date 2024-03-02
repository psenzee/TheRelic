#include "GameDimensions.h"

GameDimensions::GameDimensions(
    const Vector2 &renderFrameSize,
    const Vector2 &touchFrameSize,
    const Vector2 &referenceSize,
    bool landscape
) : mRenderFrameSize(renderFrameSize),
    mTouchFrameSize(touchFrameSize),
    mViewportSize(touchFrameSize),
    mReferenceSize(referenceSize),
    mLandscape(landscape)
{
    Vector2 rrf(renderFrameSize), rf(referenceSize), tf(mTouchFrameSize);
    if (mLandscape) {
        mTouchFrameSize.y = (rrf.y * ((float)tf.x / (float)rrf.x));
        rrf = mRenderFrameSize = mRenderFrameSize.yx();
        rf = mReferenceSize = mReferenceSize.yx();
        tf = mTouchFrameSize = mTouchFrameSize.yx();
    }
    if (mLandscape) {
        mTouchFrameSize.y = (rrf.y * ((float)tf.x / (float)rrf.x));
    } else {
        mTouchFrameSize.x = (rrf.x * ((float)tf.y / (float)rrf.y));
    }
    tf = mTouchFrameSize;
    mViewportSize = mTouchFrameSize.yx();
    mTouchFrameAspectRatio = tf.x / tf.y;
    mRenderFrameAspectRatio = rrf.x / rrf.y;
    mReferenceAspectRatio = rf.x / rf.y;
    mBounds = AABox(Tuple3f(-(tf.x - rf.x) * 0.5f, 0.f, 0.f), Tuple3f(rf.x + (tf.x - rf.x) * 0.5f, tf.y, 0.f));
    mReferenceBounds = AABox(Tuple3f(0.0f, 0.0f, 0.0f), Tuple3f(rf.x, rf.y, 0.0f));
    mTouchTransform.identity();
    if (mLandscape) {
        float minx = mBounds.minimum.x,
              maxy = mBounds.maximum.y;
        mTouchTransform.set(
            0.f, -1.f, 0.f, 0.f,
            1.f,  0.f, 0.f, 0.f,
            0.f,  0.f, 1.f, 0.f,
           minx, maxy, 0.f, 1.f
        );
    }
    Print();
}

void GameDimensions::PrintSize(const char *caption, const Vector2 &v) const
{
    printf("%-32s    W  %10.2f H  %10.2f\n", caption, v.x, v.y);
}

void GameDimensions::PrintBounds(const char *caption, const AABox &ab) const
{
    printf("%-32s    X0 %10.2f Y0 %10.2f X1 %10.2f Y1 %10.2f\n", caption, ab.minimum.x, ab.minimum.y, ab.maximum.x, ab.maximum.y);
}

void GameDimensions::Print() const
{
    printf("------------------------------------------\n");
    PrintSize("Reference Size",    mReferenceSize);
    PrintSize("Render Frame Size", mRenderFrameSize);
    PrintSize("Touch Frame Size",  mTouchFrameSize);
    printf("%-32s    %s\n", "Landscape", mLandscape ? "Yes" : "No");
    PrintBounds("Bounds", mBounds);
    printf("Touch Transform\n");
    mTouchTransform.print();
    printf("------------------------------------------\n");
}
