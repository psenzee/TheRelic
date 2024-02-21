/*
 *  RollingBall.cpp
 *  GLGravity
 *
 *  Created by Paul Senzee on 12/30/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "RollingBall.h"

#include "xml/XmlReadContext.h"
#include "render/OverheadCamera.h"
#include "render/Renderable.h"
#include "render/LocalGameServices.h"
#include "render/Material.h"
#include "render/GraphicsDevice.h"

Vector3 RollingBall::THIS_UP     (0.0f,   0.0f,   1.0f);
Vector3 RollingBall::BALL_OFFSET (0.0f, -16.0f, 104.0f);
float   RollingBall::RADIUS      = 16.0f;
            
RollingBall::RollingBall(XmlElement *options, const char *name) : bounciness(1.0f / 3.0f), conservation(0.0f), services(0), ballDynamic(0), ballStatic(0), ballShadow(0), ballHalo(0), ballHaloMAD(0), ballHaloBump(0), ballHaloLeave(0), haloState(NORMAL), halofader(0.0f)
{
    /*
    XmlElement xml = XmlUtil.GetElementWithKeyEqualTo(options, "ball", "name", name);
    this.bounciness   = XmlRenderableReader.ReadFloat("bounciness",   xml, 1.0f / 3.0f);
    this.conservation = XmlRenderableReader.ReadFloat("conservation", xml, 0.0f);
     */
}
            
void RollingBall::Load(LocalGameServices *services, XmlReadContext *context, const char *type)
{
    char tag[128];
    this->services    = services;
    sprintf(tag, "%s.Dynamic", type);
    this->ballDynamic   = context->objects[tag];
    sprintf(tag, "%s.Static", type);    
    this->ballStatic    = context->objects[tag];
    sprintf(tag, "%s.Shadow", type);    
    this->ballShadow    = context->objects[tag];
    sprintf(tag, "%s.Halo", type);
    this->ballHalo      = context->objects[tag];
    sprintf(tag, "%s.Halo.Leave", type);
    this->ballHaloLeave = context->objects[tag];
    sprintf(tag, "%s.Halo.MutualDestruction", type);
    this->ballHaloMAD   = context->objects[tag];    
    sprintf(tag, "%s.Halo.Bump", type);    
    this->ballHaloBump  = context->objects[tag];        
}

void RollingBall::Render(const OverheadCamera &camera, const Vector3 &position, const Vector3 &offset)
{
    Vector3     p   (position + offset + BALL_OFFSET),
                 d   (p - last),
                 axis(d.normal().cross(THIS_UP));
    
    float angle = d.length() / RADIUS;
    if (fabsf(angle) > 0.001f)
    {
        Quaternion nr = Quaternion::CreateFromAxisAngle(axis.normal(), angle);
        rotation = rotation * nr;
        rotation.normalize();
    }
    translate.translate(p);
    Matrix m = rotation.matrix() * translate;
    if (ballDynamic)
        ballDynamic->Render(*(services->device), m,         camera, true);
    if (ballStatic)    
    {
        //glDisable(GL_DEPTH_TEST);
        services->device->EnableDepthTest(false);
        ballStatic ->Render(*(services->device), translate, camera, true);
        //glEnable(GL_DEPTH_TEST);
        services->device->EnableDepthTest(true);
    }
    last = p;
}

// this assumes that Render() was called earlier this frame
void RollingBall::RenderShadow(const OverheadCamera &camera)
{
    if (ballShadow)
        ballShadow->Render(*(services->device), translate, camera, true);
}

// this assumes that Render() was called earlier this frame
void RollingBall::RenderHalo(const OverheadCamera &camera)
{
    Renderable *halo = 0;
    switch (haloState)
    {
    case MUTUAL_DESTRUCTION: halo = ballHaloMAD;   break;
    case BUMP:               halo = ballHaloBump;  break;
    case LEAVE:              halo = ballHaloLeave; break;
    case NORMAL: default:    halo = ballHalo;      break;
    }
    if (!halo)
        halo = ballHalo;
    if (halo)
    {
        // $HACK translate the halo up by 16.0f ($TODO until we change the mesh)
        Matrix t, s;
        s.scale(Vector3(2.0f, 2.0f, 1.0f));
        t.translate(Vector3(0.0f, 32.0f, -100.0f));
        t = s * t;
        t = t * translate;
        // $HACK end
        services->device->EnableDepthTest(false);
        static Material material;
        material.ambient = Vector4(0.25f, 0.25f, 0.25f, 1.0f);
        material.diffuse = Vector4(1.0f, 1.0f, 1.0f, halofader.GetValue());
        material.Set(*(services->device));
        halo->Render(*(services->device), t, camera, true);        
        services->device->EnableDepthTest(true);
    }
}
