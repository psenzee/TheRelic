/*
 *  OverheadCamera_.cpp
 *  GLGravity
 *
 *  Created by Paul Senzee on 12/17/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

/*
 */

#include "OverheadCamera.h"
#include "LocalGameServices.h"
//#include "GLUtils.h"
#include <math.h>

OverheadCamera::OverheadCamera(LocalGameServices *services, float fovDegrees) 
: services(services), tiltScale(1.0f, 1.0f), depthScale(1.0f)
{
    SetFovDegrees(fovDegrees);
}

Vector3 OverheadCamera::GetCameraPosition(const Vector3 &lookAt, float depthScale)
{
    float tanAngle = tanf(fov * 0.5f);
    return Vector3(lookAt.x, lookAt.y,
                   -((0.5f * services->frame.height / tanAngle) * depthScale) + lookAt.z);
}

Vector3 OverheadCamera::GetCameraPositionTilt(const Vector3 &lookAt, const Vector2 &tilt, const Vector2 &tiltScale, float depthScale)
{
    Vector3 camera = GetCameraPosition(lookAt, depthScale);
    float   d      = camera.z - lookAt.z; // distance from the player
    Vector3 c      = (lookAt - Vector3((camera.x - tilt.x * tiltScale.x),
                                       (camera.y - tilt.y * tiltScale.y),
                                       camera.z)).normal();
    return lookAt + (c * d);
}

Matrix CreateXNAPerspective(float w, float h, float znear, float zfar)
{
    float zvol = zfar / (zfar - znear);
    Matrix m;
    m.set(
                 w,  0,   0,             0,
                 0,  h,   0,             0,
                 0,  0,  -zvol,      -1.0f,
                 0,  0,  -znear * zvol,  0);
    return m;
}

Matrix CreateXNAPerspectiveFov(float fov, float aspectratio, float znear, float zfar)
{
    float h = (float)cosf(fov * 0.5f) / (float)sinf(fov * 0.5f);
    return CreateXNAPerspective(h / aspectratio, h, znear, zfar);
}

Matrix CreateXNALookAt(const Vector3 &eye, const Vector3 &at, const Vector3 &up)
{
    Vector3 zaxis = (eye - at).normal(),
            xaxis = up.cross(zaxis).normal(),
            yaxis = zaxis.cross(xaxis);
    float   xi    = -xaxis.dot(eye),
            yi    = -yaxis.dot(eye),
            zi    = -zaxis.dot(eye);
    Matrix m;
    m.set(xaxis.x, yaxis.x, zaxis.x, 0.0f,
          xaxis.y, yaxis.y, zaxis.y, 0.0f,
          xaxis.z, yaxis.z, zaxis.z, 0.0f,
          xi,      yi,      zi,      1.0f);
    return m;
}

Matrix CreateGLLookAt(const Vector3 &eye, const Vector3 &at, const Vector3 &up)
{
    Vector3 forward((at - eye).normal()),
            side(forward.cross(up).normal()),
            u(side.cross(forward));

    Matrix m;

    m.set( side.x,     side.y,     side.z,    0.0f,
           up.x,       up.y,       up.z,      0.0f,
          -forward.x, -forward.y, -forward.z, 0.0f,
          -eye.x,     -eye.y,     -eye.z,     1.0f);
    
    return m;
}

#define __glPi 3.14159265358979323846

Matrix CreateGLPerspectiveFov(float fovy, float aspect, float zNear, float zFar)
{
    float sine, cotangent, deltaZ;
    float radians = fovy / 2 * __glPi / 180;

    Matrix m;    
    
    deltaZ = zFar - zNear;
    sine = sinf(radians);
    if ((deltaZ == 0.0f) || (sine == 0.0f) || (aspect == 0.0f)) 
    {
        printf("Invalid perpective fov specified!\n");
        return m;
    }

    cotangent = cosf(radians) / sine;
    m.set(cotangent / aspect, 0.0f,      0.0f,                           0.0f,
          0.0f,               cotangent, 0.0f,                           0.0f,
          0.0f,               0.0f,     -(zFar + zNear) / deltaZ,       -1.0f,
          0.0f,               0.0f,      -2.0f * zNear * zFar / deltaZ,  0.0f);
    return m;
}

void OverheadCamera::Update()
{
    const Vector3 UP =   Vector3(1.0f, 0.0f, 0.0f); // rotated for iphone
                       //Vector3(0.0f, 1.0f, 0.0f);
    position       = GetCameraPositionTilt(lookAt, tilt, tiltScale, depthScale);
    view           = CreateXNALookAt(position, lookAt, UP);
    projection     = CreateXNAPerspectiveFov(fov, services->frame.width / float(services->frame.height), 10.0f, 2000.0f);
    viewProjection = view * projection;
    inverse        = viewProjection.invert();
}
 
Ray OverheadCamera::GetRayFromNormalizedScreen(const Vector2 &from) const
{
    return MathUtil::CreateRayFromNormalizedScreen(from, inverse);
}

bool OverheadCamera::PickPlanePoint(const Vector4 &plane, const Vector2 &from, Vector3 &at) const
{
    return MathUtil::Intersection(GetRayFromNormalizedScreen(from), plane, at);
}

VisibleBounds::VisibleBounds(const Vector4 &plane, const OverheadCamera &camera) : LO(-1.05f, -1.05f), HI(1.05f, 1.05f)
{
    memset(valid, 0, sizeof(valid));
    for (int i = 0; i < 4; i++)
        valid[i] = camera.PickPlanePoint(plane, Vector2((i & 1) ? HI.x : LO.x, (i & 2) ? HI.y : LO.y), at[i]);
}

#include "core/aabox.h"

void VisibleBounds::Print() const
{
    printf("vb: ");
    AABox box;
    for (int i = 0; i < 4; i++)
    {
        if (!valid[i])
            printf("(invalid) ");
        else
        {
            printf("(%.2f, %.2f, %.2f) ", at[i].x, at[i].y, at[i].z);
            box.insert(at[i]);
        }
    }
    printf("box ");
    printf("(%.2f, %.2f, %.2f) ", box.minimum.x, box.minimum.y, box.minimum.z);
    printf("(%.2f, %.2f, %.2f) ", box.maximum.x, box.maximum.y, box.maximum.z);
    printf("\n");
}