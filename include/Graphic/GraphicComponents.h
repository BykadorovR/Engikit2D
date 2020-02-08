#pragma once
#include <tuple>

#include "BufferManager.h"
#include "Component.h"
#include "Shader.h"
#include "GlyphsLoader.h"
#include "TextureAtlas.h"

class ObjectComponent : public Component {
private:
	std::tuple<float, float> _position;
	std::tuple<float, float> _size;
	std::shared_ptr<BufferManager> _bufferManager;
	std::shared_ptr<Shader> _shader;
	float _scale;

public:
	ObjectComponent();
	bool initialize(std::tuple<float, float> position, std::tuple<float, float> size, std::shared_ptr<BufferManager> bufferManager, std::shared_ptr<Shader> shader);
	std::shared_ptr<Shader> getShader();
	bool setMember(std::string name, float value);
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
	bool initialize(std::shared_ptr<BufferManager> bufferManager);
	bool initialize(int textureID, std::shared_ptr<BufferManager> bufferManager);
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


class Symbol {
public:
	Symbol();
	Symbol(std::tuple<wchar_t, CharacterInfo> params);
	CharacterInfo getCharacterInfo();
	wchar_t getText();
private:
	CharacterInfo _chInfo;
	wchar_t _text;
};

class Word {
public:
	Word();
	std::wstring getText();
	int getWidth();
	int getHeight();
	//bearingMax - bearingCurrent + height so we get size of char on screen with bearing padding
	int getHeightAdjusted(int bearingYMax);
	bool clear();
	bool cropTrailingSpace();
	int getBearingYMin();
	int getBearingYMax();
	std::wstring operator+=(std::tuple<wchar_t, CharacterInfo> rhs);
private:
	std::vector<Symbol> _text;
};

class Line {
public:
	Line();

	bool addWord(Word word);
	int getWidth();
	int getHeight();
	int getHeightAdjusted(int bearingYMax);
	int getBearingYMin();
	int getBearingYMax();
	std::vector<Word>& getText();
private:
	std::vector<Word> _text;
};


class TextComponent : public Component {
public:
	TextComponent();
	bool initialize(std::wstring text, std::shared_ptr<GlyphsLoader> glyphsLoader, std::shared_ptr<BufferManager> bufferManager);
	bool setFocus(bool focus);
	bool getFocus();
	std::wstring getText();
	std::shared_ptr<GlyphsLoader> getLoader();
	bool setScale(float scale);
	float getScale();
	bool setColor(std::vector<float> color);
	std::vector<float> getColor();
	bool setPageNumber(int page);
	int getPageNumber();
	bool setAllignment(std::tuple<TextAllignment, TextAllignment> allignment);
	std::tuple<TextAllignment, TextAllignment> getAllignment();
	bool setLineSpacingCoeff(float coeff);
	float getLineSpacingCoeff();
	std::shared_ptr<BufferManager> getBufferManager();
private:
	//if focus == true all keys will be handled as text for this TextComponent
	bool _focus;
	float _page;
	float _totalPages;
	std::wstring _text;
	float _scale;
	float _lineSpacingCoeff;
	std::vector<float> _color;
	std::shared_ptr<GlyphsLoader> _glyphsLoader;
	std::shared_ptr<BufferManager> _bufferManager;
	std::tuple<TextAllignment, TextAllignment> _allignment;
};