/*
 *  DeviceTexture.cpp
 *  GLGravity
 *
 *  Created by Paul Senzee on 12/29/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "DeviceTexture.h"
#include "GLUtils.h"

DeviceTexture::DeviceTexture(const char *filename) : texid(0), size(0, 0)
{
    char newname[1024];
    sprintf(newname, "%s.png", filename);
    texid = LoadTexture(newname, &size);
}

DeviceTexture::~DeviceTexture()
{
}

bool DeviceTexture::Loaded() const
{
    return texid && glIsTexture(texid);
}

void DeviceTexture::Set(GraphicsDevice &device, unsigned int blendsrc, unsigned int blenddst)
{
    if (!Loaded())
        return;
    glEnable(GL_TEXTURE_2D);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, /*GL_LINEAR_MIPMAP_NEAREST*/GL_LINEAR);
    // Set a blending function to use
//    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glBlendFunc(blendsrc, blenddst);
    // Enable blending
    glEnable(GL_BLEND);
    //glDisable(GL_BLEND);
    glBindTexture(GL_TEXTURE_2D, texid);
}
