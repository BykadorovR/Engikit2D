#include "Sprite.h"
#include <assert.h>
#include "Common.h"

Sprite::Sprite(float _sceneX, float _sceneY, float _spriteWidth, float _spriteHeight, Texture _texture) {
	sceneX = _sceneX;
	sceneY = _sceneY;
	spriteWidth = _spriteWidth;
	spriteHeight = _spriteHeight;
	texture = _texture;
	textureID = _texture.loadTexture();
}

void Sprite::initializeBuffer() {
	float vertexData[] =   { -1.0f, -1.0f, 0.0f, 0.0f,
							 -1.0f,  1.0f, 0.0f, 1.0f,
							  1.0f, -1.0f, 1.0f, 0.0f,
							  1.0f,  1.0f, 1.0f, 1.0f };
	assert(textureBuffer.bindVBO((GLvoid*)vertexData, sizeof(vertexData), GL_STATIC_DRAW) == TW_OK);
	
}

void Sprite::attach() {
	initializeBuffer();
	program = shader.buildProgramFromAsset("../data/shaders/shader.vsh", "../data/shaders/shader.fsh");
	aPositionLocation = glGetAttribLocation(program, aPositionString.c_str());
	aTextureCoordinatesLocation = glGetAttribLocation(program, aTextureCoordinatesString.c_str());
	uTextureUnitLocation = glGetUniformLocation(program, uTextureUnitString.c_str());
}

void Sprite::draw() {
	glUseProgram(program);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glUniform1i(uTextureUnitLocation, 0);

	glBindBuffer(GL_ARRAY_BUFFER, textureBuffer.getVBOObject());
	glVertexAttribPointer(aPositionLocation, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GL_FLOAT), 0);
	glVertexAttribPointer(aTextureCoordinatesLocation, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GL_FLOAT), (void*)(2 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(aPositionLocation);
	glEnableVertexAttribArray(aTextureCoordinatesLocation);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}