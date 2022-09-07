/*
* Inspired by : https://github.com/damdoy/opengl_examples
*/
#ifndef TERRAIN_H
#define TERRAIN_H

#include <vector>
#include <chrono>
#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader.h"
#include "transform.h"

#define PROFILING 1


class Ground{
public:

   void init(GLuint _program_ID){
      this->_programID = _program_ID;
      if(_programID == 0) exit(-1);
      glGenVertexArrays(1, &_vao);
      glBindVertexArray(_vao);
      sub_x = 100;
      sub_y = 100;
      nb_vertices = sub_x*sub_y;
      unsigned int nb_quads = (sub_x-1)*(sub_y-1);
      unsigned int nb_tris = nb_quads*2;
      nb_indices = nb_tris*3;
      vertices = new GLfloat[nb_vertices*3];
      indices = new GLuint[nb_indices];
      //fill the indices array
      set_indices();
      noise_start_factor = 0.70f; //multiplication factor for the first noise level
      noise_factor = 0.40f; 
      heightmap.resize(sub_x);
      for(unsigned int i = 0; i < sub_x; i++){
         heightmap[i].resize(sub_y);
 
      }
      generate_heightmap( &Ground::recursive_noise, sub_x, sub_y);
      generate_ground();
      glGenBuffers(1, &_va_ground);
      glGenBuffers(1, &_vb_ground);
      glGenBuffers(1, &_ebo_ground);
      glBindVertexArray(_va_ground);
      glBindBuffer(GL_ARRAY_BUFFER, _va_ground);
      glBufferData(GL_ARRAY_BUFFER, nb_vertices * 3 * sizeof(GLfloat), vertices, GL_STATIC_DRAW);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo_ground);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, nb_indices * sizeof(GLuint), indices, GL_STATIC_DRAW);
      glGenBuffers(1, &_vb_ground);
      glBindBuffer(GL_ARRAY_BUFFER, _vb_ground);
      glBufferData(GL_ARRAY_BUFFER, nb_vertices*3*sizeof(GLfloat), vertices, GL_STATIC_DRAW);
      GLuint vpoint_id = glGetAttribLocation(_programID, "aPosition");
      glEnableVertexAttribArray(vpoint_id);
      glVertexAttribPointer(vpoint_id, 3, GL_FLOAT, GL_FALSE, 0, NULL);
      glBindVertexArray(0);
   }

   void draw(glm::mat4 model, glm::mat4 view, glm::mat4 projection, GLfloat light_position[3],
      bool colorON, bool linesON){
   
      glUseProgram(_programID);
      glBindVertexArray(_vao);

      glUniform3fv(glGetUniformLocation(_programID, "light_position"), 1, light_position);
      glUniform1ui( glGetUniformLocation(_programID, "u_colorON"), colorON);
      glUniformMatrix4fv( glGetUniformLocation(_programID, "u_model"), 1, GL_FALSE, glm::value_ptr(model));
      glUniformMatrix4fv( glGetUniformLocation(_programID, "u_view"), 1, GL_FALSE, glm::value_ptr(view));
      glUniformMatrix4fv( glGetUniformLocation(_programID, "u_projection"), 1, GL_FALSE, glm::value_ptr(projection));
      
      if (linesON) {
          glDrawElements(GL_LINES, nb_indices, GL_UNSIGNED_INT, 0);
      }
      else {
          glDrawElements(GL_TRIANGLES, nb_indices, GL_UNSIGNED_INT, 0);
      }
      glUseProgram(0);
      glBindVertexArray(0);
   }


   void draw(glm::mat4 view, glm::mat4 projection, GLfloat light_position[3], bool colorON, bool linesON) {
      this->draw(model_transf.get_matrix(), view, projection,light_position, colorON, linesON);
   }

   //set model matrix
   void apply_transformation(Transform transf){
      model_transf = transf;
   }

 

   float get_height(float pos_x, float pos_y){

      glm::vec4 vec(pos_x, 1.0f, pos_y, 1.0f);
      glm::mat4 transf_matrix = model_transf.get_matrix();
      glm::vec4 vec_transf = glm::inverse(transf_matrix)*vec;


      float height = recursive_noise(vec_transf[0], vec_transf[2]);
      height = (transf_matrix*glm::vec4(0.0f, height, 0.0f, 1.0f))[1];
      
      return height;
   }

   std::vector<std::vector<float>> get_heightmap() {
       return heightmap;
   }

   void cleanup(){
      if(vertices != NULL){
         delete vertices;
         vertices = NULL;
      }

      if(indices != NULL){
         delete indices;
         indices = NULL;
      }

      glDeleteBuffers(1, &_vb_ground);
      glDeleteVertexArrays(1, &_vao);
      glDeleteProgram(_programID);
   }

private:
   Transform model_transf;

   GLuint _vao;
   GLuint _vb_ground;
   GLuint _va_ground;
   GLuint _ebo_ground;

   GLuint _programID;

   unsigned int sub_x;
   unsigned int sub_y;

   GLfloat *vertices = NULL;
   GLuint *indices = NULL;


   unsigned int nb_vertices;
   unsigned int nb_indices;


   float noise_start_factor;
   float noise_factor;


   //2d array
   std::vector<std::vector<float>> heightmap;


   float function_sin(float x, float y){
      return sin(x*y*10)/10;
   }
 
   float recursive_noise(float x, float y){
      unsigned int segmentation = 8; 
      float val_ret = 0.0f;
      float factor = noise_start_factor;

      for (unsigned int i = 0; i < 6; i++){
        val_ret = (2.0f*perlinNoise(x, y, segmentation)-1.0f)*factor +val_ret;
        segmentation *= 2;
        factor *= noise_factor;
      }
      return val_ret;
   }

   //find perlin calulated height value for this point
   float perlinNoise(float x, float y, unsigned int segmentation){
      float square_size = 1.0f/(segmentation); 

      float x_coordinate = (x+1.0)/2.0;
      float y_coordinate = (y+1.0)/2.0;

      //Random values
      float rand_val_00;
      float rand_val_01;
      float rand_val_10;
      float rand_val_11;
      //4 gradient vectors
      float gradient_00[2];
      float gradient_01[2];
      float gradient_10[2];
      float gradient_11[2];
      generateRandomValues(x_coordinate, y_coordinate, square_size, &rand_val_00, &rand_val_01, &rand_val_10, &rand_val_11);

      //find random gradients
      generateGradientFromRandomValues(rand_val_00*32, gradient_00);
      generateGradientFromRandomValues(rand_val_01*32, gradient_01);
      generateGradientFromRandomValues(rand_val_10*32, gradient_10);
      generateGradientFromRandomValues(rand_val_11*32, gradient_11);

      float frac_x = fmod(x_coordinate, square_size)/square_size;
      float frac_y = fmod(y_coordinate, square_size)/square_size;

      float vec_00[2] = {frac_x, frac_y}; 
      float vec_01[2] = {frac_x, -(1.0f-frac_y)}; 
      float vec_10[2] = { -(1.0f-frac_x) , frac_y}; 
      float vec_11[2] = { -(1.0f-frac_x), -(1.0f-frac_y)};

      float dot_00 = vec_00[0]*gradient_00[0]+vec_00[1]*gradient_00[1];
      float dot_01 = vec_01[0]*gradient_01[0]+vec_01[1]*gradient_01[1];
      float dot_10 = vec_10[0]*gradient_10[0]+vec_10[1]*gradient_10[1];
      float dot_11 = vec_11[0]*gradient_11[0]+vec_11[1]*gradient_11[1];

      frac_x = 6*pow(frac_x, 5)-15*pow(frac_x, 4)+10*pow(frac_x, 3);
      frac_y = 6*pow(frac_y, 5)-15*pow(frac_y, 4)+10*pow(frac_y, 3);

      float pixel_x1 = (1.0f-frac_x)*dot_00+frac_x*dot_10;
      float pixel_x2 = (1.0f-frac_x)*dot_01+frac_x*dot_11;
      float pixel = (1.0f-frac_y)*pixel_x1+frac_y*pixel_x2;
      pixel += 0.5f;

      return pixel;
   }

   void generateGradientFromRandomValues(unsigned int rand_val, float gradient[2]){
      switch(rand_val%4){
      case 0:
         gradient[0] = 1.0f;
         gradient[1] = 1.0f;
         break;
      case 1:
         gradient[0] = -1.0f;
         gradient[1] = 1.0f;
         break;
      case 2:
         gradient[0] = 1.0f;
         gradient[1] = -1.0f;
         break;
      case 3:
         gradient[0] = -1.0f;
         gradient[1] = -1.0f;
         break;
      }
   }

   void generateRandomValues(float x_corr, float y_corr, float square_size, float *rand_00, float *rand_01, float *rand_10, float *rand_11){

      float nearest_square_x = x_corr-fmod(x_corr, square_size);
      float nearest_square_y = y_corr-fmod(y_corr, square_size);
      float nearest_square_x_2 = x_corr-fmod(x_corr, square_size) + square_size;
      float nearest_square_y_2 = y_corr-fmod(y_corr, square_size) + square_size;

      unsigned int seed_x = (nearest_square_x*1024*11);
      unsigned int seed_y = (nearest_square_y*1024*11);
      unsigned int seed_x_2 = (nearest_square_x_2*1024*11);
      unsigned int seed_y_2 = (nearest_square_y_2*1024*11);

      *rand_00 = rand2d(seed_x, seed_y)/(float)MAX_RAND;
      *rand_01 = rand2d(seed_x, seed_y_2)/(float)MAX_RAND;
      *rand_10 = rand2d(seed_x_2, seed_y)/(float)MAX_RAND;
      *rand_11 = rand2d(seed_x_2, seed_y_2)/(float)MAX_RAND;
   }



   const unsigned int MAX_RAND = 0xFFFFFFFF;

   //repeatable rand functions
   unsigned int rand(){
      static unsigned int seed = 0;
      unsigned int a = 1664525;
      unsigned int c = 1013904223;
      seed = (a * seed + c);
      return seed;
   }

   unsigned int rand(unsigned int v){
      unsigned int a = 1664525;
      unsigned int c = 1013904223;
      return ((a * v + c));
   }

   unsigned int rand2d(unsigned int x, unsigned int y){
      return rand(y+rand(x));
   }

   void generate_ground(){
      for(unsigned int j = 0; j < sub_y; j++){
         for(unsigned int i = 0; i < sub_x; i++){

            GLfloat relative_x = (float(i)/(sub_x-1))*2-1;
   
            GLfloat relative_z = (float(j)/(sub_y-1))*2-1;

            unsigned int cur_pos = (j*sub_x+i)*3;

            vertices[cur_pos] = relative_x;
            vertices[cur_pos+1] = heightmap[i][j];
            vertices[cur_pos+2] = relative_z;

     
         }
      }
   }

   void generate_heightmap( float (Ground:: *func)(float, float), unsigned int grid_x, unsigned int grid_y){

      for(unsigned int j = 0; j < grid_x; j++){
         for(unsigned int i = 0; i < grid_y; i++){

            GLfloat relative_x = (float(i)/(grid_x-1))*2-1;
            GLfloat relative_z = (float(j)/(grid_y-1))*2-1;
            heightmap[i][j] = (*this.*func)(relative_x, relative_z);

         }
      }
   }

   void set_indices(){
      for (unsigned int j = 0; j < sub_y-1; j++){
         for(unsigned int i = 0; i < sub_x-1; i++){
            unsigned int v0 = j*sub_x+i;
            unsigned int v1 = j*sub_x+i+1;
            unsigned int v2 = (j+1)*sub_x+i;
            unsigned int v3 = (j+1)*sub_x+i+1;

            unsigned int index_ptr = (j*(sub_x-1)+i)*6;
            //printf("%d\n", index_ptr);

            indices[index_ptr] = v0;
            indices[index_ptr+1] = v3;
            indices[index_ptr+2] = v1;

            indices[index_ptr+3] = v0;
            indices[index_ptr+4] = v2;
            indices[index_ptr+5] = v3;
         }
      }
   }


   
};
#endif

