#include "Sprite.h"
#include <assert.h>
#include "Common.h"

Sprite::Sprite(int _sceneX, int _sceneY, int _spriteWidth, int _spriteHeight, Texture _texture) {
	sceneX = _sceneX;
	sceneY = _sceneY;
	spriteWidth = _spriteWidth;
	spriteHeight = _spriteHeight;
	texture = _texture;
	textureID = _texture.getAtlas()->getAtlasID();
	transform.identity();
}

void Sprite::initializeBuffer() {
	//due to we shift position by one direction, so we should multiply to 2
	//x = 100 , screenWidth = 200 - 1/2 but gl screen = [-1, 1] and we translate to 50, not 100
	float startX = (float) (2 * sceneX) / (float)resolution.first;
	float startY = (float) (2 * sceneY) / (float)resolution.second;
	float spriteWidthN = (float) spriteWidth / (float)resolution.first;
	float spriteHeightN = (float) spriteHeight / (float)resolution.second;
	float posXInAtlasN = (float) texture.getX() / (float)texture.getAtlas()->getWidth();
	float posYInAtlasN = (float) texture.getY() / (float)texture.getAtlas()->getHeight();
	float textureWidthN = (float) texture.getWidth() / (float)texture.getAtlas()->getWidth();
	float textureHeightN = (float)texture.getHeight() / (float)texture.getAtlas()->getHeight();
	// Order of coordinates: X, Y, S, T
	float vertexData[] =   { -spriteWidthN + startX, spriteHeightN + startY, posXInAtlasN, posYInAtlasN + textureHeightN,
							 -spriteWidthN + startX, -spriteHeightN + startY, posXInAtlasN, posYInAtlasN,
							  spriteWidthN + startX, spriteHeightN + startY, posXInAtlasN + textureWidthN, posYInAtlasN + textureHeightN,
							  spriteWidthN + startX, -spriteHeightN + startY, posXInAtlasN + textureWidthN, posYInAtlasN };
	assert(spriteBuffer.bindVBO(vertexData, sizeof(vertexData), GL_STATIC_DRAW) == TW_OK);
	
}

void Sprite::translate(int posX, int posY) {
	Matrix2D translate;
	translate.translate(posX, posY);
	transform = transform * translate;
}

void Sprite::attach() {
	initializeBuffer();
	program = shader.buildProgramFromAsset("../data/shaders/shader.vsh", "../data/shaders/shader.fsh");
	aPositionLocation = glGetAttribLocation(program, aPositionString.c_str());
	aTextureCoordinatesLocation = glGetAttribLocation(program, aTextureCoordinatesString.c_str());
	uTextureUnitLocation = glGetUniformLocation(program, uTextureUnitString.c_str());
	uMatrixLocation = glGetUniformLocation(program, uMatrix.c_str());
}

void Sprite::draw() {
	glUseProgram(program);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glUniform1i(uTextureUnitLocation, 0);

	glUniformMatrix4fv(uMatrixLocation, 1, false, transform.getData());
	glBindBuffer(GL_ARRAY_BUFFER, spriteBuffer.getVBOObject());
	//index, size, type, normalized, stride, offset in GL_ARRAY_BUFFER target
	glVertexAttribPointer(aPositionLocation, POSITION_COMPONENT_COUNT, GL_FLOAT, GL_FALSE, STRIDE, 0);
	glVertexAttribPointer(aTextureCoordinatesLocation, TEXTURE_COORDINATES_COMPONENT_COUNT, GL_FLOAT, GL_FALSE, STRIDE, (void*)(TEXTURE_COORDINATES_COMPONENT_COUNT * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(aPositionLocation);
	glEnableVertexAttribArray(aTextureCoordinatesLocation);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}