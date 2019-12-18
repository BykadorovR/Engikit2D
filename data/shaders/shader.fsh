#version 430

precision mediump float;
//bind sampler2D to specific texture unit = GL_TEXTURE0. Can be changed via code
//glActiveTexture(GL_TEXTURE0 + 1);
//glBindTexture(GL_TEXTURE_2D, id);
layout(binding=0) uniform sampler2D u_TextureUnit;
in vec2 v_TextureCoordinates;
layout(location = 2) uniform float u_AdjustX = 0.0;
layout(location = 3) uniform float u_AdjustY = 0.0;
layout(location = 4) uniform vec4 color_mask;
layout(location = 5) uniform vec4 color_addition;
out vec4 color;

void main()
{
    color = texture2D(u_TextureUnit, vec2(v_TextureCoordinates.x+u_AdjustX, v_TextureCoordinates.y+u_AdjustY));
	color *= color_mask;
	color += color_addition;
}