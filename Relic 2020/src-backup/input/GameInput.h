#ifndef _GAMEINPUT_H
#define _GAMEINPUT_H

#include "core/core.h"
#include "core/point.h"

class Toggle
{
public:
    
    typedef void (*OnStateChanged_t)(Toggle *toggle, void *context);

    inline Toggle(bool v = false) : state(v), changed(0), context(0) {}
    
    inline bool GetState()       const { return state; }
    inline void SetState(bool v)       { if (state != v) { state = v; if (changed) changed(this, context); } }
    
    inline void SetStateChangedFunction(OnStateChanged_t fn, void *contxt) { changed = fn; context = contxt; }
    
private:    
    
    bool              state;
    OnStateChanged_t  changed;
    void             *context;
};

class InputVector
{
public:
    
    inline Vector2 GetVector()
    {
        return Vector2(_clamp((vector.x + offset.x) * scale.x, clamp.x, clamp.y),
                       _clamp((vector.y + offset.y) * scale.y, clamp.z, clamp.w));        
    }
    
    inline void SetVector(const Vector2 &v)
    {
        vector = v;
    }
        
    inline InputVector(const Vector2 &offset, const Vector2 &scale, const Vector4 &clamp) 
        : scale(scale), offset(offset), clamp(clamp) {}
    
    inline InputVector(const Vector2 &offset, const Vector2 &scale, const Vector2 &clamp) 
        : scale(scale), offset(offset), clamp(-clamp.x, clamp.x, -clamp.y, clamp.y) {}

    inline InputVector(const Vector2 &scale, const Vector2 &clamp) 
        : scale(scale), clamp(-clamp.x, clamp.x, -clamp.y, clamp.y) {}
    
private:
    
    inline static float _clamp(float v, float min, float max)
    {
        return (v < min) ? min : ((v > max) ? max : v);
    }
    
    Vector2 vector,
            scale,
            offset;
    Vector4 clamp;
};

class GameInput
{
public:

    InputVector move,
                camera;
    Toggle      resume,
                back,
                start;
    
    bool        enabled;
        
    void Update(const GameTime &time)
    {
        if (enabled) {
            GetInput();
        }
    }
    
    enum TouchAction { TouchAction_Began = 0, TouchAction_Moved, TouchAction_Ended, TouchAction_Stationary, TouchAction_MAX };
    
    GameInput();
    
    inline void iPhoneTouch(const core::Point &at) { touched = true; touch = at; }
    
    void ScreenTouch(int index, const core::Point &at, TouchAction ta, const char *function);
    void ScreenTouchBegan(int index, const core::Point &at);
    void ScreenTouchMoved(int index, const core::Point &at);
    void ScreenTouchEnded(int index, const core::Point &at);    
    void ScreenTouchStationary(int index, const core::Point &at);
    
    void SetTransform(const Matrix4f &t) { transform = t; }
    
    void ClearTouches();
    
    const core::Point &GetTouch() const { return touch; }
    
    const Vector3      GetTouchPoint(TouchAction ta) const;
    const Vector3      GetTransformedTouchPoint(TouchAction ta) const;
    bool               GetTouchActionOccurred(TouchAction ta) const { return flags[ta]; }
    void               ClearTouchAction(TouchAction ta) { flags[ta] = false; }
    
    static GameInput *GetInstance();
    
private:
    
    void SetTouchAction(TouchAction action, int index, const core::Point &at);
    
    bool        touched;
    core::Point touch;
    
    Matrix4f    transform;
    
    bool        flags[TouchAction_MAX];
    core::Point points[TouchAction_MAX];
    
    void GetInput();
};

#endif // _GAMEINPUT_H
