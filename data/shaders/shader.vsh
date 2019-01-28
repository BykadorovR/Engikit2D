uniform mat4 u_Matrix;
attribute vec4 a_Position;
attribute vec2 a_TextureCoordinates;
varying vec2 v_TextureCoordinates;
void main()
{
    v_TextureCoordinates = a_TextureCoordinates;
    gl_Position = a_Position * u_Matrix;
	gl_Position.x = gl_Position.x * 2 - 1;
	gl_Position.y = gl_Position.y * 2 - 1;
}