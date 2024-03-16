#ifndef _PROJECTIONUTILT_H
#define _PROJECTIONUTILT_H

#include <string.h>

#include "math.h"
#include "vectors3.h"

template <class T>
class ProjectionUtilT
{
public:

	static void fov(T *m, T fov, T aspectratio, T znear, T zfar, bool left = true);
	static void orthogonal(T *m, T viewvolwidth, T viewvolheight, T znear, T zfar, bool left = true);
	static void perspective(T *m, T viewvolwidth, T viewvolheight, T znear, T zfar, bool left = true);
	static void shadow(T *m, const T *lightv3, const T *planenormalv3, T planed, T vanish);
	static void lookat(T *m, const T *posv3, const T *targetv3, const T *upv3, bool left = true);
};

// template implementation..

// check these against directx's..

inline static int index(int i, int j) { return i * 16 + j; }

#include "vectors3.h"

template <class T>
static void ProjectionUtilT<T>::fov(T *m, T fov, T aspectratio, T znear, T zfar, bool left = true)
{
    T handed = left ? 1.0f : -1.0f,
      h      = (T)(cos(fov / 2) / sin(fov / 2)),
	  w      = h / aspectratio;
    perspective(m, w, h, znear, zfar, left);
}

template <class T>
static void ProjectionUtilT<T>::orthogonal(T *m, T viewvolwidth, T viewvolheight, T znear, T zfar, bool left = true)
{
    T handed = left ? 1.0f : -1.0f;
    memset(&m, 0, sizeof(T) * 16);

	*(m + index(0, 0)) = 2 / viewvolwidth;
	*(m + index(1, 1)) = 2 / viewvolheight;
	*(m + index(2, 2)) = 1 / (znear - zfar);
	*(m + index(2, 3)) = znear / (znear - zfar);
	*(m + index(3, 3)) = handed;
}

template <class T>
static void ProjectionUtilT<T>::perspective(T *m, T viewvolwidth, T viewvolheight, T znear, T zfar, bool left = true)
{
    T handed = left ? 1.0f : -1.0f;
    memset(&m, 0, sizeof(T) * 16);

	*(m + index(0, 0)) = 2 * znear / viewvolwidth;
	*(m + index(1, 1)) = 2 * znear / viewvolheight;
	*(m + index(2, 2)) = zfar / (zfar - znear);
	*(m + index(3, 2)) = handed;
	*(m + index(2, 3)) = znear * zfar / (znear - zfar);
}

template <class T>
static void ProjectionUtilT<T>::shadow(T *m, const T *lightv3, const T *planenormalv3, T planed, T vanish)
{
    T *lv = lightv3, pn[3];
    math::norm3(planenormalv3, pn);
    T  d  = math::dot3(lv, pn);


	*(m + index(0, 0)) = pn[0]  * lv[0] + d;
	*(m + index(1, 0)) = pn[0]  * lv[1];
	*(m + index(2, 0)) = pn[0]  * lv[2];
	*(m + index(3, 0)) = pn[0]  * vanish;

	*(m + index(0, 1)) = pn[1]  * lv[0];
	*(m + index(1, 1)) = pn[1]  * lv[1] + d;
	*(m + index(2, 1)) = pn[1]  * lv[2];
	*(m + index(3, 1)) = pn[1]  * vanish;

	*(m + index(0, 2)) = pn[2]  * lv[0];
	*(m + index(1, 2)) = pn[2]  * lv[1];
	*(m + index(2, 2)) = pn[2]  * lv[2] + d;
	*(m + index(3, 2)) = pn[2]  * vanish;

	*(m + index(0, 3)) = planed * lv[0] + d;
	*(m + index(1, 3)) = planed * lv[1];
	*(m + index(2, 3)) = planed * lv[2];
	*(m + index(3, 3)) = planed * vanish;
}

template <class T>
static void ProjectionUtilT<T>::lookat(T *m, const T *posv3, const T *targetv3, const T *upv3, bool left = true)
{
    T handed = left ? 1.0f : -1.0f;
    memset(&m, 0, sizeof(T) * 16);

    T zaxis[3], xaxis[3], yaxis[3];

    math::sub3(targetv3, posv3, zaxis);
    math::mul3(zaxis, handed, zaxis);
    math::norm3(zaxis, zaxis);

    math::cross3(upv3, zaxis, xaxis);
    math::norm3(xaxis, xaxis);

    math::cross3(zaxis, xaxis, yaxis);

	*(m + index(0, 0)) = xaxis[0]; *(m + index(1, 0)) = yaxis[0]; *(m + index(2, 1)) = zaxis[0];
	*(m + index(0, 1)) = xaxis[1]; *(m + index(1, 1)) = yaxis[1]; *(m + index(2, 1)) = zaxis[1];
	*(m + index(0, 2)) = xaxis[2]; *(m + index(1, 2)) = yaxis[2]; *(m + index(2, 2)) = zaxis[2];

	*(m + index(0, 3)) = -math::dot3(xaxis, pos);
	*(m + index(1, 3)) = -math::dot3(yaxis, pos);
	*(m + index(2, 3)) = -math::dot3(zaxis, pos);
	*(m + index(3, 3)) = 1.0f;
}

#endif  /** _PROJECTIONUTILT_H */