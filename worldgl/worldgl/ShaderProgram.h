#pragma once

struct ShaderProgram
{
    unsigned _program_id,
             _fragment_shader_id,
             _vertex_shader_id;
    
    ShaderProgram(const char *vertex_source, const char *fragment_source);
    ~ShaderProgram();

private:
    
    bool load_vertex_shader(const char *source);
    bool load_fragment_shader(const char *source);
    void print_log(unsigned shader_id);
};
