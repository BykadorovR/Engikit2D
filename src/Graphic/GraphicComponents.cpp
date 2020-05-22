#include "GraphicComponents.h"
#include "TextureManager.h"
#include "State.h"
#include "Common.h"
#include <locale>
#include <codecvt>
#include <algorithm>

ObjectComponent::ObjectComponent() {
	_componentName = "ObjectComponent";
	_classVariablesFloat =
	{
		{"positionX", &std::get<0>(_position)},
		{"positionY", &std::get<1>(_position)},
		{"sizeX", &std::get<0>(_size)},
		{"sizeY", &std::get<1>(_size)},
		{"scale", &_scale},
		{"visible", &_visible}
	};
}

bool ObjectComponent::initialize(std::tuple<float, float> position, std::tuple<float, float> size, std::shared_ptr<BufferManager> bufferManager, std::shared_ptr<Shader> shader) {
	_position = position;
	_size = size;
	_bufferManager = bufferManager;
	_visible = true;
	_scale = 1;

	_bufferManager->addBuffer(BufferType::Position, _position, _size, resolution);
	_shader = shader;
	return false;
}

bool ObjectComponent::setMember(std::string name, float value, int index) {
	OperationComponent::setMember(name, value, index);
	getBufferManager()->changeBuffer(BufferType::Position, getPosition(), getSize(), resolution);
	return false;
}

std::shared_ptr<Shader> ObjectComponent::getShader() {
	return _shader;
}

std::shared_ptr<BufferManager> ObjectComponent::getBufferManager() {
	return _bufferManager;
}

std::tuple<float, float> ObjectComponent::getPosition() {
	return _position;
}

std::tuple<float, float> ObjectComponent::getSize() {
	return _size;
}

TextureComponent::TextureComponent() {
	_componentName = "TextureComponent";
}

bool TextureComponent::setColorMask(std::vector<float> colorMask) {
	_colorMask = colorMask;
	return false;
}

std::vector<float> TextureComponent::getColorMask() {
	return _colorMask;
}

bool TextureComponent::setColorAddition(std::vector<float> colorAddition) {
	_colorAddition = colorAddition;
	return false;
}

std::vector<float> TextureComponent::getColorAddition() {
	return _colorAddition;
}

bool TextureComponent::initialize(std::shared_ptr<BufferManager> bufferManager) {
	_textureID = -1;
	_bufferManager = bufferManager;
	return false;
}

bool TextureComponent::setTexture(int textureID) {
	_textureID = textureID;
	auto textureAtlas = TextureManager::instance()->getTextureAtlas(textureID);
	auto textureInfo = textureAtlas->getTexture(textureID);
	auto texture = std::get<0>(textureInfo);
	auto texturePosition = std::get<1>(textureInfo);
	//pos in atlas, tile size
	_bufferManager->addBuffer(BufferType::Texture, texturePosition, texture->getRealImageSize(), textureAtlas->getSize());
	_colorMask = { 1.0f, 1.0f, 1.0f, 1.0f };
	_colorAddition = { 0.0f, 0.0f, 0.0f, 0.0f };
	return false;
}

bool TextureComponent::hasBindedTexture() {
	if (_textureID < 0)
		return false;
	return true;
}

int TextureComponent::getTextureID() {
	return _textureID;
}

std::shared_ptr<BufferManager> TextureComponent::getBufferManager() {
	return _bufferManager;
}

Symbol::Symbol() {

}

Symbol::Symbol(std::tuple<wchar_t, CharacterInfo> params) {
	_text = std::get<0>(params);
	_chInfo = std::get<1>(params);
}

CharacterInfo Symbol::getCharacterInfo() {
	return _chInfo;
}

wchar_t Symbol::getText() {
	return _text;
}

Word::Word() {
}

std::wstring Word::getText() {
	std::wstring text;
	for (auto symbol : _text) {
		text += symbol.getText();
	}
	return text;
}

int Word::getSize() {
	return _text.size();
}

int Word::getWidth() {
	int size = 0;
	for (auto symbol : _text) {
		size += symbol.getCharacterInfo()._advance >> 6;
	}
	return size;
}

int Word::getHeight() {
	int size = 0;
	for (auto symbol : _text) {
		size = std::max(std::get<1>(symbol.getCharacterInfo()._size), size);
	}
	return size;
}

int Word::getHeightAdjusted(int bearingYMax) {
	int size = 0;
	for (auto symbol : _text) {
		size = std::max(std::get<1>(symbol.getCharacterInfo()._size) + bearingYMax - std::get<1>(symbol.getCharacterInfo()._bearing), size);
	}
	return size;
}

bool Word::clear() {
	_text.clear();
	return false;
}

bool Word::cropTrailingSpace() {
	if (_text.back().getText() == wordsDelimiter)
		_text.pop_back();
	return false;
}

int Word::getBearingYMin() {
	int size = INT_MAX;
	for (auto symbol : _text) {
		size = std::min(std::get<1>(symbol.getCharacterInfo()._bearing), size);
	}
	return size;
}

int Word::getBearingYMax() {
	int size = INT_MIN;
	for (auto symbol : _text) {
		size = std::max(std::get<1>(symbol.getCharacterInfo()._bearing), size);
	}
	return size;
}

std::wstring Word::operator+=(std::tuple<wchar_t, CharacterInfo> rhs) {
	_text.push_back(rhs);
	return getText();
}

Line::Line() {

}

bool Line::addWord(Word word) {
	_text.push_back(word);
	return false;
}

int Line::getWidth() {
	int width = 0;
	for (auto word : _text) {
		width += word.getWidth();
	}
	return width;
}

int Line::getHeight() {
	int height = 0;
	for (auto word : _text) {
		height = std::max(word.getHeight(), height);
	}
	return height;
}

int Line::getHeightAdjusted(int bearingYMax) {
	int height = 0;
	for (auto word : _text) {
		height = std::max(word.getHeightAdjusted(bearingYMax), height);
	}
	return height;
}

int Line::getBearingYMin() {
	int size = INT_MAX;
	for (auto symbol : _text) {
		size = std::min(symbol.getBearingYMin(), size);
	}
	return size;
}

int Line::getBearingYMax() {
	int size = INT_MIN;
	for (auto symbol : _text) {
		size = std::max(symbol.getBearingYMax(), size);
	}
	return size;
}

int Line::getSize() {
	int size = 0;
	for (auto word : _text) {
		size += word.getSize();
	}
	return size;
}

std::vector<Word>& Line::getText() {
	return _text;
}

TextComponent::TextComponent() {
	_focus = 0;
	_page = 0;
	_lineHeight = 0;
	_totalPages = 1;
	_editable = 0;
	_prevTextSize = 0;

	_classVariablesFloat =
	{
		{"page", &_page},
		{"totalPages", &_totalPages},
		{"focus", &_focus},
		{"lineHeight", &_lineHeight},
		{"spacingCoeff", &_lineSpacingCoeff},
		{"scale", &_scale},
		{"editable", &_editable},
		{"allignBearingYMax", &_allignBearingYMax},
		{"verticalScrollerPosition", &_verticalScrollerPosition},
		{"horizontalScrollerPosition", &_horizontalScrollerPosition},
	};
	
	_classVariablesString =
	{
		{"text", &_text}
	};

	_componentName = "TextComponent";
	
}

bool TextComponent::initialize(std::shared_ptr<BufferManager> bufferManager) {
	_scale = 1.f;
	_color = {1.f, 1.f, 1.f, 1.f};
	_lineSpacingCoeff = 1;
	_page = 0;
	_allignment = {TextAllignment::LEFT, TextAllignment::LEFT};
	_bufferManager = bufferManager;
	_horizontalScrollerPosition = 0;
	_verticalScrollerPosition = 0;

	//Texture buffer with temporal values, should be changed in runtime
	_bufferManager->addBuffer(BufferType::Texture, { 0, 0 }, { 0, 0 }, GlyphsLoader::instance().getAtlas()->getSize());
	return false;
}

std::shared_ptr<BufferManager> TextComponent::getBufferManager() {
	return _bufferManager;
}

bool TextComponent::setText(std::string text) {
	for (int i = 0; i < text.size(); i++) {
		if (text[i] == '\n') {
			_verticalScrollerPosition += 1;
			_horizontalScrollerPosition = 0;
		}
		else
			_horizontalScrollerPosition += 1;
	}
	//All strings are stored as UTF8, so first we should convert it
	std::string UTF8String = convertMultibyteToUTF8(text);
	_text = UTF8String;
	return false;

}

std::string TextComponent::getText() {
	return _text;
}

int TextComponent::getPrevTextSize() {
	return _prevTextSize;
}
bool TextComponent::setPrevTextSize(int size) {
	_prevTextSize = size;
	return false;
}

bool TextComponent::setScale(float scale) {
	_scale = scale;
	return false;
}

float TextComponent::getScale() {
	return _scale;
}

bool TextComponent::setLines(std::vector<Line> lines) {
	_lines = lines;
	return false;
}

std::vector<Line> TextComponent::getLines() {
	return _lines;
}

bool TextComponent::setPageNumber(int page) {
	_page = page;
	return false;
}

int TextComponent::getPageNumber() {
	return _page;
}

bool TextComponent::setColor(std::vector<float> color) {
	_color = color;
	return false;
}

std::vector<float> TextComponent::getColor() {
	return _color;
}

bool TextComponent::setFocus(bool focus) {
	return false;
}

bool TextComponent::getFocus() {
	return _focus;
}

bool TextComponent::setAllignment(std::tuple<TextAllignment, TextAllignment> allignment) {
	_allignment = allignment;
	return false;
}

std::tuple<TextAllignment, TextAllignment> TextComponent::getAllignment() {
	return _allignment;
}

bool TextComponent::setLineSpacingCoeff(float coeff) {
	_lineSpacingCoeff = coeff;
	return false;
}

float TextComponent::getLineSpacingCoeff() {
	return _lineSpacingCoeff;
}