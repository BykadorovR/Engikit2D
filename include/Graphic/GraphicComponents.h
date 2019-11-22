#pragma once
#include <tuple>

#include "BufferManager.h"
#include "Component.h"

class ObjectComponent : Component {
private:
	float _cameraMultiplier;
	float _scale;
	std::tuple<float, float> _position;
	std::tuple<float, float> _size;

public:
	ObjectComponent();
	bool initialize(std::tuple<float, float> position, std::tuple<float, float> size, std::shared_ptr<BufferManager> bufferManager);
};

class TextureComponent : Component {
private:
	int _textureID;
	std::tuple<float, float> _tileSize;
	int _currentTile;
	int _currentLatency;
	std::vector<float> _tilesOrder;
	std::vector<float> _tilesLatency;
	bool _invisible;
public:
	TextureComponent();
	bool initialize(int textureID);
};