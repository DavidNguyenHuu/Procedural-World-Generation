/*
* Inspired by : https://github.com/damdoy/opengl_examples
*/
#ifndef CAMERA_H
#define CAMERA_H

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

//fixed camera
class Camera{
public:
   Camera(){

   }

   virtual void lookAt(glm::vec3 eye, glm::vec3 center, glm::vec3 up){
      this->eye = eye;
      this->center = center;
      this->up = up;
   }

   virtual void lookAt(float eyeX, float eyeY, float eyeZ, float centerX, float centerY, float centerZ, float upX, float upY, float upZ){
      lookAt(glm::vec3(eyeX, eyeY, eyeZ), glm::vec3(centerX, centerY, centerZ), glm::vec3(upX, upY, upZ));
   }

   glm::vec3 get_position(glm::vec3 position) {
       position.x = eye.x;
       position.y = eye.y;
       position.z = eye.z;
       
       glm::vec3 result = glm::vec3(position.x,position.y,position.z);
       
       return result;
   }

   

   void get_direction(glm::vec3 direction){
      direction.x = center.x-eye.x;
      direction.y = center.y-eye.y;
      direction.z = center.z-eye.z;
   }


   glm::mat4 getMatrix(){
      return glm::lookAt(eye, center, up);
   }

   glm::mat4 get_perspective_mat(){
      return glm::perspective(3.1415f, (float)win_width/(float)win_height, 0.1f, 1000.0f);
   }

   virtual void Update(char, float){

   }


protected:

   //for the lookat functon
   glm::vec3 eye;
   glm::vec3 center;
   glm::vec3 up;

   unsigned int win_width;
   unsigned int win_height;
};

#endif
