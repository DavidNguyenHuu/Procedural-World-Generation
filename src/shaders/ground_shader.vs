#version 330 core

in vec3 aPosition;
in vec3 aNormal;
in vec2 aTexCoords;



uniform vec3  camera_position;

uniform mat4 u_light_space_matrix;
uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model;

out vec3 frag_position;
out vec3 normal;
out vec2 text_coordinate;


out float height_temp;

out vec2 TexCoords;

void main()
{
frag_position = vec3(u_model * vec4(aPosition, 1.0));

normal = transpose(inverse(mat3(u_model))) * aNormal;

text_coordinate = aTexCoords;

gl_Position = u_projection * u_view * vec4(frag_position, 1.0);

height_temp = aPosition[1];

}