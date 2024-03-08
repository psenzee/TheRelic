#pragma once

#include "core/core.h"
#include <string>
#include <utility>
#include <span>
#include <map>
#include "ShaderProgram.h"

struct ShaderRegistry
{
    ShaderProgram *GetShaderProgram(const char *name);
private:
    std::map<std::string, ShaderProgram *> mRegistry;
};
