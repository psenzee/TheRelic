#include <iostream>
#include <thread>
#include <cstdio>
#include <unordered_map>
#include <span>

#include "gl_utils.h"
#include "Shaders.h"
#include "IndexedMeshGL.h"
#include "ShaderProgram.h"
#include "TextureGL.h"

#include "std_array_mat.h"
#include "math_typedefs.h"

// TODO index/reindex

struct UiBase
{
    typedef std::array<float, 2>  size2f_t;
    typedef std::array<float, 16> mat4_t;
    
    IndexedMeshGL  *_mesh;
    ShaderProgram  *_program;
    TextureGL      *_base_texture;
    size2f_t        _window_size;
    mat4_t          _view,
                    _view_inv,
                    _view_to_pixels,
                    _view_from_pixels;
    int             _view_location_id;
    
    UiBase(size_t w, size_t h) : _mesh(nullptr), _program(nullptr), _base_texture(nullptr)
    {
        set_window_size(w, h);
        initialize();
    }
    
    ~UiBase()
    {
        delete _mesh;
        delete _program;
        delete _base_texture;
        _mesh = nullptr;
        _program = nullptr;
        _base_texture = nullptr;
    }
    
    void set_window_size(size_t w, size_t h)
    {
        float wf = float(w), hf = float(h);
        
        // set up matrices
        _window_size = size2f_t { wf, hf };
        float ratio = wf / hf;
        _view = mat4_t { 0.f };
        set_identity(_view);
        _view_inv = _view_to_pixels = _view_from_pixels = _view;
        set_scale(_view, 1.f, ratio, 1.f);
        set_scale(_view_to_pixels, wf, ratio * wf, 1.f);
        set_scale(_view_inv, 1.f, 1.f / ratio, 1.f);
        set_scale(_view_from_pixels, 1.f / w, 1.f / (ratio * w), 1.f);
    }
    
    void initialize()
    {
        const char *file = "/Users/paul/repo_p/TheRelic/worldgl/file_1011390.png";
        _base_texture = new TextureGL(file);

        const GLfloat vertices [] = {
             1.f, -1.f, 0.0f,  1.f, 0.f,
             1.f,  1.f, 0.0f,  1.f, 1.f,
            -1.f, -1.f, 0.0f,  0.f, 0.f,
            -1.f,  1.f, 0.0f,  0.f, 1.f,
        };

        const uint16_t indices [] = {
            0, 1, 2, 2, 1, 3
        };

        _mesh = new IndexedMeshGL(
            "IL P3 T2",
            (const float *)vertices,   sizeof(vertices) / sizeof(vertices[0]),
            (const uint16_t *)indices, sizeof(indices) / sizeof(indices[0])
        );

        _program = new ShaderProgram(UI_VERTEX_SHADER, UI_FRAGMENT_SHADER);
        _view_location_id = _GL(glGetUniformLocation(_program->_program_id, "um_view")); // assuming that this id is constant, otherwise move it to draw
    }
    
    void draw()
    {
        _mesh->enable(_program->_program_id);
        _base_texture->SetSampler(_program->_program_id, 0);
        _GL(glUniformMatrix4fv(_view_location_id, 1, false, _view.data()));
        _GL(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0));
    }
};

/*
size_t IndexedAAQuad(const Vector3 &vb, const Vector3 &ve, const Vector2 &tb, const Vector2 &te)
{
    // degenerate -- it's a point
    if (math::zero(vb.manhattan(ve))) {
        return 0;
    }
    bool eq_x = math::eq(vb.x, ve.x),
         eq_y = math::eq(vb.y, ve.y),
         eq_z = math::eq(vb.z, ve.z);
    // degenerate -- it's a line
    if ((eq_x && eq_y) || (eq_y && eq_z) || (eq_x && eq_z)) {
        return 0;
    } else if (eq_z || eq_y) { // I only know that eq_z has the correct normals.  I will have to test the rest
        const float vertices [] = {
            ve.x, vb.y, vb.z, te.x, tb.y,
            ve.x, ve.y, ve.z, te.x, te.y,
            vb.x, vb.y, vb.z, tb.x, tb.y,
            vb.x, ve.y, ve.z, tb.x, te.y,
        };
    } else if (eq_x) {
        const float vertices [] = {
            ve.x, ve.y, vb.z, te.x, tb.y,
            vb.x, ve.y, ve.z, te.x, te.y,
            ve.x, vb.y, vb.z, tb.x, tb.y,
            vb.x, vb.y, ve.z, tb.x, te.y,
        };
    }
    const uint16_t indices [] = {
        0, 1, 2, 2, 1, 3
    };
    return 6;
}
*/
void test_matrix()
{
    mat4_t m;
    set_identity(m);
}

int main(void)
{
    GLFWwindow *window;
    
    //mat_perspective_fov(float fov, float aspect, float znear, float zfar, bool left = true)
    //mat_print

    // Initialize the library
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    
    size_t w = 1024, h = 512;

    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(int(w), int(h), "OpenGL", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    
    // Make the window's context current
    glfwMakeContextCurrent(window);

    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        std::cerr << "Error initializing GLEW: " << glewGetErrorString(err) << std::endl;
        glfwTerminate();
        return -1;
    }
    
    UiBase ui_base(w, h);
    
    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window)) {

        // Render here
        _GL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
        //_GL(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0));
        ui_base.draw();

        // Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

