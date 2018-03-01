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


varying   vec4  color;
varying   vec4  view_space;
varying   float fog_flag;
varying   float is_shadow_flag;
varying   vec2  texCoord;

varying   float  oneCoord;
varying   vec2   twoCoord;
varying   vec2   disCoord;

uniform   sampler2D texture_2D;
uniform   sampler1D texture_1D;

uniform   float is_floor_flag;
uniform   float tex_map_ground_flag;
uniform   float is_sphere_flag;
uniform   float is_solid_flag;
uniform   float tex_map_sphere_flag;
uniform   int    LatticeFlag;

void main() 
{
    vec4  fog_color     = vec4(0.7, 0.7, 0.7, 0.5);
    float fog_start     = 0.0;
    float fog_end       = 18.0;
    float fog_density   = 0.09;
    float fog_factor;
    float z             = abs(view_space.z);
    
    
/*------------------------ texture part -------------------------*/
    
    vec4 source_color = color;
    
    if (is_floor_flag == 1.0 && tex_map_ground_flag == 1.0)
        source_color = color * texture2D( texture_2D, texCoord );
    
    if (is_solid_flag == 1.0 && is_sphere_flag == 1.0 && tex_map_sphere_flag != 0.0)
        {
        if (tex_map_sphere_flag == 1.0) // contour
            source_color = color * texture1D( texture_1D, oneCoord );
        if (tex_map_sphere_flag == 2.0) // contour
            {
            vec4 k = texture2D( texture_2D, twoCoord );
            if (k[0] == 0.0)
                k = vec4(0.9, 0.1, 0.1, 1.0);
            source_color = color * k;
            }
        }
    
    if ((LatticeFlag == 1 && is_sphere_flag == 1.0) || (LatticeFlag == 1 && is_shadow_flag == 1.0))
        {
        if (fract(4.0*disCoord[0]) < 0.35 && fract(4.0*disCoord[1]) < 0.35 )
            discard;
        }

        
/*-------------------------- fog part ---------------------------*/
    
    if (fog_flag == 0.0)
        gl_FragColor = source_color;
    
    else if (fog_flag == 1.0)
    {
        fog_factor   = (fog_end - z)/(fog_end - fog_start);
        fog_factor   = clamp(fog_factor, 0.0, 1.0);
        gl_FragColor = mix(fog_color, source_color, fog_factor);
    }
    else if (fog_flag == 2.0)
    {
        fog_factor   = 1.0 / exp(z * fog_density);
        fog_factor   = clamp(fog_factor, 0.0, 1.0);
        gl_FragColor = mix(fog_color, source_color, fog_factor);
    }
    else if (fog_flag == 3.0)
    {
        fog_factor   = 1.0 / exp( (z*fog_density)*(z*fog_density) );
        fog_factor   = clamp(fog_factor, 0.0, 1.0);
        gl_FragColor = mix(fog_color, source_color, fog_factor);
    }
    
    
} 

