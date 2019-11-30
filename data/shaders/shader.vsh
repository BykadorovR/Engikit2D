#version 420

uniform mat4 u_Matrix = mat4(1.0);
uniform mat4 u_ViewMatrix = mat4(1.0);
layout(location = 0) in vec4 a_Position;
layout(location = 1) in vec2 a_TextureCoordinates;
out vec2 v_TextureCoordinates;
void main()
{
    v_TextureCoordinates = a_TextureCoordinates;
//	mat4 u_resultMatrix = u_Matrix * u_ViewMatrix;
	gl_Position = a_Position;// *u_resultMatrix;
	gl_Position.x = gl_Position.x * 2 - 1;
	gl_Position.y = gl_Position.y * 2 - 1;
}