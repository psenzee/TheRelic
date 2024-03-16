#include "Camera.h"
#include "swizzle_xyzw.h"
#include "std_array_3.h"
#include "std_array_mat.h"
#include <math.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(const size2u_t &frame_size, float fov_degrees)
: _frame_size(frame_size), _tilt_scale { 1.0f, 1.0f }, _depth_scale(1.0f), _up { 1.0f, 0.0f, 0.0f }
{
    set_fov_degrees(fov_degrees);
}

vec3_t Camera::camera_position(const vec3_t &look_at, float depth_scale)
{
    float tan_angle = tanf(_fov * 0.5f);
    return v3(x(look_at), y(look_at), -((0.5f * y(_frame_size) / tan_angle) * depth_scale) + z(look_at));
}

vec3_t Camera::camera_position_tilt(const vec3_t &look_at, const vec2_t &tilt, const vec2_t &tilt_scale, float depth_scale)
{
    vec3_t camera = camera_position(look_at, depth_scale), tilt3 = v3(tilt, 0.f), tilt_scale3 = v3(tilt_scale, 0.f);
    float  d      = z(camera) - z(look_at); // distance from the player
    vec3_t ts     = tilt3 * tilt_scale3;
    vec3_t c      = look_at - normal(camera - ts);
    return look_at + (c * d);
}

void Camera::update()
{
    float ratio = _frame_size[0] / float(_frame_size[1]);
    _position              = camera_position_tilt(_look_at, _tilt, _tilt_scale, _depth_scale);
    _view                  = mat_look_at(_position, _look_at, up(), true);
    _projection            = mat_perspective_fov(_fov, ratio, 10.0f, 20000.0f, true); // $TODO WE'RE GOING TO HAVE TO REEVALUATE THIS FAR PLANE CHANGE!
    _view_projection       = mul_mat(_view, _projection);
    _inv_view_projection   = mat_invert(_view_projection);
    _inv_view              = mat_invert(_view);
    _inv_projection        = mat_invert(_projection);
}
