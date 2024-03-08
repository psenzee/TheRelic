precision mediump float; // Mandatory OpenGL ES float precision

varying vec2 v_texuv0; // Sent in from Vertex Shader

uniform sampler2D _texture0; // The texture to use sent from within your program.

void main()
{
    // The pixel colors are set to the texture according to texture coordinates.
    gl_FragColor = texture2D(_texture0, v_texuv0);
}
