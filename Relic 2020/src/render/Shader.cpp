#include "Shader.h"
#include "core/file.h"
#include "platform/GLIncludes.h"
#include "ShaderProgram.h"
#include "GLUtils.h"

Shader::~Shader()
{
    if (mShaderId) {
        glDeleteShader(mShaderId);
    }
    mShaderId = 0;
}

bool Shader::Load(const char *file)
{
    file::buffer_t data = file::read_file(file);
    if (!data.size) {
        return false;
    }
    SetText((const char *)data.data, data.size);
    return true;
}

bool Shader::Compile()
{
    if (mText.empty()) {
        return false;
    }
    GLuint type = mType == TYPE_FRAGMENT ? GL_FRAGMENT_SHADER : GL_VERTEX_SHADER;
    mShaderId = _GL(glCreateShader(type));
    const char *text = mText.data();
    glShaderSource(mShaderId, 1, &text, NULL);
    glCompileShader(mShaderId);
    if (ShaderProgram::SetError(mShaderId, mCompileError, GL_COMPILE_STATUS, true)) {
        glDeleteShader(mShaderId);
        mShaderId = 0;
        return false;
    }
    return true;
}

void Shader::AttachTo(uint64_t programId)
{
    glAttachShader(programId, mShaderId);
}
