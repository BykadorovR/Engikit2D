#include "Object.h"
#include <assert.h>
#include "Common.h"

Object::Object(int sceneX, int sceneY, int objectWidth, int objectHeight, Texture texture) {
	_sceneX = sceneX;
	_sceneY = sceneY;
	_objectWidth = objectWidth;
	_objectHeight = objectHeight;
	_texture = texture;
	_textureID = texture.getAtlas()->getAtlasID();
	_transform.identity();
}

void Object::setBuffer() {
	float spriteWidthN = (float)_objectWidth / (float)resolution.first;
	float spriteHeightN = (float)_objectHeight / (float)resolution.second;
	float startX = (float)(_sceneX) / (float)resolution.first;
	float startY = (float)(resolution.second - _sceneY) / (float)resolution.second;
	float posXInAtlasN = (float)_texture.getX() / (float)_texture.getAtlas()->getWidth();
	float posYInAtlasN = (float)_texture.getY() / (float)_texture.getAtlas()->getHeight();
	float textureWidthN = (float)_texture.getWidth() / (float)_texture.getAtlas()->getWidth();
	float textureHeightN = (float)_texture.getHeight() / (float)_texture.getAtlas()->getHeight();
	// Order of coordinates: X, Y, S, T
	// 0   2
	// | / |
	// 1   3
	float vertexData[] = {   startX,                startY,                 posXInAtlasN,                 posYInAtlasN,
							 startX,                startY - spriteHeightN, posXInAtlasN,                 posYInAtlasN + textureHeightN,
							 startX + spriteWidthN, startY,                 posXInAtlasN + textureWidthN, posYInAtlasN,
							 startX + spriteWidthN, startY - spriteHeightN, posXInAtlasN + textureWidthN, posYInAtlasN + textureHeightN };
	assert(_buffer.bindVBO(vertexData, sizeof(vertexData), GL_STATIC_DRAW) == TW_OK);

}

void Object::translate(int posX, int posY) {
	//change Object scene coords
	_sceneX += posX;
	_sceneY += posY;
	//change Object GL coords via matrix in vertex shader
	Matrix2D translate;
	translate.translate(posX, posY);
	_transform = _transform * translate;
}

void Object::attach() {
	setBuffer();
	_program = _shader.buildProgramFromAsset("../data/shaders/shader.vsh", "../data/shaders/shader.fsh");
	_aPositionLocation = glGetAttribLocation(_program, _aPositionString.c_str());
	_aTextureCoordinatesLocation = glGetAttribLocation(_program, _aTextureCoordinatesString.c_str());
	_uTextureUnitLocation = glGetUniformLocation(_program, _uTextureUnitString.c_str());
	_uMatrixLocation = glGetUniformLocation(_program, _uMatrix.c_str());
}

void Object::draw() {
	glUseProgram(_program);
	
	//bind buffer and handle vertex shader
	glBindBuffer(GL_ARRAY_BUFFER, _buffer.getVBOObject());
	//index, size, type, normalized, stride, offset in GL_ARRAY_BUFFER target
	glVertexAttribPointer(_aPositionLocation, POSITION_COMPONENT_COUNT, GL_FLOAT, GL_FALSE, STRIDE, 0);
	glVertexAttribPointer(_aTextureCoordinatesLocation, TEXTURE_COORDINATES_COMPONENT_COUNT, GL_FLOAT, GL_FALSE, STRIDE, (void*)(TEXTURE_COORDINATES_COMPONENT_COUNT * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(_aPositionLocation);
	glEnableVertexAttribArray(_aTextureCoordinatesLocation);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	//bind texture and handle fragment shader
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _textureID);
	glUniform1i(_uTextureUnitLocation, 0);
	glUniformMatrix4fv(_uMatrixLocation, 1, false, _transform.getData());

}

int Object::getX() {
	return _sceneX;
}

int Object::getY() {
	return _sceneY;
}

int Object::getWidth() {
	return _objectWidth;
}

int Object::getHeight() {
	return _objectHeight;
}