#include "ShaderProgram.h"
#include "Shader.h"
#include "platform/GLIncludes.h"
#include "GLUtils.h"
#include <iostream>

ShaderProgram::ShaderProgram() : mProgramId(0)
{
    mProgramId = _GL(glCreateProgram());
    for (size_t i = 0; i < Shader::MAX_SHADER_TYPES; i++) {
        mShaders[i] = nullptr;
    }
}

void ShaderProgram::SetActive()
{
    glUseProgram(GetProgramId());
}

void ShaderProgram::SetUniform(std::string_view name, const Matrix4f &m)
{
    if (mProgramId) {
        glUseProgram(mProgramId);
        uint64_t transformId = glGetUniformLocation(mProgramId, name.data());
        glUniformMatrix4fv(transformId, 1, GL_FALSE, m.data);
    }
}

void ShaderProgram::SetUniform(std::string_view name, size_t count, const float *v)
{
    if (mProgramId) {
        glUseProgram(mProgramId);
        uint64_t transformId = glGetUniformLocation(mProgramId, name.data());
        decltype(glUniform1fv) *fn[] = { glUniform1fv, glUniform2fv, glUniform3fv, glUniform4fv };
        (fn[count])(transformId, 1, v);
    }
}

void ShaderProgram::BindAttributes(std::span<const attribute_t> attributes) const
{
    for (auto &attribute : attributes) {
        glBindAttribLocation(mProgramId, attribute.first, attribute.second);
    }
    //glBindAttribLocation(mProgramId, ATTRIB_VERTEX, "position");
    //glBindAttribLocation(mProgramId, ATTRIB_COLOR, "color");
}

bool ShaderProgram::SetError(uint64_t id, error_t &result, unsigned statusType, bool compile)
{
    GLint logLength = 0;
    auto fnGetInfoInt = compile ? glGetShaderiv : glGetProgramiv;
    fnGetInfoInt((GLuint)id, GL_INFO_LOG_LENGTH, &logLength);
    result.first = 0;
    result.second.clear();
    if (logLength > 0) {
        GLchar *log = new GLchar [logLength];
        auto fnGetLog = compile ? glGetShaderInfoLog : glGetProgramInfoLog;
        fnGetLog((GLuint)id, logLength, &logLength, log);
        result.second.assign(log, logLength);
        delete [] log;
    }
    GLint status = 0;
    fnGetInfoInt((GLuint)id, statusType, &status);
    result.first = status;
    return status == 0; // there was an error
}

bool ShaderProgram::LoadShader(Shader::Type type, const char *filename)
{
    if (mShaders[type]) {
        delete mShaders[type];
    }
    if (!mProgramId) {
        std::cout << "Error: Program ID not set" << std::endl;
        return false;
    }
    Shader *file = new Shader(type);
    if (!file->Load(filename)) {
        std::cout << "Error: Can't load shader file '" << filename << "'" << std::endl;
        delete file;
        return false;
    }
    if (!file->Compile()) {
        std::cout << "Error: Failed to compile shader, error:" << std::endl << file->GetError().second << std::endl;
        delete file;
        return false;
    }
    file->AttachTo(mProgramId);
    mShaders[type] = file;
    return true;
}

bool ShaderProgram::Link()
{
    glLinkProgram(mProgramId);
    if (!SetError(mProgramId, mLinkError, GL_LINK_STATUS, false)) {
        glValidateProgram(mProgramId);
        return !SetError(mProgramId, mValidateError, GL_VALIDATE_STATUS, false);
    }
    return false;
}

/*
- (BOOL)loadShaders
{
    GLuint vertShader, fragShader;
    NSString *vertShaderPathname, *fragShaderPathname;

    // Create shader program.
    program = glCreateProgram();

    // Create and compile vertex shader.
    vertShaderPathname = [[NSBundle mainBundle] pathForResource:@"Shader" ofType:@"vsh"];
    if (![self compileShader:&vertShader type:GL_VERTEX_SHADER file:vertShaderPathname])
    {
        NSLog(@"Failed to compile vertex shader");
        return FALSE;
    }

    // Create and compile fragment shader.
    fragShaderPathname = [[NSBundle mainBundle] pathForResource:@"Shader" ofType:@"fsh"];
    if (![self compileShader:&fragShader type:GL_FRAGMENT_SHADER file:fragShaderPathname])
    {
        NSLog(@"Failed to compile fragment shader");
        return FALSE;
    }
    
    // Attach vertex shader to program.
    glAttachShader(program, vertShader);
    
    // Attach fragment shader to program.
    glAttachShader(program, fragShader);
    
    // Bind attribute locations.
    // This needs to be done prior to linking.
    glBindAttribLocation(program, ATTRIB_VERTEX, "position");
    glBindAttribLocation(program, ATTRIB_COLOR, "color");
    
    // Link program.
    if (![self linkProgram:program])
    {
        NSLog(@"Failed to link program: %d", program);

        if (vertShader)
        {
            glDeleteShader(vertShader);
            vertShader = 0;
        }
        if (fragShader)
        {
            glDeleteShader(fragShader);
            fragShader = 0;
        }
        if (program)
        {
            glDeleteProgram(program);
            program = 0;
        }

        return FALSE;
    }
    
    // Get uniform locations.
    uniforms[UNIFORM_TRANSLATE] = glGetUniformLocation(program, "translate");
    
    // Release vertex and fragment shaders.
    if (vertShader)
        glDeleteShader(vertShader);
    if (fragShader)
        glDeleteShader(fragShader);
    
    return TRUE;
}
*/
