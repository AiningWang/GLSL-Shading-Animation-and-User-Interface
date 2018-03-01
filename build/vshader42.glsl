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


attribute vec3 vPosition;
attribute vec3 vColor;
attribute vec2 vTexCoord;

varying   vec4  color;
varying   vec4  view_space;
varying   float fog_flag;
varying   float is_shadow_flag;
varying   vec2  texCoord;

varying   float oneCoord;
varying   vec2  twoCoord;
varying   vec2  disCoord;

uniform mat4 model_view;
uniform mat4 projection;
uniform float FogFlag;
uniform float ShadowBlendFlag;
uniform float isShadowFlag;

uniform int VerticalFlag;
uniform int SlantedFlag;
uniform int EyeSpaceFlag;
uniform int ObjectSpaceFlag;
uniform float tex_map_sphere_flag;

uniform int TiltedFlag;
uniform int UpRightFlag;
uniform int LatticeFlag;


void main() 
{
    vec4 vPosition4 = vec4(vPosition.x, vPosition.y, vPosition.z, 1.0);
    vec4 vColor4;
    
    is_shadow_flag = isShadowFlag;
    
    if (isShadowFlag == 1.0 && ShadowBlendFlag == 1.0)
        vColor4 = vec4(vColor.r, vColor.g, vColor.b, 0.65);
    else
        vColor4 = vec4(vColor.r, vColor.g, vColor.b, 1.0);

    
    gl_Position  = projection * model_view * vPosition4;
    
    color       = vColor4;
    view_space  = model_view * vPosition4;
    fog_flag    = FogFlag;
    
    texCoord    = vec2( vTexCoord.x*1.25,  vTexCoord.y*1.5);
    
    vec4 objectSpace = vPosition4;
    vec4 eyeSpace = model_view * vPosition4;
    
    
    if (tex_map_sphere_flag == 1.0)
        {
        if (VerticalFlag == 1 && EyeSpaceFlag == 1)
            oneCoord = 2.5 * eyeSpace.x;
        if (VerticalFlag == 1 && ObjectSpaceFlag == 1)
            oneCoord = 2.5 * objectSpace.x;
        if (SlantedFlag == 1 && EyeSpaceFlag == 1)
            oneCoord = 1.5 * (eyeSpace.x + eyeSpace.y + eyeSpace.z);
        if (SlantedFlag == 1 && ObjectSpaceFlag == 1)
            oneCoord = 1.5 * (objectSpace.x + objectSpace.y + objectSpace.z);
        }

    
    if (tex_map_sphere_flag == 2.0)
        {
        if (VerticalFlag == 1 && EyeSpaceFlag == 1)
            {
            twoCoord[0] = 0.5 * (eyeSpace.x + 1.0);
            twoCoord[1] = 0.5 * (eyeSpace.y + 1.0);
            }
        if (VerticalFlag == 1 && ObjectSpaceFlag == 1)
            {
            twoCoord[0] = 0.5 * (objectSpace.x + 1.0);
            twoCoord[1] = 0.5 * (objectSpace.y + 1.0);
            }
        if (SlantedFlag == 1 && EyeSpaceFlag == 1)
            {
            twoCoord[0] = 0.3 * (eyeSpace.x + eyeSpace.y + eyeSpace.z);
            twoCoord[1] = 0.3 * (eyeSpace.x - eyeSpace.y + eyeSpace.z);
            }
        if (SlantedFlag == 1 && ObjectSpaceFlag == 1)
            {
            twoCoord[0] = 0.3 * (objectSpace.x + objectSpace.y + objectSpace.z);
            twoCoord[1] = 0.3 * (objectSpace.x - objectSpace.y + objectSpace.z);
            }

        }
    
    
    if (LatticeFlag == 1 )
        {
        if (UpRightFlag == 1 && EyeSpaceFlag == 1)
            {
            disCoord[0] = 0.5 * (eyeSpace.x + 1.0);
            disCoord[1] = 0.5 * (eyeSpace.y + 1.0);
            }
        if (UpRightFlag == 1 && ObjectSpaceFlag == 1)
            {
            disCoord[0] = 0.5 * (objectSpace.x + 1.0);
            disCoord[1] = 0.5 * (objectSpace.y + 1.0);
            }
        if (TiltedFlag == 1 && EyeSpaceFlag == 1)
            {
            disCoord[0] = 0.3 * (eyeSpace.x + eyeSpace.y + eyeSpace.z);
            disCoord[1] = 0.3 * (eyeSpace.x - eyeSpace.y + eyeSpace.z);
            }
        if (TiltedFlag == 1 && ObjectSpaceFlag == 1)
            {
            disCoord[0] = 0.3 * (objectSpace.x + objectSpace.y + objectSpace.z);
            disCoord[1] = 0.3 * (objectSpace.x - objectSpace.y + objectSpace.z);
            }
        
        }

} 


