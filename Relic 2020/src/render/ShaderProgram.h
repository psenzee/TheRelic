#pragma once

#include "core/core.h"
#include <string>
#include <utility>
#include <span>
#include <string_view>
#include "Shader.h"

struct ShaderProgram
{
    typedef std::pair<uint64_t, std::string>  error_t;
    typedef std::pair<uint64_t, const char *> attribute_t;

    ShaderProgram();
    
    inline unsigned GetProgramId() const { return mProgramId; }
    
    void SetActive();

    bool LoadShader(Shader::Type type, const char *filename);
    bool Link();

    void BindAttributes(std::span<const attribute_t> attributes) const;

    void SetUniform(std::string_view name, const Matrix4f &m);

    void SetUniform(std::string_view name, float v)           { return SetUniform(name, 1, &v); }
    void SetUniform(std::string_view name, const Vector2 &v)  { return SetUniform(name, 2, v.data); }
    void SetUniform(std::string_view name, const Vector3 &v)  { return SetUniform(name, 3, v.data); }

    static bool SetError(uint64_t id, error_t &result, unsigned statusType, bool compile);

private:
    
    void SetUniform(std::string_view name, size_t count, const float *v);

    unsigned    mProgramId;
    Shader     *mShaders[Shader::MAX_SHADER_TYPES];
    error_t     mLinkError,
                mValidateError;
};
