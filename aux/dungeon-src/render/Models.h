/*
 *  Models_.h
 *  GLGravity
 *
 *  Created by Paul Senzee on 12/17/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _MODELS_H
#define _MODELS_H

#include "core/xna.h"

#include "core/aabox.h"
#include "math/Collider.h"

#include "ContentCache.h"
#include "Texture.h"
#include "Transform.h"
#include "RenderStates.h"
#include "Animator.h"

#include <vector>

class DeviceModel;
class OverheadCamera;

class IModel
{
public:
        
    virtual ~IModel() {}    
        
    virtual DeviceModel            *GetDeviceModel()     = 0;
    
    virtual IModel                 *GetModel()           = 0;
    virtual std::vector<Texture *> &GetTextures()        = 0;
    virtual Transform               GetTransform() const = 0;
    virtual RenderStates           *GetStates()          = 0;
    virtual Collider               *GetCollider()        = 0;
    virtual IAnimator              *GetAnimator()        = 0;
    
    virtual bool                    IsStatic() const     = 0;
    
    virtual IModel                 *CopyIfNecessary()    = 0;
        
    virtual void                    Render(GraphicsDevice &device, const Matrix &transform, const OverheadCamera &camera, bool now) = 0;
    virtual bool                    IsVisible(const Matrix &transform, const OverheadCamera &camera) const = 0;
    virtual void                    Update(const GameTime &time) = 0;
    
    virtual    bool                    SetProperty(const char *key, const char *value) = 0;
};

class StaticModel : public IModel
{
public:
        
    static IAnimator *GlobalAnimator;
    
    static const Matrix &GetAnimatorMatrix();
        
    static void GlobalUpdate(const GameTime &time);
        
    DeviceModel            *GetDeviceModel()     { return model; }
    IModel                 *GetModel()           { return this; }
    std::vector<Texture *> &GetTextures()        { return textures; }
    Transform               GetTransform() const;
    RenderStates           *GetStates()          { return states; }
    Collider               *GetCollider()        { return collider; }
    IAnimator              *GetAnimator()        { return 0; }
    
    bool                    IsStatic() const     { return true; }
        
    IModel                 *CopyIfNecessary()    { return this; /* don't copy */ }
    
    bool                    SetProperty(const char *key, const char *value);
        
    StaticModel(DeviceModel *model,
                std::vector<Texture *> &textures, const Transform &transform,
                Collider *collider, RenderStates *states);
        
    void Render(GraphicsDevice &device, const Matrix &transform, const OverheadCamera &camera, bool now);
    bool IsVisible(const Matrix &transform, const OverheadCamera &camera) const;    
    void Update(const GameTime &time);
        
private:
        
//    static void SetupEffects(DeviceModel *model);
    
    static bool GlobalAnimatorMatrixIsIdentity;
        
    DeviceModel            *model;
    std::vector<Texture *>  textures;
    Transform               transform;
    RenderStates           *states;
    Collider               *collider;
    bool                    transformidentity,
                            cullinvisible;
    AABox                   bounds; // we're going to cache this for now
    float                   transparentz;
};

class ModelAnimator : public IModel // decorator
{
public:    
        
    DeviceModel            *GetDeviceModel()     { return model->GetDeviceModel(); }
    IModel                 *GetModel()           { return model->GetModel(); }
    std::vector<Texture *> &GetTextures()        { return model->GetTextures(); }
    Transform               GetTransform() const { return Transform(model->GetTransform().matrix * transform.matrix); }
    RenderStates           *GetStates()          { return model->GetStates(); }
    Collider               *GetCollider()        { return model->GetCollider(); }
    IAnimator              *GetAnimator()        { return animator; }
    
    bool                    IsStatic() const     { return false; }    
    
    IModel                 *CopyIfNecessary()    { return new ModelAnimator(this); }
    
    bool                    SetProperty(const char *key, const char *value) { return model->SetProperty(key, value); }
        
    void                    Render(GraphicsDevice &device, const Matrix &transform, const OverheadCamera &camera, bool now);
    bool                    IsVisible(const Matrix &transform, const OverheadCamera &camera) const;        
    void                    Update(const GameTime &time);
        
    ModelAnimator(IModel *model, IAnimator *animator);
        
    ModelAnimator(ModelAnimator *model);
        
private:
        
    IModel    *model;
    Transform  transform;
    IAnimator *animator;
};

class RenderableModel : public IModel
{
    IModel *model;
        
public:
        
    RenderableModel(ContentCache *content, const char *name, const char *file,
                    std::vector<Texture *> textures, const Transform &transform,
                    IAnimator *animator, Collider *collider, RenderStates *states);
        
    RenderableModel(RenderableModel *model);
        
    void Update(const GameTime &time);
    void Render(GraphicsDevice &device, const Matrix &transform, const OverheadCamera &camera, bool now);    
    bool IsVisible(const Matrix &transform, const OverheadCamera &camera) const;
        
    DeviceModel            *GetDeviceModel()     { return model->GetDeviceModel(); }
    IModel                 *GetModel()           { return model; }
    std::vector<Texture *> &GetTextures()        { return model->GetTextures(); }
    Transform               GetTransform() const { return model->GetTransform(); }
    RenderStates           *GetStates()          { return model->GetStates(); }
    Collider               *GetCollider()        { return model->GetCollider(); }
    IAnimator              *GetAnimator()        { return model->GetAnimator(); }

    bool                    IsStatic() const     { return model->IsStatic(); }
    
    bool                    SetProperty(const char *key, const char *value) { return model->SetProperty(key, value); }    
    
    IModel                 *CopyIfNecessary();
};

#endif // _MODELS_H