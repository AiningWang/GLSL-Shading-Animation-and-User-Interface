
/*
 File Name: "vshader53.glsl":
 Vertex shader:
 - Per vertex shading for a single point light source;
 distance attenuation is Yet To Be Completed.
 - Entire shading computation is done in the Eye Frame.
 */

/* GLSL 120 */
attribute vec4 vPosition;
attribute vec3 vNormal;
attribute vec2 vTexCoord;

varying   vec4  color;
varying   vec4  view_space;
varying   vec2  texCoord;
varying   float fog_flag;
varying   float oneCoord;
varying   vec2  twoCoord;
varying   vec2  disCoord;

/*-----------------------tex para----------------------*/
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

/*-----------------------shader para----------------------*/
uniform vec4  AmbientProduct, DiffuseProduct, SpecularProduct;
uniform mat4  ModelView2;
uniform mat4  Projection2;
uniform mat3  Normal_Matrix;
uniform vec4  LightPosition;   // Must be in Eye Frame
uniform float Shininess;

uniform float ConstAtt;  // Constant Attenuation
uniform float LinearAtt; // Linear Attenuation
uniform float QuadAtt;   // Quadratic Attenuation

uniform float SpotFlag;
uniform float PointFlag;
uniform float spot_exp;
uniform float spot_ang;

uniform vec4  spot_direction;
uniform vec4  MaterialAmbient;
uniform vec4  MaterialDiffuse;
uniform vec4  MaterialSpecular;
uniform vec4  LightAmbient;
uniform vec4  LightDiffuse;
uniform vec4  LightSpecular;

void main()
{
    /*-----------------texture-------------------*/
    view_space = ModelView2 * vPosition;
    fog_flag   = FogFlag;
    
    vec4 objectSpace = vPosition;
    vec4 eyeSpace = ModelView2 * vPosition;
    
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
    
    
    /*------------------shader------------------*/
    vec3 pos                = (ModelView2 * vPosition).xyz;
    vec3 N                  = normalize(Normal_Matrix * vNormal);
    vec4 light_color        = vec4( 1.0, 1.0, 1.0, 1   );
    vec3 light_direction    = vec3( 0.1, 0.0, -1.0     );

    
    float Shininess         =   125.0;
    
    
    vec3 L_ini = normalize (-light_direction);
    vec3 E_ini = normalize (-pos);
    vec3 H_ini = normalize ( L_ini + E_ini);
    
    vec4  ini_ambient = AmbientProduct;
    
    float ini_d = max( dot(L_ini, N), 0.0 );
    vec4  ini_diffuse = ini_d * DiffuseProduct;
    
    float ini_s = pow( max(dot(N, H_ini), 0.0), Shininess );
    vec4  ini_specular = ini_s * SpecularProduct;
    if( dot(L_ini, N) < 0.0 ) {
        ini_specular = vec4(0.0, 0.0, 0.0, 1.0);
    }
    
    vec4 ini_color = 1.0 * (ini_ambient + ini_diffuse + ini_specular);
    
    
        // Transform vertex  position into eye coordinates
    
    
    vec3 L = normalize( LightPosition.xyz - pos );
    vec3 E = normalize( -pos );
    vec3 H = normalize( L + E );
    
    /*--- To Do: Compute attenuation ---*/
    float attenuation = 1.0;
    vec3  tmp_cal_d = LightPosition.xyz - pos;
    float cur_distance=length(tmp_cal_d);
    attenuation = 1.0 / float(ConstAtt + LinearAtt * cur_distance + QuadAtt * cur_distance * cur_distance);
    
    // Compute terms in the illumination equation
    vec4  ambient = LightAmbient * MaterialAmbient;
    
    float d = max( dot(L, N), 0.0 );
    vec4  diffuse = d * LightDiffuse * MaterialDiffuse;
    
    float s = pow( max(dot(N, H), 0.0), Shininess );
    vec4  specular = s * LightSpecular * MaterialSpecular;
    
    if( dot(L, N) < 0.0 )
        specular = vec4(0.0, 0.0, 0.0, 1.0);

    gl_Position = Projection2 * ModelView2 * vPosition;
    
    /*--- attenuation below must be computed properly ---*/
    vec4 color_add_point = attenuation * (ambient + diffuse + specular);
    
    vec3 spot_l     = -L;
    vec3 spot_l_f   = normalize( spot_direction.xyz - LightPosition.xyz );
    
    float spot_att  = pow(dot(spot_l, spot_l_f), spot_exp);
    
    vec4 color_add_spot = spot_att * color_add_point;
    
    if ( dot(spot_l,spot_l_f) < spot_ang)
        color_add_spot  = vec4(0,0,0,1);
    
    
    if (SpotFlag == 1.0)
        color = ini_color + light_color * MaterialAmbient + color_add_spot;
    else
        color = ini_color + light_color * MaterialAmbient + color_add_point;
    
    
    
    if (isShadowFlag == 1.0 && ShadowBlendFlag == 1.0)
        color[3] = 0.65;
    
    texCoord    = vec2( vTexCoord.x*1.25,  vTexCoord.y*1.5);
}
