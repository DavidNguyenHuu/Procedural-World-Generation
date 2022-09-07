#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h> 


class World
{

public:
    bool Keys[1024];
    unsigned int Width, Height;

   // std::vector <Trees> trees;
   
 
    //Constructor
    World(unsigned int width, unsigned int height);
    //Destructor
    ~World();
    void Init(unsigned int input_width, unsigned int input_height);
    void Render();
    void ProcessInput(GLFWwindow* window, float dt);
    unsigned int loadTexture(char const* path);

    void DoCollisions();



};
#endif