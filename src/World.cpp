#include "World.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/string_cast.hpp>

#include <string>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cmath>
#include <vector>

#include <stdio.h>
#include <stdlib.h>
#include <time.h> 
#include "stb_image.h"
//Shapes

//Models
#include "ground.h"
#include "Tree.h"

//Cameras
#include "camera.h"
#include "flyCam.h"
#include "player.h"
#include "shader.h"


//Method helps with transformation of models
#include "transform.h" 

float FOV = 1.5;
Camera *cam;
Fly_Cam flyCam;
Player player;

//Model intialization
Ground ground;
Ground grass;
Tree treeTop;
Tree treeBottom;


int density = 100;
//For randomness
std::vector<glm::vec3> treePositions;
std::vector<glm::vec3> grassPositions;
std::vector<glm::vec3> randomScale;

//Heightmap
std::vector<std::vector<float>> heightmap;
std::vector<float> y_position;

//Matrices
glm::mat4 projection;
glm::mat4 model;
glm::mat4 view;


glm::vec3 camera_position;


//Drawing parameters
bool colorON = false;
bool textureON = false;
bool linesON = false;

// Time
double lastFrame_ = glfwGetTime();
float  frameTime_ = 0.0f;
///////

bool moving_light;

GLfloat light_position[3];



int move = 0;


World::World(unsigned int width, unsigned int height)
    : Keys(), Width(width), Height(height)
{
    //Leave empty
}

World::~World()
{
}

void World::Init(unsigned int input_width, unsigned int input_height)
{

    light_position[0] = 0.0; //x
    light_position[1] = 10.0; //up
    light_position[2] = 0.0; //z


    /*
    *  edit: increased "far" value to see world further.
    */

    projection = glm::perspective(FOV, (float)Width / (float)Height, 0.01f, 200.0f);
    
    //Load Shaders
    //
    //GROUND 
    Transform ground_transformation;
    ground_transformation.scale(input_width,input_height,input_width);
    GLuint _programID = load_shaders("src/shaders/ground_shader.vs", "src/shaders/ground_shader.fs");
    ground.init(_programID);
    ground.apply_transformation(ground_transformation);

    //GRASS
    _programID = load_shaders("src/shaders/grass_shader.vs", "src/shaders/grass_shader.fs");
    grass.init(_programID);

    
    //TREE
    _programID = load_shaders("src/shaders/tree_shader.vs", "src/shaders/tree_shader.fs");
    treeTop.init(_programID);
    treeBottom.init(_programID);


 
   

   //Generate Random Positions for trees and grass
    srand(glfwGetTime());
    for (int i = 0; i <density; i++) {
        float randX = rand() %100;
        float randY = rand()%8+4;
        float randZ = rand() %100;
        treePositions.push_back(glm::vec3(randX, randY,randZ));
        treePositions.push_back(glm::vec3(-randX, randY, -randZ));
        treePositions.push_back(glm::vec3(randX, randY, -randZ));
        treePositions.push_back(glm::vec3(-randX, randY, randZ));
        
        //This generates the grass right under the tree.
        grassPositions.push_back(glm::vec3(randX, 0, randZ));
        grassPositions.push_back(glm::vec3(-randX, 0, -randZ));
        grassPositions.push_back(glm::vec3(randX, 0, -randZ));
        grassPositions.push_back(glm::vec3(-randX, 0, randZ));
    }

    //Random scaling : essentially generating random vectors.
    srand(glfwGetTime());
    for (int i = 0; i < density; i++) {
        float randX = rand()%20+1;
        float randY = rand()%4+1;
        float randZ = rand()%20+1;
        //This generates the grass right under the tree.
        randomScale.push_back(glm::vec3(randX, randY, randZ));
    }

    /* Print random vectors (debug)
    for (int i = 0; i < randomScale.size(); i++) {
        std::cout << glm::to_string(randomScale.at(i)) << std::endl ;
    }*/


 

    //initialize flying camera
    flyCam.lookAt(3.0f, 3.0f, 3.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
    flyCam.set_speed(25.0f);



    //initialize player
    player.lookAt(1.5f, 1.5f, 1.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
    //Set player bound to ground
    player.init(0.5f, &ground);
    player.set_speed(10.0f);
    cam = &player;
    player.update_pos();
    

}



//TO-DO : 
/*
*  - AABB collision :
*/ 
void World::DoCollisions()
{
    /*
    * 
    * 
    */
    glm::vec3 temp = cam->get_position(camera_position);


    for (int i=0; i < treePositions.size(); i++) {

        glm::vec3 treeTemp = treePositions.at(i);
        if (temp.x == treeTemp.x)
        {
            std::cout << "Collided" << std::endl;
        }
        /*
        * if (CheckCollision){
        *  set player position back
        * }
        * 
        */
    }
 }
  

//bool CheckCollision(Object # 1 , Object # 2);
bool CheckCollision() // AABB - AABB collision [ AXIS ALIGNED BOUNDING BOX ]
/*
* Edges are parallel to the ground , makes it easy to calculate.
*/
{
    /*
    * Pseudo-code:  pos1    +   size1     > =    pos2      &&         pos2    +     size2
    *                             

    * -> BOOL checkColisionX: [obj_ONE_pos_X + obj_ONE_size_X] >= [obj_pos_TWO_X]  &&AND  [obj_pos_TWO_X] + [obj_TWO_size_X] >= [obj_ONE_pos_X]
    * 
    * -> BOOL checkColisionZ:  [obj_ONE_pos_Z + obj_ONE_size_Z] >= [obj_pos_TWO_Z] &&AND  [obj_pos_TWO_Z] + [obj_TWO_size_Z] >= [obj_ONE_pos_Z]
    *  
    * return checkCollision X & Y 
    * 
    */

    return true;
}



void World::ProcessInput(GLFWwindow* window, float dt)
{
    if (glfwGetKey(window, '0') == GLFW_PRESS) {
 
    }
    else if (glfwGetKey(window, '1') == GLFW_PRESS) {
        linesON = true;
    }
    else if (glfwGetKey(window, '2') == GLFW_PRESS) {
        linesON = false;
    }

    // Movement (W A S D)
    if (glfwGetKey(window, 'S') == GLFW_PRESS) {
        cam->Update('S', dt);
    }
    if (glfwGetKey(window, 'A') == GLFW_PRESS) {
        cam->Update('A', dt);
    }
    if (glfwGetKey(window, 'W') == GLFW_PRESS) {
        cam->Update('W', dt);
    }
    if (glfwGetKey(window, 'D') == GLFW_PRESS) {
        cam->Update('D', dt);
    }


    // LOOKING ( MOUSE )
    if (glfwGetKey(window, 'L') == GLFW_PRESS) {
        cam->Update('L', dt);
    }
    if (glfwGetKey(window, 'J') == GLFW_PRESS) {
        cam->Update('J', dt);
    }
    if (glfwGetKey(window, 'K') == GLFW_PRESS) {
        cam->Update('K', dt);
    }
    if (glfwGetKey(window, 'I') == GLFW_PRESS) {
        cam->Update('I', dt);
    }

    //change camera type
    if (glfwGetKey(window, 'M') == GLFW_PRESS) {
        cam = &flyCam;
    }
    if (glfwGetKey(window, 'N') == GLFW_PRESS) {
        cam = &player;
    }

    if (glfwGetKey(window, 'B') == GLFW_PRESS) {
    
    }
    if (glfwGetKey(window, 'V') == GLFW_PRESS) {
      
    }

    //activate the colours of the terrain
    if (glfwGetKey(window, 'Z') == GLFW_PRESS) {
        colorON = true;

    }
    if (glfwGetKey(window, 'X') == GLFW_PRESS) {
        colorON = false;
    }

    if (glfwGetKey(window, 'T') == GLFW_PRESS) {

    }
    if (glfwGetKey(window, 'Y') == GLFW_PRESS) {

    }
}
unsigned int World::loadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);

    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        //stbi_set_flip_vertically_on_load(true);
        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}




void World::Render()
{
  
    
    cam->get_position(camera_position);

    this->DoCollisions();

    //Ground
    ground.draw(cam->getMatrix(), projection, light_position, colorON, linesON);
   




    
    move = 1000- fabs(-1000 - fmod(8* glfwGetTime(),100.0));
  
   
 
    

    
 
}