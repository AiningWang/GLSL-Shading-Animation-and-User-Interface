#include "Angel-yjc.h"
#pragma comment(lib,"glew32.lib")
#include "math.h"
#include <iostream>
#include <fstream>

using namespace std;

typedef Angel::vec3  color3;
typedef Angel::vec3  point3;
typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

#define PI 3.14159


GLuint program;         /* shader program object id */
GLuint program2;
GLuint program3;


GLuint x_buffer;        /* vertex buffer object id for x axis */
GLuint y_buffer;        /* vertex buffer object id for y axis */
GLuint z_buffer;        /* vertex buffer object id for z axis */
GLuint sphere_buffer;   /* vertex buffer object id for sphere */
GLuint floor_buffer;    /* vertex buffer object id for floor */

GLuint shadow_buffer;   /* vertex buffer object id for shadow */
GLuint smooth_buffer;
GLuint flat_buffer;
GLuint floor_lighting_buffer;    /* vertex buffer object id for lighting floor */
GLuint firework_buffer;


/* --------- Projection transformation parameters ---------*/

GLfloat     fovy = 45.0;   // Field-of-view in Y direction angle (in degrees)
GLfloat     aspect;        // Viewport aspect ratio
GLfloat     zNear = 0.5, zFar = 35.0;
GLfloat     angle = 0.0;    // rotation angle in degrees
vec4        init_eye( 7.0, 3.0, -10.0, 1.0);// initial viewer position
vec4        eye = init_eye;    // current viewer position
vec4        A(  3, 1,  5, 1);
vec4        B( -1, 1, -4, 1);
vec4        C(3.5, 1,-2.5,1);
vec4        OY( 0, 1,  0, 1);

int animationFlag = 0;      // 1: animation; 0: non-animation. Toggled by key 'a' or 'A'
int sphereFlag = 0;         // 1: solid sphere; 0: wireframe sphere. Toggled by key 's' or 'S'
int floorFlag = 1;          // 1: solid floor; 0: wireframe floor. Toggled by key 'f' or 'F'
int beginFlag = 0;          // 1: begin used; 0: begin unused

int triangle_num = 0;
float speed = 2.0;

const int sphere_NumVertices = 1500*3;
#if 0
point3 sphere_points[sphere_NumVertices]; // positions for all vertices
color3 sphere_colors[sphere_NumVertices]; // colors for all vertices
#endif
#if 1
point3 sphere_points[5000];
color3 sphere_colors[5000];
#endif

const int shadow_NumVertices = 1500*3;
#if 0
point3 shadow_points[shadow_NumVertices]; // positions for all vertices
color3 shadow_colors[shadow_NumVertices]; // colors for all vertices
#endif
#if 1
point3 shadow_points[5000];
color3 shadow_colors[5000];
#endif

point4 shade_points[5000];
vec3   flat_shade[5000];
vec3   smooth_shade[5000];



const int floor_NumVertices = 6;        //(1 face)*(2 triangles/face)*(3 vertices/triangle)
point3 floor_points[floor_NumVertices]; // positions for all vertices
color3 floor_colors[floor_NumVertices]; // colors for all vertices

point4 floor_lighting_points[floor_NumVertices]; // positions for all vertices
vec3   floor_lighting_colors[floor_NumVertices]; // colors for all vertices

point3 floor_vertices[4] = {
    point3(  5, 0,  8),
    point3(  5, 0, -4),
    point3( -5, 0, -4),
    point3( -5, 0,  8)
};

point4 floor_lighting_vertices[4] = {
    point4(  5., 0.,  8., 1.),
    point4(  5., 0., -4., 1.),
    point4( -5., 0., -4., 1.),
    point4( -5., 0.,  8., 1.)
};


// RGBA colors
color3 vertex_colors[9] = {
    color3( 0.0, 0.0, 0.0),  // black
    color3( 1.0, 0.0, 0.0),  // red
    color3( 1.0, 1.0, 0.0),  // yellow
    color3( 0.0, 1.0, 0.0),  // green
    color3( 0.0, 0.0, 1.0),  // blue
    color3( 1.0, 0.0, 1.0),  // magenta
    color3( 1.0, 1.0, 1.0),  // white
    color3( 0.0, 1.0, 1.0),  // cyan
    color3( 1.0, 0.84,  0)   // golden yellow
    
};


const int axis_NumVertices = 3; //(3 vertices/triangle)
point3 x_points[3] = {point3(0,0,0), point3(5,0,0), point3(10,0,0)};
color3 x_colors[3] = {vertex_colors[1], vertex_colors[1], vertex_colors[1]};
point3 y_points[3] = {point3(0,0,0), point3(0,5,0), point3(0,10,0)};
color3 y_colors[3] = {vertex_colors[5], vertex_colors[5], vertex_colors[5]};
point3 z_points[3] = {point3(0,0,0), point3(0,0,5), point3(0,0,10)};
color3 z_colors[3] = {vertex_colors[4], vertex_colors[4], vertex_colors[4]};


/*------- Shader Lighting Parameters -------*/

point4 light_position   (-14.0, 12.0, -3.0, 1.0);
color3 shadow_color     (0.25, 0.25, 0.25);

color4 sphere_material_ambient  (0.2, 0.2,  0.2, 1.0 );
color4 sphere_material_diffuse  (1.0, 0.84, 0.0, 1.0 );
color4 sphere_material_specular (1.0, 0.84, 0.0, 1.0 );

color4 floor_material_ambient   (0.2, 0.2, 0.2, 1.0 );
color4 floor_material_diffuse   (0.0, 1.0, 0.0, 1.0 );
color4 floor_material_specular  (0.0, 0.0, 0.0, 1.0 );

color4 light_color      (1.0, 1.0, 1.0, 1.0);
color4 light_ambient1   (0.0, 0.0, 0.0, 1.0);
color4 light_diffuse1   (0.8, 0.8, 0.8, 1.0);
color4 light_specular1  (0.2, 0.2, 0.2, 1.0);
vec4   light_direction  (0.1, 0.0, -1.0, 0.0);

color4 light_ambient2   (0.0, 0.0, 0.0, 1.0);
color4 light_diffuse2   (1.0, 1.0, 1.0, 1.0);
color4 light_specular2  (1.0, 1.0, 1.0, 1.0);


color4 sphere_ambient_product  = light_ambient1 * sphere_material_ambient;
color4 sphere_diffuse_product  = light_diffuse1 * sphere_material_diffuse;
color4 sphere_specular_product = light_specular1 * sphere_material_specular;
color4 floor_ambient_product  = light_ambient1 * floor_material_ambient;
color4 floor_diffuse_product  = light_diffuse1 * floor_material_diffuse;
color4 floor_specular_product = light_specular1 * floor_material_specular;

float const_att         = 2.0;
float linear_att        = 0.01;
float quad_att          = 0.001;
float material_shininess= 125.0;

int shadowFlag          = 1;         // 1: enable shadow; 0: no shadow
int lightingFlag        = 0;
int flatshadingFlag     = 0;
int smoothshadingFlag   = 0;
int pointlightFlag      = 0;
int spotlightFlag       = 0;



/*------- Texture Parameters -------*/

#define ImageWidth  64
#define ImageHeight 64
GLubyte checkImage[ImageHeight][ImageWidth][4];

#define stripeImageWidth 32
GLubyte stripeImage[4 * stripeImageWidth];

static GLuint TexName;

int isShadowFlag        = 0;
int isFloorFlag         = 0;
int isSphereFlag        = 0;

int fogFlag             = 0;
int blendingShadowFlag  = 0;
int texGroundFlag       = 0;
int texSphereFlag       = 0; // 0: no    1: contour   2: checker

int verticalFlag        = 1;
int slantedFlag         = 0;
int eyeSpaceFlag        = 0;
int objectSpaceFlag     = 1;

int tiltedFlag          = 0;
int upRightFlag         = 1;
int latticeFlag         = 0;

int fireworkFlag        = 0;

float T_sub = 0.0;

const int firework_NumVertices = 300;
point3 firework_points[firework_NumVertices]; // positions for all vertices
color3 firework_colors[firework_NumVertices]; // colors for all vertices

GLuint  model_view;  // model-view matrix uniform shader variable location
GLuint  projection;  // projection matrix uniform shader variable location

GLuint  model_view2;  // model-view matrix uniform shader variable location
GLuint  projection2;  // projection matrix uniform shader variable location

GLuint  model_view3;  // model-view matrix uniform shader variable location
GLuint  projection3;  // projection matrix uniform shader variable location


//----------------------------------------------------------------------------

void firework_set_up(void)
{
    int i;
    for (i = 0; i < firework_NumVertices; i++)
        {
        firework_colors[i] = vec3( rand()%256/256.0, rand()%256/256.0, rand()%256/256.0 );
        firework_points[i] = vec3( 2.0*(rand()%256/256.0 - 0.5),
                                     1.2*2.0*(rand()%256/256.0),
                                     2.0*(rand()%256/256.0 - 0.5) );
        }
}



//----------------------------------------------------------------------------
void image_set_up(void)
{
    int i, j, c;
    
    /* --- Generate checkerboard image to the image array ---*/
    for (i = 0; i < ImageHeight; i++)
        for (j = 0; j < ImageWidth; j++)
        {
            c = (((i & 0x8) == 0) ^ ((j & 0x8) == 0));
    
            if (c == 1) /* white */
            {
                c = 255;
                checkImage[i][j][0] = (GLubyte) c;
                checkImage[i][j][1] = (GLubyte) c;
                checkImage[i][j][2] = (GLubyte) c;
            }
            else  /* green */
            {
                checkImage[i][j][0] = (GLubyte) 0;
                checkImage[i][j][1] = (GLubyte) 150;
                checkImage[i][j][2] = (GLubyte) 0;
            }
            checkImage[i][j][3] = (GLubyte) 255;
        }
    
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
    /*--- Generate 1D stripe image to array stripeImage[] ---*/
    for (j = 0; j < stripeImageWidth; j++) {
        /* When j <= 4, the color is (255, 0, 0),   i.e., red stripe/line.
         When j > 4,  the color is (255, 255, 0),  i.e., yellow remaining texture
         */
        stripeImage[4*j]   = (GLubyte) 255;
        stripeImage[4*j+1] = (GLubyte) ((j>4) ? 255 : 0);
        stripeImage[4*j+2] = (GLubyte) 0;
        stripeImage[4*j+3] = (GLubyte) 255;
    }
    
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    /*----------- End 1D stripe image ----------------*/
    
    /*--- texture mapping set-up is to be done in
     init() (set up texture objects),
     display() (activate the texture object to be used, etc.)
     and in shaders.
     ---*/
    
} /* end function */



void SetUp_Fog_Var1()
{
    glUniform1f( glGetUniformLocation(program, "FogFlag"),      fogFlag * 1.0 );

}

void SetUp_Fog_Var2()
{
    glUniform1f( glGetUniformLocation(program2, "FogFlag"),     fogFlag * 1.0);
}


void SetUp_Shadow_Var1()
{
    glUniform1f( glGetUniformLocation(program, "isShadowFlag"), isShadowFlag * 1.0 );
    glUniform1f( glGetUniformLocation(program, "ShadowBlendFlag"), blendingShadowFlag * 1.0 );
    
    glUniform1i( glGetUniformLocation(program, "TiltedFlag"),   tiltedFlag  );
    glUniform1i( glGetUniformLocation(program, "UpRightFlag"),  upRightFlag );
    glUniform1i( glGetUniformLocation(program, "LatticeFlag"),  latticeFlag );
}


void SetUp_Shadow_Var2()
{
    glUniform1f( glGetUniformLocation(program2, "isShadowFlag"), isShadowFlag * 1.0 );
    glUniform1f( glGetUniformLocation(program2, "ShadowBlendFlag"), blendingShadowFlag * 1.0 );
    
    glUniform1i( glGetUniformLocation(program, "TiltedFlag"),   tiltedFlag  );
    glUniform1i( glGetUniformLocation(program, "UpRightFlag"),  upRightFlag );
    glUniform1i( glGetUniformLocation(program, "LatticeFlag"),  latticeFlag );
}


void SetUp_Tex_Floor_Var1()
{
    glUniform1i( glGetUniformLocation(program, "texture_2D"), 0 );
    glUniform1f( glGetUniformLocation(program, "is_floor_flag"), isFloorFlag * 1.0 );
    glUniform1f( glGetUniformLocation(program, "tex_map_ground_flag"), texGroundFlag * 1.0 );
}


void SetUp_Tex_Floor_Var2()
{
    glUniform1i( glGetUniformLocation(program2, "texture_2D"), 0 );
    glUniform1f( glGetUniformLocation(program2, "is_floor_flag"), isFloorFlag * 1.0);
    glUniform1f( glGetUniformLocation(program2, "tex_map_ground_flag"), texGroundFlag * 1.0);
}


void SetUp_Tex_Sphere_Var1()
{
    glUniform1i( glGetUniformLocation(program, "texture_1D"), 1 );
    glUniform1i( glGetUniformLocation(program, "texture_2D"), 0 );
    glUniform1f( glGetUniformLocation(program, "is_solid_flag"),    sphereFlag * 1.0);
    glUniform1f( glGetUniformLocation(program, "is_sphere_flag"),   isSphereFlag * 1.0);
    glUniform1f( glGetUniformLocation(program, "tex_map_sphere_flag"), texSphereFlag * 1.0);
    glUniform1i( glGetUniformLocation(program, "VerticalFlag"),     verticalFlag );
    glUniform1i( glGetUniformLocation(program, "SlantedFlag"),      slantedFlag );
    glUniform1i( glGetUniformLocation(program, "EyeSpaceFlag"),     eyeSpaceFlag );
    glUniform1i( glGetUniformLocation(program, "ObjectSpaceFlag"),  objectSpaceFlag );
    
    glUniform1i( glGetUniformLocation(program, "TiltedFlag"),   tiltedFlag  );
    glUniform1i( glGetUniformLocation(program, "UpRightFlag"),  upRightFlag );
    glUniform1i( glGetUniformLocation(program, "LatticeFlag"),  latticeFlag );
}

void SetUp_Tex_Sphere_Var2()
{
    glUniform1i( glGetUniformLocation(program2, "texture_1D"), 1 );
    glUniform1i( glGetUniformLocation(program2, "texture_2D"), 0 );
    glUniform1f( glGetUniformLocation(program2, "is_solid_flag"),    sphereFlag * 1.0);
    glUniform1f( glGetUniformLocation(program2, "is_sphere_flag"),   isSphereFlag * 1.0);
    glUniform1f( glGetUniformLocation(program2, "tex_map_sphere_flag"), texSphereFlag * 1.0);
    glUniform1i( glGetUniformLocation(program2, "VerticalFlag"),     verticalFlag );
    glUniform1i( glGetUniformLocation(program2, "SlantedFlag"),      slantedFlag );
    glUniform1i( glGetUniformLocation(program2, "EyeSpaceFlag"),     eyeSpaceFlag );
    glUniform1i( glGetUniformLocation(program2, "ObjectSpaceFlag"),  objectSpaceFlag );
    
    glUniform1i( glGetUniformLocation(program2, "TiltedFlag"),   tiltedFlag  );
    glUniform1i( glGetUniformLocation(program2, "UpRightFlag"),  upRightFlag );
    glUniform1i( glGetUniformLocation(program2, "LatticeFlag"),  latticeFlag );
}



void SetUp_Point_Light_Var(mat4 mv)
{
    vec4 light_position_eye = mv * light_position;
    glUniform4fv(glGetUniformLocation(program2, "LightPosition"), 1, light_position_eye);
    glUniform1f (glGetUniformLocation(program2, "ConstAtt"),      const_att);
    glUniform1f (glGetUniformLocation(program2, "LinearAtt"),     linear_att);
    glUniform1f (glGetUniformLocation(program2, "QuadAtt"),       quad_att);
    glUniform4fv(glGetUniformLocation(program2, "LightAmbient"),  1, light_ambient2);
    glUniform4fv(glGetUniformLocation(program2, "LightDiffuse"),  1, light_diffuse2);
    glUniform4fv(glGetUniformLocation(program2, "LightSpecular"), 1, light_specular2);
    glUniform1f (glGetUniformLocation(program2, "PointFlag"),     pointlightFlag);
    glUniform1f (glGetUniformLocation(program2, "SpotFlag"),      0.0);
}



void SetUp_Spot_Light_Var(mat4 mv)
{
    float spot_exponent = 15.0;
    float spot_angle    = cos(20.0 * PI / 180);
    
    vec4   light_position_eye = mv * light_position;
    point4 spot_direction = mv * vec4(-6.0, 0.0, -4.5, 1.0);

    glUniform4fv(glGetUniformLocation(program2, "LightPosition"), 1, mv*light_position);
    glUniform1f (glGetUniformLocation(program2, "ConstAtt"),     const_att);
    glUniform1f (glGetUniformLocation(program2, "LinearAtt"),    linear_att);
    glUniform1f (glGetUniformLocation(program2, "QuadAtt"),      quad_att);
    glUniform4fv(glGetUniformLocation(program2, "LightAmbient"),  1, light_ambient2);
    glUniform4fv(glGetUniformLocation(program2, "LightDiffuse"),  1, light_diffuse2);
    glUniform4fv(glGetUniformLocation(program2, "LightSpecular"), 1, light_specular2);
    glUniform1f (glGetUniformLocation(program2, "PointFlag"),    0.0);
    glUniform1f (glGetUniformLocation(program2, "SpotFlag"),     spotlightFlag);
    
    glUniform4fv(glGetUniformLocation(program2, "spot_direction"), 1, spot_direction);
    glUniform1f (glGetUniformLocation(program2, "spot_exp"),      spot_exponent);
    glUniform1f (glGetUniformLocation(program2, "spot_ang"),      spot_angle);
}



void SetUp_Sphere_Material_Var()
{
    glUniform4fv(glGetUniformLocation(program2, "AmbientProduct"),  1, sphere_ambient_product );
    glUniform4fv(glGetUniformLocation(program2, "DiffuseProduct"),  1, sphere_diffuse_product );
    glUniform4fv(glGetUniformLocation(program2, "SpecularProduct"), 1, sphere_specular_product );
    glUniform4fv(glGetUniformLocation(program2, "MaterialAmbient"), 1, sphere_material_ambient);
    glUniform4fv(glGetUniformLocation(program2, "MaterialDiffuse"), 1, sphere_material_diffuse);
    glUniform4fv(glGetUniformLocation(program2, "MaterialSpecular"), 1, sphere_material_specular);
    glUniform1f (glGetUniformLocation(program2, "Shininess"), material_shininess);
}



void SetUp_Floor_Material_Var()
{
    glUniform4fv(glGetUniformLocation(program2, "AmbientProduct"),  1, floor_ambient_product );
    glUniform4fv(glGetUniformLocation(program2, "DiffuseProduct"),  1, floor_diffuse_product );
    glUniform4fv(glGetUniformLocation(program2, "SpecularProduct"), 1, floor_specular_product );
    glUniform4fv(glGetUniformLocation(program2, "MaterialAmbient"), 1, floor_material_ambient);
    glUniform4fv(glGetUniformLocation(program2, "MaterialDiffuse"), 1, floor_material_diffuse);
    glUniform4fv(glGetUniformLocation(program2, "MaterialSpecular"), 1, floor_material_specular);
    glUniform1f (glGetUniformLocation(program2, "Shininess"), 1.0);
}



//----------------------------------------------------------------------------
int Index = 0; // YJC: This must be a global variable since quad() is called
               //      multiple times and Index should then go up to 36 for
               //      the 36 vertices and colors



//ball(): assign color to the vertices
void colorBallShadow()
{
    while(Index < triangle_num * 3)
    {
        sphere_colors[Index] = vertex_colors[8];
        shadow_colors[Index] = shadow_color;
        Index ++;
    }
}


void getFlatData()
{
    for (int i = 0; i < triangle_num; i++)
    {
        vec4 t1(sphere_points[3*i][0]  , sphere_points[3*i][1]  , sphere_points[3*i][2]  , 1.0);
        vec4 t2(sphere_points[3*i+1][0], sphere_points[3*i+1][1], sphere_points[3*i+1][2], 1.0);
        vec4 t3(sphere_points[3*i+2][0], sphere_points[3*i+2][1], sphere_points[3*i+2][2], 1.0);
    
        vec3 t_normal       = normalize(cross(t2-t1, t3-t2));
        flat_shade[3*i]     = t_normal;
        flat_shade[3*i+1]   = t_normal;
        flat_shade[3*i+2]   = t_normal;
    }
}


void getSmoothData()
{
    for (int i = 0; i< triangle_num*3; i++)
    {
        vec3 p = vec3(shade_points[i][0], shade_points[i][1], shade_points[i][2]);
        smooth_shade[i] = normalize(p);
    }
}



//-------------------------------
// generate 2 triangles: 6 vertices and 6 colors
void floor()
{
    floor_colors[0] = vertex_colors[3]; floor_points[0] = floor_vertices[0];
    floor_colors[1] = vertex_colors[3]; floor_points[1] = floor_vertices[1];
    floor_colors[2] = vertex_colors[3]; floor_points[2] = floor_vertices[2];
  
    floor_colors[3] = vertex_colors[3]; floor_points[3] = floor_vertices[2];
    floor_colors[4] = vertex_colors[3]; floor_points[4] = floor_vertices[3];
    floor_colors[5] = vertex_colors[3]; floor_points[5] = floor_vertices[0];
}



void lighting_floor()
{
    vec4 u = floor_lighting_vertices[1] - floor_lighting_vertices[0];
    vec4 v = floor_lighting_vertices[2] - floor_lighting_vertices[1];
    vec3 normal = normalize(cross(u, v));
    
    floor_lighting_colors[0] = normal;
    floor_lighting_colors[1] = normal;
    floor_lighting_colors[2] = normal;
    
    floor_lighting_colors[3] = normal;
    floor_lighting_colors[4] = normal;
    floor_lighting_colors[5] = normal;
    
    floor_lighting_points[0] = floor_lighting_vertices[0];
    floor_lighting_points[1] = floor_lighting_vertices[1];
    floor_lighting_points[2] = floor_lighting_vertices[2];
    
    floor_lighting_points[3] = floor_lighting_vertices[2];
    floor_lighting_points[4] = floor_lighting_vertices[3];
    floor_lighting_points[5] = floor_lighting_vertices[0];
}



//----------------------------------------------------------------------------
// let user input filemame and load data
void loadData()
{
    FILE *fp;
    char filename[50];
    char buff[25];
    float triangle_para[3];
    int p;
    /* input filename */
    cout << "Please input filename:" << endl;
    cin  >> filename;
    fp = fopen(filename, "r");
    /* get number of triangles */
    fgets(buff, 255, fp);
    triangle_num = atoi(buff);
    /* get parameters of triangles */
    for (int i = 0; i < triangle_num; i++)
    {
        fscanf(fp, "%d\n", &p);
    
        fscanf(fp, "%f %f %f\n", &triangle_para[0], &triangle_para[1], &triangle_para[2]);
        sphere_points[3*i] = point3(triangle_para[0], triangle_para[1], triangle_para[2]);
        shadow_points[3*i] = point3(triangle_para[0], triangle_para[1], triangle_para[2]);
        shade_points[3*i]  = point4(triangle_para[0], triangle_para[1], triangle_para[2], 1);

        fscanf(fp, "%f %f %f\n", &triangle_para[0], &triangle_para[1], &triangle_para[2]);
        sphere_points[3*i+1] = point3(triangle_para[0], triangle_para[1], triangle_para[2]);
        shadow_points[3*i+1] = point3(triangle_para[0], triangle_para[1], triangle_para[2]);
        shade_points[3*i+1]  = point4(triangle_para[0], triangle_para[1], triangle_para[2], 1);
    
        fscanf(fp, "%f %f %f\n", &triangle_para[0], &triangle_para[1], &triangle_para[2]);
        sphere_points[3*i+2] = point3(triangle_para[0], triangle_para[1], triangle_para[2]);
        shadow_points[3*i+2] = point3(triangle_para[0], triangle_para[1], triangle_para[2]);
        shade_points[3*i+2]  = point4(triangle_para[0], triangle_para[1], triangle_para[2], 1);
    }
    
    fclose(fp);
}



//----------------------------------------------------------------------------
// OpenGL initialization
void init()
{
    image_set_up();
    loadData();
    colorBallShadow();
    getFlatData();
    getSmoothData();
    floor();
    lighting_floor();
    firework_set_up();
    
    /*--- Create and Initialize a texture object ---*/
    
    // checker tex
    glGenTextures( 1, &TexName );      // Generate texture obj name(s)
    
    glActiveTexture( GL_TEXTURE0 );  // Set the active texture unit to be 0
    glBindTexture(GL_TEXTURE_2D, TexName); // Bind the texture to this texture unit
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ImageWidth, ImageHeight,
                 0, GL_RGBA, GL_UNSIGNED_BYTE, checkImage);

    
    // strip tex
    glGenTextures( 1, &TexName );      // Generate texture obj name(s)
    
    glActiveTexture( GL_TEXTURE1 );  // Set the active texture unit to be 1
    glBindTexture(GL_TEXTURE_1D, TexName); // Bind the texture to this texture unit
    
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, 32,
                 0, GL_RGBA, GL_UNSIGNED_BYTE, stripeImage);
    
    
    // firework
    glGenBuffers(1, &firework_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, firework_buffer);
    glBufferData(GL_ARRAY_BUFFER,
                 firework_NumVertices * sizeof(point3) + firework_NumVertices * sizeof(color3),
                 NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, firework_NumVertices * sizeof(point3), firework_points);
    glBufferSubData(GL_ARRAY_BUFFER,
                    sizeof(point3) * firework_NumVertices,
                    sizeof(color3) * firework_NumVertices,
                    firework_colors);
    
    
// Create and initialize a vertex buffer object for sphere, to be used in display()
    glGenBuffers(1, &sphere_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, sphere_buffer);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(point3) * sphere_NumVertices + sizeof(color3) * sphere_NumVertices,
                 NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(point3) * sphere_NumVertices, sphere_points);
    glBufferSubData(GL_ARRAY_BUFFER,
                    sizeof(point3) * sphere_NumVertices,
                    sizeof(color3) * sphere_NumVertices,
                    sphere_colors);
    
    
// Create and initialize a vertex buffer object for shadow, to be used in display()
    glGenBuffers(1, &shadow_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, shadow_buffer);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(point3) * shadow_NumVertices + sizeof(color3) * shadow_NumVertices,
                 NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(point3) * shadow_NumVertices, shadow_points);
    glBufferSubData(GL_ARRAY_BUFFER,
                    sizeof(point3) * shadow_NumVertices,
                    sizeof(color3) * shadow_NumVertices,
                    shadow_colors);
    
 
// Create and initialize a vertex buffer object for flat, to be used in display()
    glGenBuffers(1, &flat_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, flat_buffer);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(point4) * sphere_NumVertices + sizeof(vec3 ) * sphere_NumVertices,
                 NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(point4) * sphere_NumVertices, shade_points);
    glBufferSubData(GL_ARRAY_BUFFER,
                    sizeof(point4) * sphere_NumVertices,
                    sizeof(vec3  ) * sphere_NumVertices,
                    flat_shade);
    
    
// Create and initialize a vertex buffer object for smooth, to be used in display()
    glGenBuffers(1, &smooth_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, smooth_buffer);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(point4) * sphere_NumVertices + sizeof(vec3  ) * sphere_NumVertices,
                 NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(point4) * sphere_NumVertices, shade_points);
    glBufferSubData(GL_ARRAY_BUFFER,
                    sizeof(point4) * sphere_NumVertices,
                    sizeof(vec3  ) * sphere_NumVertices,
                    smooth_shade);
    
    
    
// Create and initialize a vertex buffer object for floor, to be used in display()
    glGenBuffers(1, &floor_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, floor_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(floor_points) + sizeof(floor_colors),
                 NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(floor_points), floor_points);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(floor_points), sizeof(floor_colors), floor_colors);
    
    
// Create and initialize a vertex buffer object for lighting floor, to be used in display()
    glGenBuffers(1, &floor_lighting_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, floor_lighting_buffer);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(floor_lighting_points) + sizeof(floor_lighting_colors),
                 NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(floor_lighting_points), floor_lighting_points);
    glBufferSubData(GL_ARRAY_BUFFER,
                    sizeof(floor_lighting_points),
                    sizeof(floor_lighting_colors),
                    floor_lighting_colors);
 
    
// Create and initialize a vertex buffer object for x, to be used in display()
    glGenBuffers(1, &x_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, x_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(x_points) + sizeof(x_colors),
                 NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(x_points), x_points);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(x_points), sizeof(x_colors),x_colors);

    
// Create and initialize a vertex buffer object for y, to be used in display()
    glGenBuffers(1, &y_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, y_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(y_points) + sizeof(y_colors),
                 NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(y_points), y_points);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(y_points), sizeof(y_colors), y_colors);
 
    
// Create and initialize a vertex buffer object for z, to be used in display()
    glGenBuffers(1, &z_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, z_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(z_points) + sizeof(z_colors),
                 NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(z_points), z_points);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(z_points), sizeof(z_colors), z_colors);

    
 // Load shaders and create a shader program (to be used in display())
    glEnable( GL_DEPTH_TEST );
    glClearColor( 0.529, 0.807, 0.92, 1.0 );
    glLineWidth( 1.5 );
    
    program     = InitShader("vshader42.glsl", "fshader42.glsl");
    program2    = InitShader("vshader53.glsl", "fshader53.glsl");
    program3    = InitShader("vshader55.glsl", "fshader55.glsl");
}


//----------------------------------------------------------------------------
// drawObj(buffer, num_vertices):
//   draw the object that is associated with the vertex buffer object "buffer"
//   and has "num_vertices" vertices.
//
void drawObj(GLuint buffer, int num_vertices)
{
    //--- Activate the vertex buffer object to be drawn ---//
    glBindBuffer(GL_ARRAY_BUFFER, buffer);

    /*----- Set up vertex attribute arrays for each vertex attribute -----*/
    GLuint vPosition = glGetAttribLocation(program, "vPosition");
    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

    GLuint vColor = glGetAttribLocation(program, "vColor"); 
    glEnableVertexAttribArray(vColor);
    glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 0,
                          BUFFER_OFFSET(sizeof(point3) * num_vertices) );
    
    GLuint vTexCoord = glGetAttribLocation(program, "vTexCoord" );
    glEnableVertexAttribArray( vTexCoord );
    glVertexAttribPointer( vTexCoord, 2, GL_FLOAT, GL_FALSE, 0,
                          BUFFER_OFFSET(sizeof(point3) * num_vertices) );
    
    
    // the offset is the (total) size of the previous vertex attribute array(s)

    /* Draw a sequence of geometric objs (triangles) from the vertex buffer
       (using the attributes specified in each enabled vertex attribute array) */
    glDrawArrays(GL_TRIANGLES, 0, num_vertices);

    /*--- Disable each vertex attribute array being enabled ---*/
    glDisableVertexAttribArray(vPosition);
    glDisableVertexAttribArray(vColor);
    glDisableVertexAttribArray(vTexCoord);
}


void drawObj2(GLuint buffer, int num_vertices)
{
    //--- Activate the vertex buffer object to be drawn ---//
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    
    /*----- Set up vertex attribute arrays for each vertex attribute -----*/
    GLuint vPosition = glGetAttribLocation(program2, "vPosition");
    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    
    GLuint vNormal = glGetAttribLocation(program2, "vNormal");
    glEnableVertexAttribArray(vNormal);
    glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0,
                          BUFFER_OFFSET(num_vertices * sizeof(point4)));

    GLuint vTexCoord = glGetAttribLocation(program2, "vTexCoord" );
    glEnableVertexAttribArray( vTexCoord );
    glVertexAttribPointer( vTexCoord, 2, GL_FLOAT, GL_FALSE, 0,
                          BUFFER_OFFSET(sizeof(point3) * num_vertices) );
    
    
    // the offset is the (total) size of the previous vertex attribute array(s)
    
    /* Draw a sequence of geometric objs (triangles) from the vertex buffer
     (using the attributes specified in each enabled vertex attribute array) */
    glDrawArrays(GL_TRIANGLES, 0, num_vertices);
    
    /*--- Disable each vertex attribute array being enabled ---*/
    glDisableVertexAttribArray(vPosition);
    glDisableVertexAttribArray(vTexCoord);
    glDisableVertexAttribArray(vNormal);
}


void drawObj3(GLuint buffer, int num_vertices)
{
    //--- Activate the vertex buffer object to be drawn ---//
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    
    /*----- Set up vertex attribute arrays for each vertex attribute -----*/
    GLuint vPosition = glGetAttribLocation(program, "vPosition");
    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    
    GLuint vColor = glGetAttribLocation(program, "vColor");
    glEnableVertexAttribArray(vColor);
    glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 0,
                          BUFFER_OFFSET(sizeof(point3) * num_vertices) );
    
    // the offset is the (total) size of the previous vertex attribute array(s)
    
    /* Draw a sequence of geometric objs (triangles) from the vertex buffer
     (using the attributes specified in each enabled vertex attribute array) */
    //glDrawArrays(GL_TRIANGLES, 0, num_vertices);
    glDrawArrays(GL_TRIANGLES, 0, num_vertices);
    /*--- Disable each vertex attribute array being enabled ---*/
    glDisableVertexAttribArray(vPosition);
    glDisableVertexAttribArray(vColor);

}


//--------------------------------------------------------------------
mat4 BallToShadow = mat4( 12.,  0.,  0.,  0.,
                          14.,  0.,  3., -1.,
                           0.,  0., 12.,  0.,
                           0.,  0.,  0., 12.);

mat4 AccumRotate = mat4(  1,  0,  0,  0,
                          0,  1,  0,  0,
                          0,  0,  1,  0,
                          0,  0,  0,  1);

mat4    TransL;
mat4    Ball;
int     routeFlag = 0;  // 0: A to B  1: B to C  2: C to B
vec3    rotate_axis;
float   d;


void DrawXYZ(mat4 mv0)
{
    isShadowFlag = 0;
    isFloorFlag  = 0;
    isSphereFlag = 0;
    
    /*----- Set up the Mode-View matrix for x y z-----*/
    glUseProgram(program);
    model_view = glGetUniformLocation(program, "model_view" );
    projection = glGetUniformLocation(program, "projection" );
    
    glUniformMatrix4fv(model_view, 1, GL_TRUE, mv0); // GL_TRUE: matrix is row-major
    
    SetUp_Fog_Var1();
    SetUp_Shadow_Var1();
    SetUp_Tex_Floor_Var1();
    SetUp_Tex_Sphere_Var1();
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    drawObj(x_buffer, axis_NumVertices);
    drawObj(y_buffer, axis_NumVertices);
    drawObj(z_buffer, axis_NumVertices);
}


void DrawFloor(mat4 mv0, mat4 p)
{
    isShadowFlag = 0;
    isFloorFlag  = 1;
    isSphereFlag = 0;
    
    /*----- Set up the Mode-View matrix for the floor -----*/
        // The set-up below gives a new scene (scene 2), using Correct LookAt() function
    
    if (lightingFlag == 0) // original floor
        {
        glUseProgram(program);
        model_view = glGetUniformLocation(program, "model_view" );
        projection = glGetUniformLocation(program, "projection" );
    
        glUniformMatrix4fv(projection, 1, GL_TRUE, p);
        glUniformMatrix4fv(model_view, 1, GL_TRUE, mv0);
    
        SetUp_Fog_Var1();
        CheckError();
        SetUp_Shadow_Var1();
        CheckError();
        SetUp_Tex_Floor_Var1();
        SetUp_Tex_Sphere_Var1();
    
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        CheckError();
        drawObj(floor_buffer, floor_NumVertices);  // draw the floor
        CheckError();
        }
    
    else // lighting shadow
        {
        glUseProgram(program2);
        model_view2 = glGetUniformLocation(program2, "ModelView2" );
        projection2 = glGetUniformLocation(program2, "Projection2" );
    
        glUniformMatrix4fv(projection2, 1, GL_TRUE, p);
    
        if (pointlightFlag==1)  SetUp_Point_Light_Var(mv0);
        else                    SetUp_Spot_Light_Var(mv0);
    
        SetUp_Floor_Material_Var();
    
        SetUp_Fog_Var2();
        SetUp_Tex_Floor_Var2();
        SetUp_Shadow_Var2();
        SetUp_Tex_Sphere_Var2();
    
        glUniformMatrix4fv(model_view2, 1, GL_TRUE, mv0 );
        mat3 normal_matrix = NormalMatrix(mv0, 1);
        glUniformMatrix3fv(glGetUniformLocation(program2, "Normal_Matrix"), 1, GL_TRUE, normal_matrix);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        drawObj2(floor_lighting_buffer, floor_NumVertices);
        }
}


void DrawSphere(mat4 mv0, mat4 p)
{
    isShadowFlag = 0;
    isFloorFlag  = 0;
    isSphereFlag = 1;
    mat4    mv = mv0;
    /*----- Set Up the Model-View matrix for the sphere -----*/
    
        // get sphere matrix
    d = (2 * PI * angle) / 360;
    float total_path = length(B - A) + length(C - B) + length(A - C);
    float temp_d;
        // A to B
    if ((routeFlag == 0 && d < length(B - A)) || (routeFlag == 2 && d >= length(A - C)) )
    {   if (routeFlag == 2)
        {   AccumRotate = Rotate(angle - speed, rotate_axis[0], rotate_axis[1], rotate_axis[2]) * AccumRotate;
            angle = 0.0; routeFlag = 0; d = (2 * PI * angle) / 360;
        }
        rotate_axis = cross(OY, B - A);
        TransL = Translate(A + d * normalize(B - A));
    }
        // B to C
    else if ((routeFlag == 1 && d < length(C - B)) || (routeFlag == 0 && d >= length(B - A)) )
    {   if (routeFlag == 0)
        {   AccumRotate = Rotate(angle - speed, rotate_axis[0], rotate_axis[1], rotate_axis[2]) * AccumRotate;
            angle = 0.0; routeFlag = 1; d = (2 * PI * angle) / 360;
        }
        rotate_axis = cross(OY, C - B);
        TransL = Translate(B + d * normalize(C - B));
    }
        // C to A
    else if ((routeFlag == 2 && d < length(A - C))|| (routeFlag == 1 && d >= length(C - B)) )
    {   if (routeFlag == 1)
        {   AccumRotate = Rotate(angle - speed, rotate_axis[0], rotate_axis[1], rotate_axis[2]) * AccumRotate;
            angle = 0.0; routeFlag = 2; d = (2 * PI * angle) / 360;
        }
        rotate_axis = cross(OY, A - C);
        TransL = Translate(C + d * normalize(A - C));
    }
    
    
    Ball = TransL * Scale (1.0, 1.0, 1.0)
                  * Rotate(angle, rotate_axis[0], rotate_axis[1], rotate_axis[2])
                  * AccumRotate;
    mv = mv * Ball;
    
    
    
    
    // draw sphere
    if ((flatshadingFlag == 0 && smoothshadingFlag == 0) || (sphereFlag == 0))
        {
        glUseProgram(program);
        
        model_view = glGetUniformLocation(program, "model_view" );
        projection = glGetUniformLocation(program, "projection" );
        glUniformMatrix4fv(projection, 1, GL_TRUE, p);
        glUniformMatrix4fv(model_view, 1, GL_TRUE, mv);
    
        SetUp_Fog_Var1();
        SetUp_Tex_Floor_Var1();
        SetUp_Shadow_Var1();
        SetUp_Tex_Sphere_Var1();
    
        if (sphereFlag == 1)    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Filled sphere
        else                    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);// Wireframe sphere
        
        drawObj(sphere_buffer, sphere_NumVertices);  // draw the sphere
        }
    
    else
        {
        if (flatshadingFlag == 1)
            {
            glUseProgram(program2);
        
            model_view2 = glGetUniformLocation(program2, "ModelView2" );
            projection2 = glGetUniformLocation(program2, "Projection2" );
            glUniformMatrix4fv(projection2, 1, GL_TRUE, p);
            glUniformMatrix4fv(model_view2, 1, GL_TRUE, mv);
        
            if (pointlightFlag==1)  SetUp_Point_Light_Var(mv0);
            else                    SetUp_Spot_Light_Var(mv0);
            SetUp_Sphere_Material_Var();

            SetUp_Fog_Var2();
            SetUp_Tex_Floor_Var2();
            SetUp_Shadow_Var2();
            SetUp_Tex_Sphere_Var2();
        
            mat3 normal_matrix = NormalMatrix(mv, 1);
            glUniformMatrix3fv(glGetUniformLocation(program2, "Normal_Matrix"), 1, GL_TRUE, normal_matrix);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            drawObj2(flat_buffer,  sphere_NumVertices);
            }
        
        else if (smoothshadingFlag == 1)
            {
            glUseProgram(program2);
        
            model_view2 = glGetUniformLocation(program2, "ModelView2" );
            projection2 = glGetUniformLocation(program2, "Projection2" );
            glUniformMatrix4fv(projection2, 1, GL_TRUE, p);
            glUniformMatrix4fv(model_view2, 1, GL_TRUE, mv);

            if (pointlightFlag==1)  SetUp_Point_Light_Var(mv0);
            else                    SetUp_Spot_Light_Var(mv0);
            SetUp_Sphere_Material_Var();

            SetUp_Fog_Var2();
            SetUp_Tex_Floor_Var2();
            SetUp_Shadow_Var2();
            SetUp_Tex_Sphere_Var2();
        
            mat3 normal_matrix = NormalMatrix(mv, 1);
            glUniformMatrix3fv(glGetUniformLocation(program2, "Normal_Matrix"), 1, GL_TRUE, normal_matrix);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            drawObj2(smooth_buffer, sphere_NumVertices);
            }
        }
}


void DrawShadow(mat4 mv0, mat4 p)
{
    isShadowFlag = 1;
    isFloorFlag = 0;
    isSphereFlag = 0;
    
    mat4    mv = mv0;
    /*----- Set up the Mode-View matrix for the shadow-----*/
    if (shadowFlag == 1)
    {
        glUseProgram(program);
        model_view = glGetUniformLocation(program, "model_view" );
        projection = glGetUniformLocation(program, "projection" );

        mv = mv0 * BallToShadow * Ball;
    
        glUniformMatrix4fv(model_view, 1, GL_TRUE, mv);
    
        SetUp_Fog_Var1();
        SetUp_Tex_Floor_Var1();
        SetUp_Shadow_Var1();
        SetUp_Tex_Sphere_Var1();
    
        if (sphereFlag == 1)
            {
            if (blendingShadowFlag == 1)
                {
                glDepthMask(GL_FALSE);
                drawObj(shadow_buffer, shadow_NumVertices);
                glDepthMask(GL_TRUE);
                }
            else
                {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                drawObj(shadow_buffer, shadow_NumVertices);
                }
            }
        else
            {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            drawObj(shadow_buffer, shadow_NumVertices);
            }

    }
}


void DrawFirework(mat4 mv0, mat4 p, float c_r, float c_g, float c_b, float v_x, float v_y, float v_z)
{
    glUseProgram(program3);
    model_view3 = glGetUniformLocation(program3, "ModelView3" );
    projection3 = glGetUniformLocation(program3, "Projection3" );
    
    glUniformMatrix4fv(projection3, 1, GL_TRUE, p);
    glUniformMatrix4fv(model_view3, 1, GL_TRUE, mv0);
    
    glPointSize(3.0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
    
    float T = glutGet(GLUT_ELAPSED_TIME);
    float time = T - T_sub;
    while(time > 10000.0)
        time  = time - 10000.0;

    glUniform1f( glGetUniformLocation(program3, "Time"), time);
    glUniform1f( glGetUniformLocation(program3, "ColorR"), c_r);
    glUniform1f( glGetUniformLocation(program3, "ColorG"), c_g);
    glUniform1f( glGetUniformLocation(program3, "ColorB"), c_b);
    glUniform1f( glGetUniformLocation(program3, "VelocityX"), v_x);
    glUniform1f( glGetUniformLocation(program3, "VelocityY"), v_y);
    glUniform1f( glGetUniformLocation(program3, "VelocityZ"), v_z);
    
    if (fireworkFlag == 1)
        drawObj3(firework_buffer, firework_NumVertices);
}


//----------------------------------------------------------------------------
void display( void )
{

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

/*---  Set up and pass on Projection matrix to the shader ---*/
    mat4    p = Perspective(fovy, aspect, zNear, zFar);
    
/*---  Set up and pass on Model-View matrix to the shader ---*/
    // eye is a global variable of vec4 set to init_eye and updated by keyboard()
    vec4    at(0.0, 0.0, 0.0, 1.0);
    vec4    up(0.0, 1.0, 0.0, 0.0);
    mat4    mv0 = LookAt(eye, at, up);

    
    glEnable(GL_DEPTH_TEST); // always enable test
    
    if (eye[1] >= 0.0)// when viewer is above plane,
        {
        glDepthMask(GL_FALSE); // read anything into buffer but floor
        DrawFloor(mv0, p);
        glDepthMask(GL_TRUE);
        DrawSphere(mv0, p);
        if (blendingShadowFlag == 1)
            {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            DrawShadow(mv0, p);
            glDisable(GL_BLEND);
            }
        else
            DrawShadow(mv0, p);
        DrawXYZ(mv0);
        }
    
    else // when viewer is under the plane
        {
        DrawSphere(mv0, p);
        glDepthMask(GL_FALSE);
        if (blendingShadowFlag == 1)
            {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            DrawShadow(mv0, p);
            glDisable(GL_BLEND);
            }
        else
            DrawShadow(mv0, p);
        glDepthMask(GL_TRUE);
    
        DrawXYZ(mv0);
        DrawFloor(mv0, p);
        }
    for (int i = 0; i < firework_NumVertices; i++)
        {
        DrawFirework(mv0, p,
                     firework_colors[i][0], firework_colors[i][1], firework_colors[i][2],
                     firework_points[i].x,  firework_points[i].y,  firework_points[i].z);
        }
    glutSwapBuffers();
}



//---------------------------------------------------------------------------
void idle (void)
{
    //angle += 0.02;
    angle += speed;    //YJC: change this value to adjust the cube rotation speed.
    glutPostRedisplay();
}



//----------------------------------------------------------------------------
void keyboard(unsigned char key, int x, int y)
{
    switch(key)
    {
        case 'X': eye[0] += 1.0; break;
        case 'x': eye[0] -= 1.0; break;
        case 'Y': eye[1] += 1.0; break;
        case 'y': eye[1] -= 1.0; break;
        case 'Z': eye[2] += 1.0; break;
        case 'z': eye[2] -= 1.0; break;

        case 'b': case 'B': // Toggle between animation and non-animation
            if (beginFlag == 0)
            {
                animationFlag = 1;
                beginFlag = 1;
            }
            if (animationFlag == 1) glutIdleFunc(idle);
            else                    glutIdleFunc(NULL);
            break;
        
        
        case 'f': case 'F': // Toggle between filled and wireframe floor
            floorFlag = 1 -  floorFlag;
            break;
        
        case 'e': case 'E':
            eyeSpaceFlag = 1;
            objectSpaceFlag = 0;
            break;
        
        case 'o': case 'O':
            if (objectSpaceFlag == 0)
                { tiltedFlag = 0; upRightFlag = 1;}
            eyeSpaceFlag = 0;
            objectSpaceFlag = 1;
            break;
        
        case 'v': case 'V':
            verticalFlag = 1;
            slantedFlag = 0;
            if (objectSpaceFlag == 1)
                { tiltedFlag = 0; upRightFlag = 1;}
            break;
        
        case 's': case 'S':
            verticalFlag = 0;
            slantedFlag = 1;
            if (objectSpaceFlag == 1)
                { tiltedFlag = 1; upRightFlag = 0;}
            break;
        
        case 'l': case 'L':
            if (sphereFlag == 0)
                latticeFlag = 0;
            latticeFlag = 1 - latticeFlag;
            break;
        
        case 't': case 'T':
            tiltedFlag = 1;
            upRightFlag = 0;
            if (objectSpaceFlag == 1)
                { verticalFlag = 0;  slantedFlag = 1; }
            break;
        
        case 'u': case 'U':
            tiltedFlag = 0;
            upRightFlag = 1;
            if (objectSpaceFlag == 1)
                { verticalFlag = 1;  slantedFlag = 0; }
            break;
        
    }
    
    glutPostRedisplay();
}



//----------------------------------------------------------------------------
void myMouse(int button, int state, int x, int y)
{
    if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN && beginFlag == 1)
    {
        animationFlag = 1 - animationFlag;
        if (animationFlag == 1)
            glutIdleFunc(idle);
        else
            glutIdleFunc(NULL);
    }
}



//----------------------------------------------------------------------------
void top_menu(int id)
{
    switch(id)
    {
        case 1: // back to init position
            eye = init_eye;
            break;
        case 2: // wireframe
            sphereFlag = 1-sphereFlag;
            if (sphereFlag == 0)
                latticeFlag = 0;
            break;
        case 3: // quit
            exit(0);
            break;
    }
    glutPostRedisplay();
}

void shadow_menu(int id)
{
    switch(id)
    {
        case 1: shadowFlag = 0;  break;
        case 2: shadowFlag = 1;  break;
    }
    glutPostRedisplay();
}


void shade_menu(int id)
{
    switch(id)
    {
        case 1: // flat shading
            if (lightingFlag == 1)
            {
                flatshadingFlag  = 1;
                smoothshadingFlag= 0;
            }
            sphereFlag           = 1;
            break;
    
        case 2: // smooth shading
            if (lightingFlag == 1)
            {
                flatshadingFlag  = 0;
                smoothshadingFlag= 1;
            }
            sphereFlag           = 1;
            break;
    }
    glutPostRedisplay();
}


void lighting_menu(int id)
{
    switch(id)
    {
        case 1:
            lightingFlag        = 0;
            flatshadingFlag     = 0;
            smoothshadingFlag   = 0;
            break;
        case 2:
            lightingFlag = 1;  break;
    }
    glutPostRedisplay();
}


void light_source_menu(int id)
{
    switch(id)
    {
        case 1: // spot light
            spotlightFlag  = 1;
            pointlightFlag = 0;
            break;
        case 2: // point
            spotlightFlag       = 0;
            pointlightFlag      = 1;
            break;
    }
    glutPostRedisplay();
}

void fog_menu(int id)
{
    switch(id)
    {
        case 1:  fogFlag = 0;  break;
        case 2:  fogFlag = 1;  break;
        case 3:  fogFlag = 2;  break;
        case 4:  fogFlag = 3;  break;
    }
    glutPostRedisplay();
}


void blending_shadow_menu(int id)
{
    switch(id)
    {
        case 1:  blendingShadowFlag = 0; break;
        case 2:  blendingShadowFlag = 1; break;
    }
    glutPostRedisplay();
}


void tex_map_ground_menu(int id)
{
    switch(id)
    {
        case 1:  texGroundFlag = 0; break;
        case 2:  texGroundFlag = 1; break;
    }
    glutPostRedisplay();
}


void tex_map_sphere_menu(int id)
{
    switch(id)
    {
        case 1:  texSphereFlag = 0; break;
        case 2:  texSphereFlag = 1; break;
        case 3:  texSphereFlag = 2; break;
    }
    glutPostRedisplay();
}


void firework_menu(int id)
{
    switch(id)
    {
        case 1:
            fireworkFlag = 0;
            break;
        case 2:
            if  (fireworkFlag == 0)
                {
                fireworkFlag = 1;
                T_sub = glutGet(GLUT_ELAPSED_TIME);
                }
            break;
    }
    glutPostRedisplay();
}



//----------------------------------------------------------------------------
void reshape(int width, int height)
{
    glViewport(0, 0, width, height);
    aspect = (GLfloat) width  / (GLfloat) height;
    Perspective(fovy, aspect, zNear, zFar);
    glutPostRedisplay();
}



//----------------------------------------------------------------------------
void add_menu()
{
    int shadow_id = glutCreateMenu(shadow_menu);
    glutAddMenuEntry("No", 1);
    glutAddMenuEntry("Yes", 2);
    
    int shade_id = glutCreateMenu(shade_menu);
    glutAddMenuEntry("flat shading", 1);
    glutAddMenuEntry("smooth shading", 2);
    
    int lighting_id = glutCreateMenu(lighting_menu);
    glutAddMenuEntry("No", 1);
    glutAddMenuEntry("Yes", 2);
    
    int light_source_id = glutCreateMenu(light_source_menu);
    glutAddMenuEntry("spot light", 1);
    glutAddMenuEntry("point source", 2);
    
    int fog_id = glutCreateMenu(fog_menu);
    glutAddMenuEntry("no fog", 1);
    glutAddMenuEntry("linear", 2);
    glutAddMenuEntry("exponential", 3);
    glutAddMenuEntry("exponential square", 4);
    
    int blending_shadow_id = glutCreateMenu(blending_shadow_menu);
    glutAddMenuEntry("No", 1);
    glutAddMenuEntry("Yes", 2);
    
    int tex_map_ground_id = glutCreateMenu(tex_map_ground_menu);
    glutAddMenuEntry("No", 1);
    glutAddMenuEntry("Yes", 2);
    
    int tex_map_sphere_id = glutCreateMenu(tex_map_sphere_menu);
    glutAddMenuEntry("No", 1);
    glutAddMenuEntry("Yes - Contour Lines", 2);
    glutAddMenuEntry("Yes - Checkerboard",  3);
    
    int firework_id = glutCreateMenu(firework_menu);
    glutAddMenuEntry("No", 1);
    glutAddMenuEntry("Yes", 2);
    
    
    glutCreateMenu(top_menu);
    glutAddMenuEntry("Default View Point", 1);
    glutAddMenuEntry("Wire Frame Sphere", 2);
    glutAddMenuEntry("Quit", 3);
    
    glutAddSubMenu("Shadow",                shadow_id);
    glutAddSubMenu("Shading",               shade_id);
    glutAddSubMenu("Enable Lighting",       lighting_id);
    glutAddSubMenu("Light Source",          light_source_id);
    glutAddSubMenu("Fog Options",           fog_id);
    glutAddSubMenu("Blending Shadow",       blending_shadow_id);
    glutAddSubMenu("Texture Mapped Ground", tex_map_ground_id);
    glutAddSubMenu("Texture Mapped Sphere", tex_map_sphere_id);
    glutAddSubMenu("Firework",              firework_id);
    
    glutAttachMenu(GLUT_LEFT_BUTTON);
}



//----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    int err;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(512, 512);
    glutCreateWindow("Rolling Sphere with Shading");

/* Call glewInit() and error checking */
    err = glewInit();
    if (GLEW_OK != err)
    {
        printf("Error: glewInit failed: %s\n", (char*) glewGetErrorString(err));
        exit(1);
    }
    
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutIdleFunc(NULL);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(myMouse);
    
    init();
    add_menu(); // sub menu

    glutMainLoop();
    return 0;
}
