//Source : https://github.com/JoeyDeVries/LearnOpenGL/blob/master/src/5.advanced_lighting/3.1.3.shadow_mapping/3.1.3.shadow_mapping.fs
//https://github.com/JoeyDeVries/LearnOpenGL/blob/master/src/2.lighting/2.2.basic_lighting_specular/2.2.basic_lighting.fs
#version 330 core

out vec4 FragColor;
out vec3 frag_position;
out vec3 Normal;
out vec2 TexCoords;
out vec4 frag_position_light_space_matrix;


uniform sampler2D diffuseTexture;
uniform sampler2D shadowMap;

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform vec3 lightColor;
uniform vec3 objectColor;


uniform bool texture_on;
uniform bool shadow_on;

// for texture
struct Material {
    sampler2D diffuse;
    vec3 specular;    
    float shininess;
}; 
//for light
struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float cutOff;
};


uniform vec4 shader_color ;

in float height_temp;


vec4 blue = vec4(0.0, 0.0, 1.0, 1.0);  
vec4 green = vec4(0.1, 0.22, 0.15, 1.0);
vec4 white = vec4(0.0, 0.2, 0.2, 1.0);

void main()
{    




float height = height_temp/2.0+0.5;

vec4 pixel_colour = vec4(0.0,0.0,0.0,1.0);

//green + blue gradient
if(height < 0.5)
{
  pixel_colour = (1.0-height*0.5)*blue + height*2*green ;
}
//green + white gradient
else
{
  pixel_colour = (1.0-(height-0.5)*2)*green + (height-0.5)*2*white;
}


FragColor = pixel_colour;


   
}


