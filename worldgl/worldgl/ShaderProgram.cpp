#include "ShaderProgram.h"
#include "gl_utils.h"
#include "VertexFormat.h"

#include <iostream>
#include <thread>
#include <cstdio>

ShaderProgram::ShaderProgram(const char *vertex_source, const char *fragment_source) :
    _program_id(0), _fragment_shader_id(0), _vertex_shader_id(0)
{
    load_vertex_shader(vertex_source);
    load_fragment_shader(fragment_source);

    _program_id = _GL(glCreateProgram());
    _GL(glAttachShader(_program_id, _vertex_shader_id));
    _GL(glAttachShader(_program_id, _fragment_shader_id));
    _GL(glBindFragDataLocation(_program_id, 0, "o_color"));
    for (unsigned i = 0; i < 16; i++) {
        _GL(glBindAttribLocation(_program_id, i, VertexField::nameForLocationId(i)));
    }
    _GL(glLinkProgram(_program_id));
    _GL(glUseProgram(_program_id));
}

ShaderProgram::~ShaderProgram()
{
    _GL(glDeleteProgram(_program_id));
    _GL(glDeleteShader(_fragment_shader_id));
    _GL(glDeleteShader(_vertex_shader_id));
}

bool ShaderProgram::load_vertex_shader(const char *source)
{
    GLint compiled = false;
    _vertex_shader_id = _GL(glCreateShader(GL_VERTEX_SHADER));
    _GL(glShaderSource(_vertex_shader_id, 1, &source, NULL));
    _GL(glCompileShader(_vertex_shader_id));
    _GL(glGetShaderiv(_vertex_shader_id, GL_COMPILE_STATUS, &compiled));
    if (!compiled) {
        std::cerr << "Failed to compile vertex shader!" << std::endl;
        print_log(_vertex_shader_id);
        return false;
    }
    return true;
}

bool ShaderProgram::load_fragment_shader(const char *source)
{
    GLint compiled = false;
    _fragment_shader_id = _GL(glCreateShader(GL_FRAGMENT_SHADER));
    _GL(glShaderSource(_fragment_shader_id, 1, &source, NULL));
    _GL(glCompileShader(_fragment_shader_id));
    _GL(glGetShaderiv(_fragment_shader_id, GL_COMPILE_STATUS, &compiled));
    if (!compiled) {
        std::cerr << "Failed to compile fragment shader!" << std::endl;
        print_log(_fragment_shader_id);
        return false;
    }
    return true;
}

void ShaderProgram::print_log(unsigned shader_id)
{
    int InfoLogLength = 0;
    int CharsWritten = 0;

    glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, & InfoLogLength);

    if (InfoLogLength > 0) {
        GLchar * InfoLog = new GLchar [InfoLogLength];
        glGetShaderInfoLog(shader_id, InfoLogLength, & CharsWritten, InfoLog);
        std::cout << "Shader Info Log:" << std::endl << InfoLog << std::endl;
        delete [] InfoLog;
    }
}

