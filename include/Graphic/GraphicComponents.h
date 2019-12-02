#pragma once
#include <tuple>

#include "BufferManager.h"
#include "Component.h"
#include "Shader.h"

class ObjectComponent : public Component {
private:
	float _cameraMultiplier;
	float _scale;
	std::tuple<float, float> _position;
	std::tuple<float, float> _size;
	std::shared_ptr<Buffer> _buffer;
	std::shared_ptr<Shader> _shader;

public:
	ObjectComponent();
	bool initialize(std::tuple<float, float> position, std::tuple<float, float> size, std::shared_ptr<BufferManager> bufferManager);
	std::shared_ptr<Shader> getShader();
	std::shared_ptr<Buffer> getBuffer();
};

class TextureComponent : public Component {
private:
	int _textureID;
	std::tuple<float, float> _tileSize;
	int _currentTile;
	int _currentLatency;
	std::vector<float> _tilesOrder;
	std::vector<float> _tilesLatency;
	std::shared_ptr<Buffer> _buffer;
	bool _invisible;
public:
	TextureComponent();
	bool initialize(int textureID, std::shared_ptr<BufferManager> bufferManager);
	int getTextureID();
	std::shared_ptr<Buffer> getBuffer();
};