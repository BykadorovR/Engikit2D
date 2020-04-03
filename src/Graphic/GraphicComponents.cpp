#include "GraphicComponents.h"
#include "TextureManager.h"
#include "State.h"
#include "Common.h"
#include <locale>
#include <codecvt>

ObjectComponent::ObjectComponent() {
	_componentName = "ObjectComponent";
	_classVariablesFloat =
	{
		{"positionX", &std::get<0>(_position)},
		{"positionY", &std::get<1>(_position)},
		{"sizeX", &std::get<0>(_size)},
		{"sizeY", &std::get<1>(_size)},
		{"scale", &_scale}
	};
}

bool ObjectComponent::initialize(std::tuple<float, float> position, std::tuple<float, float> size, std::shared_ptr<BufferManager> bufferManager, std::shared_ptr<Shader> shader) {
	_position = position;
	_size = size;
	_bufferManager = bufferManager;

	_bufferManager->addBuffer(BufferType::Position, _position, _size, resolution);
	_shader = shader;
	return false;
}

bool ObjectComponent::setMember(std::string name, float value) {
	Component::setMember(name, value);
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
	_colorMask = { 1.0f, 1.0f, 1.0f, 1.0f };
	_colorAddition = { 0.0f, 0.0f, 0.0f, 0.0f };
	return false;
}

bool TextureComponent::initialize(int textureID, std::shared_ptr<BufferManager> bufferManager) {
	_textureID = textureID;
	_bufferManager = bufferManager;
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
		size = max(std::get<1>(symbol.getCharacterInfo()._size), size);
	}
	return size;
}

int Word::getHeightAdjusted(int bearingYMax) {
	int size = 0;
	for (auto symbol : _text) {
		size = max(std::get<1>(symbol.getCharacterInfo()._size) + bearingYMax - std::get<1>(symbol.getCharacterInfo()._bearing), size);
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
		size = min(std::get<1>(symbol.getCharacterInfo()._bearing), size);
	}
	return size;
}

int Word::getBearingYMax() {
	int size = INT_MIN;
	for (auto symbol : _text) {
		size = max(std::get<1>(symbol.getCharacterInfo()._bearing), size);
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
		height = max(word.getHeight(), height);
	}
	return height;
}

int Line::getHeightAdjusted(int bearingYMax) {
	int height = 0;
	for (auto word : _text) {
		height = max(word.getHeightAdjusted(bearingYMax), height);
	}
	return height;
}

int Line::getBearingYMin() {
	int size = INT_MAX;
	for (auto symbol : _text) {
		size = min(symbol.getBearingYMin(), size);
	}
	return size;
}

int Line::getBearingYMax() {
	int size = INT_MIN;
	for (auto symbol : _text) {
		size = max(symbol.getBearingYMax(), size);
	}
	return size;
}

std::vector<Word>& Line::getText() {
	return _text;
}

TextComponent::TextComponent() {
	_focus = false;
	_page = 0;
	_classVariablesFloat =
	{
		{"page", &_page},
		{"totalPages", &_totalPages},
		{"focus", &_focus}
	};
	
	_classVariablesString =
	{
		{"text", &_text}
	};
	
}

bool TextComponent::initialize(std::string text, std::shared_ptr<GlyphsLoader> glyphsLoader, std::shared_ptr<BufferManager> bufferManager) {
	_scale = 1.f;
	_color = {1.f, 1.f, 1.f, 1.f};
	_lineSpacingCoeff = 1;
	_page = 0;
	_allignment = {TextAllignment::LEFT, TextAllignment::LEFT};
	//All strings are stored as UTF8, so first we should convert it
	std::string UTF8String = convertMultibyteToUTF8(text);

	_text = UTF8String;
	_glyphsLoader = glyphsLoader;
	_bufferManager = bufferManager;

	//Texture buffer with temporal values, should be changed in runtime
	_bufferManager->addBuffer(BufferType::Texture, { 0, 0 }, { 0, 0 }, _glyphsLoader->getAtlas()->getSize());
	return false;
}

std::shared_ptr<BufferManager> TextComponent::getBufferManager() {
	return _bufferManager;
}

std::string TextComponent::getText() {
	return _text;
}

bool TextComponent::setScale(float scale) {
	_scale = scale;
	return false;
}

float TextComponent::getScale() {
	return _scale;
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

std::shared_ptr<GlyphsLoader> TextComponent::getLoader() {
	return _glyphsLoader;
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