/*--------------
Vertex Shader: filename "vTexture.glsl"
---------------*/
// #version 150    // YJC: Comment/un-comment this line to resolve compilation errors
                   //      due to different settings of the default GLSL version

/**
 * For Mac OS X, GLSL version is by default 120 (OpenGL 2.1 with compatibility profile).
 * We need to use legacy synatx in GLSL 120:
 * 1. Replace keyword 'in' with 'attributes' for the input vertex attributes.
 * 2. Replace keyword 'out' with 'varying' for variables sent to fragment shader.
 */

/* GLSL 120 */
attribute vec3 vPosition;
attribute vec2 vTexCoord;

/* GLSL 130+ */
// in vec3 vPosition;
// in vec2 vTexCoord;

uniform mat4 ModelView;
uniform mat4 Projection;

uniform vec4 uColor; // obj color (as a uniform variable)

/* GLSL 120 */
varying vec4 color;
varying vec2 texCoord;

/* GLSL 130+ */
// out vec4 color;
// out vec2 texCoord;

void main() 
{
    
    vec4 vPosition4 = vec4(vPosition.x, vPosition.y, vPosition.z, 1.0);
    gl_Position = Projection * ModelView * vPosition4;

    color = uColor;
    texCoord = vTexCoord;
} 
