#ifndef _ANIMATOR_H
#define _ANIMATOR_H

#include "core/xna.h"

class IAnimator;

typedef void (*AnimationComplete)(IAnimator *animator, void *user);

class IAnimator
{
public:
    
    virtual ~IAnimator() {}
        
    virtual void           Reset()                                           = 0;
    virtual const Vector4 &GetColor()                                  const = 0;
    virtual const Matrix  &GetMatrix()                                 const = 0;
    virtual const Matrix  &Update(const GameTime &time)                      = 0;
    virtual const Matrix  &ForceUpdate(const GameTime &time)                 = 0;
    virtual IAnimator     *CopyIfNecessary()                                 = 0;
    virtual void           SetScale(const Vector3 &scale)                    = 0;
    virtual void           SetCompleteEvent(AnimationComplete c, void *user) = 0;
};

class NullAnimator : public IAnimator
{
public:    

    void           Reset()                                           {}
    const Vector4 &GetColor()  const;
    const Matrix  &GetMatrix() const;
    const Matrix  &Update(const GameTime &time);
    const Matrix  &ForceUpdate(const GameTime &time);
    IAnimator     *CopyIfNecessary()                                 { return this; }
    void           SetScale(const Vector3 &scale)                    {}
    void           SetCompleteEvent(AnimationComplete c, void *user) {}
};

class RotateAnimator : public IAnimator
{
public:
        
    RotateAnimator(const Vector3 &angles) : angles(angles) { Reset(); }
    
    void           Reset();
    void           SetScale(const Vector3 &scale);
    const Vector4 &GetColor()  const;
    const Matrix  &GetMatrix() const;
    const Matrix  &Update(const GameTime &time);
    const Matrix  &ForceUpdate(const GameTime &time);
    IAnimator     *CopyIfNecessary();
    void           SetCompleteEvent(AnimationComplete c, void *user);

private:

    Vector3 angles,
            rotation,
            scale;
    Matrix  matrix;
    int     elapsed;
};

class TextAnimator : public IAnimator
{
public:
        
    TextAnimator(const Vector3 &values) : values(values) { Reset(); }
        
    void          Reset();
    void          SetScale(const Vector3 &scale);
    const Vector4 &GetColor() const;    
    const Matrix &GetMatrix() const;
    const Matrix &Update(const GameTime &time);
    const Matrix &ForceUpdate(const GameTime &time);
    IAnimator    *CopyIfNecessary();
    void          SetCompleteEvent(AnimationComplete c, void *user);
        
private:
        
    Vector3 values,
            accumulated,
            scale;
    Vector4 color;
    Matrix  matrix;
    int     elapsed;
};

class TimedAnimator : public IAnimator
{
public:    
    
    TimedAnimator(IAnimator *animator, int ms, bool loop);
    
    void           Reset();
    void           SetScale(const Vector3 &scale);
    const Vector4 &GetColor() const;    
    const Matrix  &GetMatrix() const;
    const Matrix  &Update(const GameTime &time);
    const Matrix  &ForceUpdate(const GameTime &time);    
    IAnimator     *CopyIfNecessary();
    void           SetCompleteEvent(AnimationComplete c, void *user);

private:
    
    IAnimator         *animator;
    int                msElapsed,
                       msMaximum;
    AnimationComplete  complete;
    Matrix             matrix;
    bool               fired;
    bool               loop;
    void              *user;
};

#endif // _ANIMATOR_H