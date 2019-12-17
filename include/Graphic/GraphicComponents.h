#pragma once
#include <tuple>

#include "BufferManager.h"
#include "Component.h"
#include "Shader.h"
#include "GlyphsLoader.h"
#include "TextureAtlas.h"

class ObjectComponent : public Component {
private:
	float _cameraMultiplier;
	float _scale;
	std::tuple<float, float> _position;
	std::tuple<float, float> _size;
	std::shared_ptr<BufferManager> _bufferManager;
	std::shared_ptr<Shader> _shader;

public:
	ObjectComponent();
	bool initialize(std::tuple<float, float> position, std::tuple<float, float> size, std::shared_ptr<BufferManager> bufferManager, std::shared_ptr<Shader> shader);
	std::shared_ptr<Shader> getShader();
	std::shared_ptr<BufferManager> getBufferManager();
	std::tuple<float, float> getPosition();
	std::tuple<float, float> getSize();
};

class TextureComponent : public Component {
private:
	int _textureID;
	std::tuple<float, float> _tileSize;
	int _currentTile;
	int _currentLatency;
	std::vector<float> _tilesOrder;
	std::vector<float> _tilesLatency;
	std::shared_ptr<BufferManager> _bufferManager;
	std::vector<float> _colorMask;
	std::vector<float> _colorAddition;
	bool _invisible;
public:
	TextureComponent();
	bool initialize(int textureID, std::shared_ptr<BufferManager> bufferManager);
	bool setColorMask(std::vector<float> colorMask);
	std::vector<float> getColorMask();
	bool setColorAddition(std::vector<float> colorAddition);
	std::vector<float> getColorAddition();
	int getTextureID();
	std::shared_ptr<BufferManager> getBufferManager();
};

enum TextComponentType {
	LABEL = 0,
	EDIT = 1
};

class TextComponent : public Component {
public:
	TextComponent();
	bool initialize(TextComponentType type, std::wstring text, float scale, std::vector<float> color, 
					std::shared_ptr<GlyphsLoader> glyphsLoader, std::shared_ptr<BufferManager> bufferManager);
	bool setFocus(bool focus);
	bool getFocus();
	std::wstring getText();
	std::shared_ptr<GlyphsLoader> getLoader();
	float getScale();
	std::vector<float> getColor();
	std::shared_ptr<BufferManager> getBufferManager();
private:
	//if focus == true all keys will be handled as text for this TextComponent
	bool _focus;
	int _page;
	std::wstring _text;
	float _scale;
	std::vector<float> _color;
	std::shared_ptr<GlyphsLoader> _glyphsLoader;
	TextComponentType _type;
	std::shared_ptr<BufferManager> _bufferManager;
};