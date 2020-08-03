#version 430

layout(location = 0) in vec4 a_Position;
layout(location = 1) in vec2 a_TextureCoordinates;
layout(location = 2) in vec3 a_Normal;
uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

out vec2 v_TextureCoordinates;
out vec3 v_Normal;
out vec3 FragPos;

uniform vec3 lightPos;

void main()
{
    v_TextureCoordinates = a_TextureCoordinates;
	v_Normal = mat3(transpose(inverse(u_Model))) * a_Normal;
	FragPos = vec3(u_Model * a_Position);
    gl_Position = u_Projection * u_View * u_Model * a_Position;
}