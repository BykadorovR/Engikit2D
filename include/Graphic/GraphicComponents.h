#pragma once
#include <tuple>

#include "BufferManager.h"
#include "Component.h"
#include "Shader.h"
#include "GlyphsLoader.h"
#include "TextureAtlas.h"

class ObjectComponent : public OperationComponent {
private:
	std::tuple<float, float> _position;
	std::tuple<float, float> _size;
	std::shared_ptr<BufferManager> _bufferManager;
	std::shared_ptr<Shader> _shader;
	float _scale;
	float _visible;

public:
	ObjectComponent();
	bool initialize(std::tuple<float, float> position, std::tuple<float, float> size, std::shared_ptr<BufferManager> bufferManager, std::shared_ptr<Shader> shader);
	std::shared_ptr<Shader> getShader();
	bool setMember(std::string name, float value, int index = -1);
	std::shared_ptr<BufferManager> getBufferManager();
	std::tuple<float, float> getPosition();
	std::tuple<float, float> getSize();
};

class TextureComponent : public OperationComponent {
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
public:
	TextureComponent();
	bool initialize(std::shared_ptr<BufferManager> bufferManager);
	bool setTexture(int textureID);
	bool setColorMask(std::vector<float> colorMask);
	std::vector<float> getColorMask();
	bool setColorAddition(std::vector<float> colorAddition);
	std::vector<float> getColorAddition();
	int getTextureID();
	bool hasBindedTexture();
	std::shared_ptr<BufferManager> getBufferManager();
};

enum TextAllignment {
	LEFT = 0,
	CENTER = 1,
	RIGHT = 2
};

class TextComponent : public OperationComponent {
public:
	TextComponent();
	bool initialize(std::shared_ptr<BufferManager> bufferManager);
	bool setFocus(bool focus);
	bool getFocus();
	std::string getText();
	bool setText(std::string text);
	bool setScale(float scale);
	float getScale();
	bool setColor(std::vector<float> color);
	std::vector<float> getColor();
	bool setAllignment(std::tuple<TextAllignment, TextAllignment> allignment);
	std::tuple<TextAllignment, TextAllignment> getAllignment();
	bool setLineSpacingCoeff(float coeff);
	float getLineSpacingCoeff();
	std::shared_ptr<BufferManager> getBufferManager();
private:
	//if focus == true all keys will be handled as text for this TextComponent
	float _focus;
	float _editable;
	float _cursorPosition;
	std::string _text;
	float _scale;
	float _lineSpacingCoeff;
	std::vector<float> _color;
	std::shared_ptr<BufferManager> _bufferManager;
	std::tuple<TextAllignment, TextAllignment> _allignment;
};