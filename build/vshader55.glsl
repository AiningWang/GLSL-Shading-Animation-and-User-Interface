/***************************
 * File: vshader42.glsl:
 *   A simple vertex shader.
 *
 * - Vertex attributes (positions & colors) for all vertices are sent
 *   to the GPU via a vertex buffer object created in the OpenGL program.
 *
 * - This vertex shader uses the Model-View and Projection matrices passed
 *   on from the OpenGL program as uniform variables of type mat4.
 ***************************/

// #version 150  // YJC: Comment/un-comment this line to resolve compilation errors
                 //      due to different settings of the default GLSL version

/**
 * For Mac OS X, GLSL version is by default 120 (OpenGL 2.1 with compatibility profile).
 * We need to use legacy synatx in GLSL 120:
 * 1. Replace keyword 'in' with 'attributes' for the input vertex attributes.
 * 2. Replace keyword 'out' with 'varying' for variables sent to fragment shader.
 */

/* GLSL 120 */
attribute vec3 vPosition;
attribute vec3 vColor;


varying   vec4  color;
varying   float y;

uniform float Time;
uniform mat4 ModelView3;
uniform mat4 Projection3;

uniform float ColorR;
uniform float ColorG;
uniform float ColorB;
uniform float VelocityX;
uniform float VelocityY;
uniform float VelocityZ;


void main() 
{
    
    float a = -0.00000049;
    float x = 0.001 * VelocityX * Time;
    float z = 0.001 * VelocityZ * Time;
    y = 0.1 + 0.001 * VelocityY * Time + 0.5*a*Time*Time;

    
    vec4 vPosition4 = vec4(x, y, z, 1.0);
    vec4 vColor4    = vec4( ColorR, ColorG, ColorB, 1.0);

    gl_Position = Projection3 * ModelView3 * vPosition4;
    
    color  = vColor4;
} 
