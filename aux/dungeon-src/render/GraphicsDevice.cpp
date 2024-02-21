/*
 *  GraphicsDevice.cpp
 *  GLGravity
 *
 *  Created by Paul Senzee on 12/29/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "GraphicsDevice.h"
#include "platform/GLUtils.h"

#include "core/xna.h"

void GraphicsDevice::EnableDepthTest(bool v)
{
    if (v) glEnable (GL_DEPTH_TEST);
    else   glDisable(GL_DEPTH_TEST);    
}

void GraphicsDevice::EnableDepthWrite(bool v)
{
    glDepthMask(v ? GL_TRUE : GL_FALSE);    
}

void GraphicsDevice::EnableLighting(bool v)
{
    if (v) glEnable (GL_LIGHTING);
    else   glDisable(GL_LIGHTING);    
}
        
void GraphicsDevice::SetProjection(const Matrix &m)
{
     //Set the OpenGL projection matrix
     glMatrixMode(GL_PROJECTION);
     glLoadMatrixf((float *)&m);
     glMatrixMode(GL_MODELVIEW); //Make the OpenGL modelview matrix the default
}
