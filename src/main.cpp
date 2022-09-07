#define GLEW_STATIC 1

#include <GL/glew.h>
#include <GLFW/glfw3.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include <fstream>
#include <string>
#include <sstream>
#include <cmath>
#include <vector>

#include <stdlib.h>
#include <time.h> 
#include "stb_image.h"

#include "world.h"



const int WIDTH= 1024;
const int HEIGHT= 768;

int input_width = 100;
int input_height = 8;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

World world(WIDTH,HEIGHT);

int main() {


    if (!glfwInit()) {
        std::cout << "Error to initialize GLFW" << std::endl;
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "COMP 371 : PROCEDURAL WORLD - 26659330", NULL, NULL);

    if (!window) {
        std::cout << "failed to open window" << std::endl;
        return -1;
    }
    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_NO_ERROR) {
        std::cout << "glew error\n";
        return -1;
    }

    //Background color
    glClearColor(0.5647058823529412, 0.6274509803921569, 0.6705882352941176, 1.0);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


    //Initiate world with user input
    world.Init(input_width,input_height);

    //main loop: control and drawing
    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && !glfwWindowShouldClose(window)) {

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glfwPollEvents();

        static float lastFrame_ = 0;
        float currentTime_ = glfwGetTime();
        float dt = currentTime_ - lastFrame_;

       //Update World
        world.ProcessInput(window, dt);



        //Render World
        world.Render();

        glfwSwapBuffers(window);

        lastFrame_ = currentTime_;
    }

 

    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
   
    glViewport(0, 0, width, height);
}



