attribute vec4 _position; // Attribute is data send with each vertex. Position
attribute vec2 _texuv0; // and texture coordinates is the example here

uniform mat4 u_view; // This is sent from within your program

varying mediump vec2 v_texuv0; // varying means it is passed to next shader

void main()
{
    // Multiply the view projection matrix with the vertex position
    gl_Position = u_view * _position;
    v_texuv0 = _texuv0; // Send texture coordinate data to fragment shader.
}
