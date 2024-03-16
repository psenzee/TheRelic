#pragma once

constexpr const char *VERTEX_SHADER = R"GLSL(

    #version 150
    precision mediump float;

    in vec3      a_position;
    in vec2      a_texture;
    out vec2     v_texture;
    uniform mat4 um_model_view_projection;

    void main()
    {
        gl_Position = vec4(a_position, 1.0) * um_model_view_projection;
        v_texture = a_texture;
    }

)GLSL";

constexpr const char *FRAGMENT_SHADER = R"GLSL(

    #version 150
    precision mediump float; // Mandatory OpenGL ES float precision

    in vec2           v_texture; // Sent in from Vertex Shader
    out vec4          o_color;
    uniform sampler2D u_sampler; // texture sent in

    void main()
    {
        o_color = texture(u_sampler, v_texture);
    }

)GLSL";


constexpr const char *UI_VERTEX_SHADER = R"GLSL(

    #version 150
    precision mediump float;

    in vec3      a_position;
    in vec2      a_texture;
    out vec2     v_texture;
    uniform mat4 um_view;

    void main()
    {
        gl_Position = vec4(a_position, 1.0) * um_view;
        v_texture = a_texture;
    }

)GLSL";

constexpr const char *UI_FRAGMENT_SHADER = R"GLSL(

    #version 150
    precision mediump float; // Mandatory OpenGL ES float precision

    in vec2           v_texture; // Sent in from Vertex Shader
    out vec4          o_color;
    uniform sampler2D u_sampler; // texture sent in

    void main()
    {
        o_color = texture(u_sampler, v_texture);
    }

)GLSL";
