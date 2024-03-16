#ifndef _GLSTATES_H
#define _GLSTATES_H

#include "core/core.h"
#include "render/GLIncludes.h"

#include "core/core_assert.h"

class BoolStateSetter
{
public:
    inline BoolStateSetter(int state) : mState(state) {}
    inline void Set(bool value) { if (value) glEnable(mState); else glDisable(mState); }
private:
    int mState;
};
/*
class TriStateSetter
{
public:
    inline TriStateSetter(int state) : mState(state) {}
    inline void Set(const TriState &value) { if (!value.IsUnknown()) { if (value.ToBool()) glEnable(mState); else glDisable(mState); } }
private:
    int mState;
};
*/

class DepthWriteSetter
{
public:    
    inline void Set(bool value) { glDepthMask(value ? GL_TRUE : GL_FALSE); }
private:
};

class BoolClientStateSetter
{
public:
    inline BoolClientStateSetter(int state) : mState(state) {}
    inline void Set(bool value) { if (value) glEnableClientState(mState); else glDisableClientState(mState); }
private:
    int mState;
};

class LightScalarSetter
{
public:
    inline LightScalarSetter(int light, int state) : mId(light), mState(state) {}   
    inline void Set(float value) { glLightf(mId, mState, value); }
private:
    int mId, mState;
};

template <typename T>
class LightVectorSetter
{
public:
    LightVectorSetter(int light, int state) : mId(light), mState(state) {}   
    inline void Set(const T &value) { glLightfv(mId, mState, static_cast<const float *>(&value)); }
private:
    int mId, mState;
};

class MaterialScalarSetter
{
public:
    inline MaterialScalarSetter(int state) : mState(state) {}
    inline void Set(float value) { glMaterialf(GL_FRONT_AND_BACK, mState, value); }
private:
    int mState;
};

template <typename T>
class MaterialVectorSetter
{
public:
    MaterialVectorSetter(int state) : mState(state) {}   
    inline void Set(const T &value) { glMaterialfv(GL_FRONT_AND_BACK, mState, static_cast<const float *>(&value)); }
private:
    int mState;
};

template <typename SetterT, typename T>
class State
{
public:

    inline State(const SetterT &setter) : mSetter(setter), mKnown(false) {}
    
    inline void Set(const T &v)             { if (v != mValue || !mKnown) ForceSet(v); }
    inline void ForceSet(const T &v)        { mSetter.Set(v); mValue = v; mKnown = true; }
    inline bool Get()                 const { core_assert(mKnown); return mValue; }

private:

    SetterT mSetter;
    bool    mValue;
    bool    mKnown;
};

class GLBoolState            : public State<BoolStateSetter, bool>                  { public: inline GLBoolState(int state) : State<BoolStateSetter, bool>(BoolStateSetter(state)) {} };
//class GLTriState             : public State<TriStateSetter, TriState>               { public: inline GLTriState(int state) : State<TriStateSetter, bool>(TriStateSetter(state)) {} };
class GLBoolClientState      : public State<BoolClientStateSetter, bool>            { public: inline GLBoolClientState(int state) : State<BoolClientStateSetter, bool>(BoolClientStateSetter(state)) {} };
class GLDepthWriteState      : public State<DepthWriteSetter, bool>                 { public: inline GLDepthWriteState() : State<DepthWriteSetter, bool>(DepthWriteSetter()) {} };
class GLLightScalarState     : public State<LightScalarSetter, float>               { public: inline GLLightScalarState(int light, int state) : State<LightScalarSetter, float>(LightScalarSetter(light, state)) {} };
class GLLightVector4State    : public State<LightVectorSetter<Vector4>, Vector4>    { public: inline GLLightVector4State(int light, int state) : State<LightVectorSetter<Vector4>, Vector4>(LightVectorSetter<Vector4>(light, state)) {} };
class GLMaterialScalarState  : public State<MaterialScalarSetter, float>            { public: inline GLMaterialScalarState(int state) : State<MaterialScalarSetter, float>(MaterialScalarSetter(state)) {} };
class GLMaterialVector4State : public State<MaterialVectorSetter<Vector4>, Vector4> { public: inline GLMaterialVector4State(int state) : State<MaterialVectorSetter<Vector4>, Vector4>(MaterialVectorSetter<Vector4>(state)) {} };

struct GLLightState
{
    int                 light;
    GLBoolState         enable;
    GLLightScalarState  attenuationQuadratic;
    GLLightScalarState  attenuationLinear;
    GLLightVector4State ambient;
    GLLightVector4State diffuse;
    GLLightVector4State specular;
    GLLightVector4State position;

    GLLightState(int light) 
        : light      (light), 
          enable     (light),
          attenuationQuadratic(light, GL_QUADRATIC_ATTENUATION),
          attenuationLinear(light, GL_LINEAR_ATTENUATION),
          ambient    (light, GL_AMBIENT),
          diffuse    (light, GL_DIFFUSE),
          specular   (light, GL_SPECULAR),
          position   (light, GL_POSITION) {}
};

struct GLMaterialState
{
    GLMaterialScalarState  shininess;
    GLMaterialVector4State ambient;
    GLMaterialVector4State diffuse;
    GLMaterialVector4State specular;
    GLMaterialVector4State emissive;

    GLMaterialState() 
        : shininess(GL_SHININESS),
          ambient  (GL_AMBIENT),
          diffuse  (GL_DIFFUSE),
          specular (GL_SPECULAR),
          emissive (GL_EMISSION) {}
};

struct GLStates
{
    static GLBoolState       blend;
    static GLBoolState       cullFace;
    static GLBoolState       depthTest;
    static GLBoolState       fog;
    static GLDepthWriteState depthWrite;
                                    
    static GLBoolState       lighting;
    static GLBoolState       texture;
    static GLBoolState       normalize;
    static GLBoolState       rescaleNormal;
    
    static GLBoolClientState vertices;
    static GLBoolClientState normals;
    static GLBoolClientState uvs;
    
    static GLLightState      light[];
                                    
    static GLMaterialState   material;
};

#endif // _GLSTATES_H
