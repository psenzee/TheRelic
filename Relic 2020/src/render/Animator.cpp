#include "Animator.h"

static Matrix  IDENTITY;
static Vector4 COLOR_WHITE;

const Vector4 &NullAnimator::GetColor() const
{ 
    return COLOR_WHITE;
}

const Matrix &NullAnimator::GetMatrix() const
{ 
    return IDENTITY;
}

const Matrix &NullAnimator::Update(const GameTime &time)
{ 
    return IDENTITY;
}

const Matrix &NullAnimator::ForceUpdate(const GameTime &time)
{ 
    return IDENTITY;
}

void RotateAnimator::Reset()
{
    scale    = Vector3(1.0f, 1.0f, 1.0f);
    rotation = Vector3(0.0f, 0.0f, 0.0f);
    matrix   = IDENTITY;
    elapsed  = 0;
}

const Vector4 &RotateAnimator::GetColor() const
{ 
    return COLOR_WHITE;
}

void RotateAnimator::SetScale(const Vector3 &scale)
{
    this->scale = scale;
}

const Matrix &RotateAnimator::GetMatrix() const
{
    return matrix;
}

const Matrix &RotateAnimator::Update(const GameTime &time)
{
    elapsed += time.elapsed;
    if (elapsed > 16)
    {
        elapsed = 0;
        ForceUpdate(time);
    }
    return matrix;
}

const Matrix &RotateAnimator::ForceUpdate(const GameTime &time)
{
    rotation += angles * scale;
    matrix.eulerrotation(rotation);
    return matrix;
}

IAnimator *RotateAnimator::CopyIfNecessary()
{
    return new RotateAnimator(angles);
}

void RotateAnimator::SetCompleteEvent(AnimationComplete complete, void *user)
{
    // nothing..
}

void TextAnimator::Reset()
{
    scale       = Vector3(1.0f, 1.0f, 1.0f);
    accumulated = Vector3(0.01f, 0.01f, 0.01f);
    matrix      = IDENTITY;
    elapsed     = 0;
    color       = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
}

void TextAnimator::SetScale(const Vector3 &scale)
{
    this->scale = scale;    
}

const Vector4 &TextAnimator::GetColor() const
{
    return color;
}

const Matrix &TextAnimator::GetMatrix() const
{
    return matrix;
}

const Matrix &TextAnimator::Update(const GameTime &time)
{
    elapsed += time.elapsed;
    if (elapsed > 16) {
        elapsed = 0;
        ForceUpdate(time);
    }
    return matrix;
}

const Matrix &TextAnimator::ForceUpdate(const GameTime &time)
{
    accumulated += values * scale;
    if (accumulated.x > 1.0f/*1.5f*/)
        color.w *= 0.95f;
    matrix.translate(Vector3(0.0f, 0.0f, -accumulated.z * 128.0f));
    return matrix;
}

IAnimator *TextAnimator::CopyIfNecessary()
{
    return new TextAnimator(values);    
}

void TextAnimator::SetCompleteEvent(AnimationComplete complete, void *user)
{
    // ..
}

TimedAnimator::TimedAnimator(IAnimator *animator, int ms, bool loop) 
    : animator(animator), msElapsed(0), msMaximum(ms), loop(loop), complete(0), fired(false), user(0)
{
}

void TimedAnimator::Reset()
{
    msElapsed = 0;
    fired     = false;
    animator->Reset();
    matrix    = IDENTITY;    
}

const Vector4 &TimedAnimator::GetColor() const
{ 
    return animator->GetColor();
}

void TimedAnimator::SetScale(const Vector3 &scale)
{
    animator->SetScale(scale);
}

const Matrix &TimedAnimator::GetMatrix() const
{
    return matrix;
}

const Matrix &TimedAnimator::Update(const GameTime &time)
{
    if (!fired) {
        msElapsed += time.elapsed;
        if (msElapsed < msMaximum) {
            matrix = animator->Update(time);
        } else {
            fired = true;
            matrix = IDENTITY;
            if (complete != 0) {
                complete(this, user);
            }
            if (loop) {
                Reset();
            }
        }
    }
    return matrix;
}

const Matrix &TimedAnimator::ForceUpdate(const GameTime &time)
{
    if (!fired) {
        matrix = animator->ForceUpdate(time);
    }
    return matrix;
}

IAnimator *TimedAnimator::CopyIfNecessary()
{
    return new TimedAnimator(animator->CopyIfNecessary(), msMaximum, loop);
}

void TimedAnimator::SetCompleteEvent(AnimationComplete complete, void *user)
{
    this->complete = complete;
    this->user     = user;
}

