#include "UiTransition.h"
#include "UiCore.h"

void UiTransition::NotifyVisible(bool visible)
{
    if (visible && mVisibilityFrames == 0)
        mVisibilityFrames = mMaxVisibilityFrames;
    else if (!visible && mVisibilityFrames == 0)
        mVisibilityFrames = -mMaxVisibilityFrames;
}

void UiTransition::Update()
{
    if (mVisibilityFrames > 0)
        mVisibilityFrames--;
    else if (mVisibilityFrames < 0)
        mVisibilityFrames++;
}