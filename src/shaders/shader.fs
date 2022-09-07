#version 330 core


in vec3 frag_normal_transformed;
in vec3 frag_position;

uniform vec3 camera_position;
uniform vec3 light_position;

uniform bool u_colorON;

out vec3 color;

in float frag_nontransfheight;

uniform vec4 shader_color;

//colors the terrain depending on the height
const vec3 COLOR[3] = vec3[](
    vec3(0.0, 0.0, 1.0), //blue, water
    vec3(0.1, 0.22, 0.15), // green, grass
    vec3(0.0, 0.8, 0.8)); //gray/white, snowtops

void main(){

   vec3 light_dir = normalize(light_position-frag_position);
  
  float diffuse_light = 0.0;

   vec3 view_dir = normalize(camera_position-frag_position);
   diffuse_light = dot(frag_normal_transformed, light_dir);


   float lum = 0.8*diffuse_light;
   float height = frag_nontransfheight/2.0+0.5;
   

   vec3 pixel_colour = vec3(0.0);

   if(height < 0.5){
      pixel_colour = (1.0-height*2)*COLOR[0]+height*2*COLOR[1];
   }else{
      pixel_colour = (1.0-(height-0.5)*2)*COLOR[1]+(height-0.5)*2*COLOR[2];
   }

   if(!u_colorON){
      pixel_colour = vec3(1.0);
   }
      color = pixel_colour*lum;
    }
