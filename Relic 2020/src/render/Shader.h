#pragma once

#include "core/core.h"
#include <string>
#include <utility>

class Shader
{
public:
    typedef std::pair<uint64_t, std::string> error_t;

    enum Type { TYPE_VERTEX = 0, TYPE_FRAGMENT, MAX_SHADER_TYPES };

    inline Shader(Type type) : mShaderId(0), mType(type) {}
    ~Shader();

    inline Shader        &SetText(const char *data)              { mText = data; return *this; }
    inline Shader        &SetText(const char *data, size_t size) { mText.assign(data, size); return *this; }
    inline Shader        &SetText(const std::string &data)       { mText = data; return *this; }

    bool                  Load(const char *file);
    bool                  Compile();
    void                  AttachTo(uint64_t programId);

    inline const error_t &GetError() const                       { return mCompileError; }

private:

    uint64_t    mShaderId;
    std::string mText;
    error_t     mCompileError;
    Type        mType;
};
