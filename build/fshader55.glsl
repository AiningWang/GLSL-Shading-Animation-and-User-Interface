/*****************************
 * File: fshader42.glsl
 *       A simple fragment shader
 *****************************/

// #version 150  // YJC: Comment/un-comment this line to resolve compilation errors
                 //      due to different settings of the default GLSL version

/**
 * For Mac OS X, GLSL version is by default 120 (OpenGL 2.1 with compatibility profile).
 * We need to use legacy synatx in GLSL 120:
 * 1. Replace 'out' with 'varying' for variables sent from vertex shader.
 * 2. Use built-in variable 'gl_FragColor' for the final output color ('out vec4 fColor' is not needed).
 */


varying vec4 color;
varying float y;



void main() 
{

    if (y < 0.1)    discard;
    else            gl_FragColor = color;
    gl_FragColor = color;

} 

