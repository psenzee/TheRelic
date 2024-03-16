#pragma once

#include <array>

#include "math_typedefs.h"
#include "std_array_mat.h"

class Camera
{
public:
    
    static constexpr const float PI         = 3.141592653589793f;
    static constexpr const float RAD_TO_DEG = 180.f / PI;
    static constexpr const float DEG_TO_RAD = PI / 180.f;
    
    Camera(const size2u_t &frame_size, float fov_degrees);
        
    inline const vec3_t   &look_at()                       const { return _look_at; }
    inline Camera         &set_look_at(const vec3_t &v)          { _look_at = v; update(); return *this; }
    inline float           fov_degrees()                   const { return float(_fov * RAD_TO_DEG); }
    inline Camera         &set_fov_degrees(float v)              { _fov = float(v * DEG_TO_RAD); update(); return *this; }
    inline float           fov_radians()                   const { return _fov; }
    inline Camera         &set_fov_radians(float v)              { _fov = v; update(); return *this; }
    inline float           depth_scale()                   const { return _depth_scale; }
    inline Camera         &set_depth_scale(float v)              { _depth_scale = v; update(); return *this; }
    inline const vec3_t   &up()                            const { return _up; }
    inline Camera         &set_up(const vec3_t &up)              { _up = up; return *this; }
    inline const vec2_t   &tilt()                          const { return _tilt; }
    inline Camera         &set_tilt(const vec2_t &v)             { _tilt = v; update(); return *this; }
    inline const vec2_t   &tilt_scale()                    const { return _tilt_scale; }
    inline Camera         &set_tilt_scale(const vec2_t &v)       { _tilt_scale = v; update(); return *this; }

    inline vec3_t          view_vector()                   const { return normal(_look_at - _position); }

    inline const vec3_t   &position()                      const { return _position; }
    inline const mat4_t   &view()                          const { return _view; }
    inline const mat4_t   &projection()                    const { return _projection; }
    inline const mat4_t   &view_projection()               const { return _view_projection; }
    inline const mat4_t   &inverse_view_projection()       const { return _inv_view_projection; }
    inline const mat4_t   &inverse_view()                  const { return _inv_view; }
    inline const mat4_t   &inverse_projection()            const { return _inv_projection; }
    

    inline Camera         &_set_view(const mat4_t &m)            { _view = m; return *this; } // *hack* for tweaking
    inline Camera         &_set_projection(const mat4_t &m)      { _projection = m; return *this; } // *hack* for tweaking
        
private:
        
    vec3_t            _position,
                      _look_at,
                      _up;
    vec2_t            _tilt,
                      _tilt_scale;
    mat4_t            _view,
                      _projection,
                      _view_projection,
                      _inv_view,
                      _inv_projection,
                      _inv_view_projection;
    float             _fov,
                      _depth_scale;
    size2u_t          _frame_size;
        
    void              update();
    
    vec3_t            camera_position(const vec3_t &look_at, float depth_scale);
    vec3_t            camera_position_tilt(const vec3_t &look_at, const vec2_t &tilt, const vec2_t &tilt_scale, float depth_scale);
};

