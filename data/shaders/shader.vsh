#version 430

layout(location = 0) in vec4 a_Position;
layout(location = 1) in vec2 a_TextureCoordinates;
out vec2 v_TextureCoordinates;
void main()
{
    v_TextureCoordinates = a_TextureCoordinates;
	gl_Position = a_Position;
	gl_Position.x = gl_Position.x * 2 - 1;
	gl_Position.y = gl_Position.y * 2 - 1;
}