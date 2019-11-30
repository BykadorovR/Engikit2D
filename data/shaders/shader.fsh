#version 430

precision mediump float;

//bind sampler2D to specific texture unit = GL_TEXTURE0. Can be changed via code
//glActiveTexture(GL_TEXTURE0 + 1);
//glBindTexture(GL_TEXTURE_2D, id);
layout(binding=0) uniform sampler2D u_TextureUnit;
in vec2 v_TextureCoordinates;
layout(location = 3) uniform float u_AdjustX = 0.0;
layout(location = 4) uniform float u_AdjustY = 0.0;

void main()
{
    gl_FragColor = texture2D(u_TextureUnit, vec2(v_TextureCoordinates.x+u_AdjustX, v_TextureCoordinates.y+u_AdjustY));
}