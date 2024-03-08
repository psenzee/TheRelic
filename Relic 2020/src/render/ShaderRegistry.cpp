#include "ShaderRegistry.h"
#include "Shader.h"
#include "platform/GLIncludes.h"
#include <iostream>

extern "C" const char *globalTranslatePath(const char *name);

ShaderProgram *ShaderRegistry::GetShaderProgram(const char *name)
{
    std::string namestr(name);
    if (!mRegistry.contains(namestr)) {
        ShaderProgram *program = new ShaderProgram();
        char path[1024] = "";
        snprintf(path, sizeof(path) - 1, "%s.vsh", globalTranslatePath(name));
        program->LoadShader(Shader::TYPE_VERTEX, path);
        snprintf(path, sizeof(path) - 1, "%s.fsh", globalTranslatePath(name));
        program->LoadShader(Shader::TYPE_FRAGMENT, path);
        // todo attributes..
        mRegistry[namestr] = program;
    }
    return mRegistry[namestr];
}
