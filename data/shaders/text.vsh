#version 430
layout(location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>
out vec2 TexCoords;

void main()
{
	gl_Position = vec4(vertex.xy, 0.0, 1.0);
	gl_Position.x = gl_Position.x * 2 - 1;
	gl_Position.y = gl_Position.y * 2 - 1;
	TexCoords = vertex.zw;
}