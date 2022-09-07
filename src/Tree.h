
#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader.h"
#include "transform.h"


class Tree {

public:

    void init(GLuint _programID) {
        this->_programID = _programID;
        if (_programID == 0) exit(-1);

        glUseProgram(_programID);

        glGenVertexArrays(1, &_vao);
        glBindVertexArray(_vao);

        GLfloat indexed_position[] = { 
           -1.0f, -1.0f,  1.0f,
           1.0f, -1.0f,  1.0f,
           1.0f,  1.0f,  1.0f,
           -1.0f,  1.0f,  1.0f,
           -1.0f, -1.0f, -1.0f,
           1.0f, -1.0f, -1.0f,
           1.0f,  1.0f, -1.0f,
           -1.0f,  1.0f, -1.0f };

        GLuint index[] = { 
           0, 1, 2,//front
           0, 2, 3,
           1, 5, 6,//right
           1, 6, 2,
           5, 4, 7,//back
           5, 7, 6,
           4, 0, 3,//left
           4, 3, 7,
           3, 2, 6,//top
           3, 6, 7,
           1, 0, 4,//bottom
           1, 4, 5 };

        GLfloat position[36 * 3];

        static const GLfloat vertices[] =
        {
            -0.5f, -0.5f, -0.5f,
             0.5f, -0.5f, -0.5f,
             0.5f,  0.5f, -0.5f,
             0.5f,  0.5f, -0.5f,
            -0.5f,  0.5f, -0.5f,
            -0.5f, -0.5f, -0.5f,

            -0.5f, -0.5f,  0.5f,
             0.5f, -0.5f,  0.5f,
             0.5f,  0.5f,  0.5f,
             0.5f,  0.5f,  0.5f,
            -0.5f,  0.5f,  0.5f,
            -0.5f, -0.5f,  0.5f,

            -0.5f,  0.5f,  0.5f,
            -0.5f,  0.5f, -0.5f,
            -0.5f, -0.5f, -0.5f,
            -0.5f, -0.5f, -0.5f,
            -0.5f, -0.5f,  0.5f,
            -0.5f,  0.5f,  0.5f,

             0.5f,  0.5f,  0.5f,
             0.5f,  0.5f, -0.5f,
             0.5f, -0.5f, -0.5f,
             0.5f, -0.5f, -0.5f,
             0.5f, -0.5f,  0.5f,
             0.5f,  0.5f,  0.5f,

            -0.5f, -0.5f, -0.5f,
             0.5f, -0.5f, -0.5f,
             0.5f, -0.5f,  0.5f,
             0.5f, -0.5f,  0.5f,
            -0.5f, -0.5f,  0.5f,
            -0.5f, -0.5f, -0.5f,

            -0.5f,  0.5f, -0.5f,
             0.5f,  0.5f, -0.5f,
             0.5f,  0.5f,  0.5f,
             0.5f,  0.5f,  0.5f,
            -0.5f,  0.5f,  0.5f,
            -0.5f,  0.5f, -0.5f
        };
        //we need to generate positions one by one in order to have per surface vertex
        generate_positions(indexed_position, index, position);

     ;


        glGenBuffers(1, &_vbo_pos);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo_pos);
        glBufferData(GL_ARRAY_BUFFER, sizeof(position), position, GL_STATIC_DRAW);

        GLuint id_pos = glGetAttribLocation(_programID, "position");
        glEnableVertexAttribArray(id_pos);
        glVertexAttribPointer(id_pos, 3, GL_FLOAT, GL_FALSE, 0, NULL);


        _num_vertices = sizeof(position) / sizeof(GLfloat);
        _num_indices = sizeof(index) / sizeof(GLuint);

        //Default color
        color.x = 1.0;
        color.y = 0.0;
        color.z = 0.0;
        glBindVertexArray(0);
    }

    void draw(glm::mat4 model, glm::mat4 view, glm::mat4 projection) {
        glUseProgram(_programID);
        glBindVertexArray(_vao);
        glUniform4f(glGetUniformLocation(_programID, "color"), color.x, color.y, color.z, 1.0f);
        glUniformMatrix4fv(glGetUniformLocation(_programID, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(_programID, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(_programID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glDrawArrays(GL_TRIANGLES, 0, _num_vertices);
        glUseProgram(0);
        glBindVertexArray(0);
    }

    void set_color(float r, float g, float b) {
        color.x = r;
        color.y = g;
        color.z = b;
    }


    void draw() {
        glUseProgram(_programID);
        glBindVertexArray(_vao);
        draw(this->model_matrix, this->view_matrix, this->projection_matrix);
        glUseProgram(0);
        glBindVertexArray(0);
    }



    void draw(glm::mat4x4 view, glm::mat4x4 projection) 
    {
        this->draw(model_transf.get_matrix(), view, projection);
    }

   
    void apply_transformation(Transform transf) 
    {
        model_transf = transf;
     }



protected:

    Transform model_transf;

    GLuint _programID;
    GLuint _vao;
    GLuint _vbo_pos;
   

    glm::mat4x4 model_matrix;
    glm::mat4x4 view_matrix;
    glm::mat4x4 projection_matrix;
    GLfloat camera_position[3];

 
    unsigned int _num_indices;
    unsigned int _num_vertices;

    glm::vec3 color;

    //generate positions for tree
    void generate_positions(GLfloat indexed_position[24], GLuint index[36], GLfloat position[36 * 3]) {
        for (size_t i = 0; i < 36; i++) {
            position[i * 3 + 0] = indexed_position[index[i] * 3 + 0];
            position[i * 3 + 1] = indexed_position[index[i] * 3 + 1];
            position[i * 3 + 2] = indexed_position[index[i] * 3 + 2];
        }
    }
};