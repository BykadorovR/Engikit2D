#include "Sprite.h"
#include <assert.h>
#include "Common.h"

Sprite::Sprite(int sceneX, int sceneY, int spriteWidth, int spriteHeight, Texture texture) : Object(sceneX, sceneY, spriteWidth, spriteHeight, texture) {
}

AnimatedSprite::AnimatedSprite(int sceneX, int sceneY, int spriteWidth, int spriteHeight, Texture texture) : Object(sceneX, sceneY, spriteWidth, spriteHeight, texture) {
}

void AnimatedSprite::setBuffer() {
	float spriteWidthN = (float)_objectWidth / (float)resolution.first;
	float spriteHeightN = (float)_objectHeight / (float)resolution.second;
	float startX = (float)(_sceneX) / (float)resolution.first;
	float startY = (float)(resolution.second - _sceneY) / (float)resolution.second;
	float posXInAtlasN = (float)_texture.getX() / (float)_texture.getAtlas()->getWidth();
	float posYInAtlasN = (float)_texture.getY() / (float)_texture.getAtlas()->getHeight();
	float widthTile = (float)_texture.getWidth() / (float) _texture.getColumn() / (float)_texture.getAtlas()->getWidth();
	_widthTile = widthTile;
	float heightTile = (float)_texture.getHeight() / (float) _texture.getRow() / (float)_texture.getAtlas()->getHeight();
	_heightTile = heightTile;
	// Order of coordinates: X, Y, S, T
	// 0   2
	// | / |
	// 1   3
	float vertexData[] = {   startX,                startY,                 posXInAtlasN,             posYInAtlasN,
							 startX,                startY - spriteHeightN, posXInAtlasN,             posYInAtlasN + heightTile,
							 startX + spriteWidthN, startY,                 posXInAtlasN + widthTile, posYInAtlasN,
							 startX + spriteWidthN, startY - spriteHeightN, posXInAtlasN + widthTile, posYInAtlasN + heightTile };
	assert(_buffer.bindVBO(vertexData, sizeof(vertexData), GL_STATIC_DRAW) == TW_OK);
}

void AnimatedSprite::setAnimate(std::vector<int> tilesOrder, std::vector<int> tilesLatency) {
	_tilesLatency = tilesLatency;
	_tilesOrder = tilesOrder;
}

int AnimatedSprite::getCurrentAnimateTile() {
	return _tilesOrder[_currentAnimateTile];
}

void AnimatedSprite::attach() {
	Object::attach();
	_uAdjustXLocation = glGetUniformLocation(_program, _uAdjustX.c_str());
	_uAdjustYLocation = glGetUniformLocation(_program, _uAdjustY.c_str());
}

void AnimatedSprite::draw() {
	Object::draw();
	glUniform1f(_uAdjustXLocation, _widthTile * _tilesOrder[(_currentAnimateTile)] + 1);
	if (_currentAnimateTile < _tilesOrder.size()) {
		if (_currentLatency < _tilesLatency[_currentAnimateTile])
			_currentLatency++;
		else {
			_currentLatency = 0;
			_currentAnimateTile++;
			if (_currentAnimateTile == _tilesOrder.size())
				_currentAnimateTile = 0;
		}
	}
}