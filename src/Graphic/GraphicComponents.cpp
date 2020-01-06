#include "GraphicComponents.h"
#include "TextureManager.h"
#include "State.h"
#include <algorithm>

ObjectComponent::ObjectComponent() {
	_componentName = "ObjectComponent";
}

bool ObjectComponent::initialize(std::tuple<float, float> position, std::tuple<float, float> size, std::shared_ptr<BufferManager> bufferManager, std::shared_ptr<Shader> shader) {
	_position = position;
	_size = size;
	_bufferManager = bufferManager;

	_bufferManager->addBuffer(BufferType::Position, _position, _size, resolution);
	_shader = shader;
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

TextComponent::TextComponent() {
	_focus = false;
	_page = 0;
}

bool TextComponent::initialize(std::wstring text, std::shared_ptr<GlyphsLoader> glyphsLoader, std::shared_ptr<BufferManager> bufferManager) {
	_scale = 1.f;
	_color = {1.f, 1.f, 1.f, 1.f};
	_lineSpacingCoeff = 1;
	_page = 0;
	_allignment = {TextAllignment::LEFT, TextAllignment::LEFT};
	_text = text;
	_glyphsLoader = glyphsLoader;
	_bufferManager = bufferManager;

	//Texture buffer with temporal values, should be changed in runtime
	_bufferManager->addBuffer(BufferType::Texture, { 0, 0 }, { 0, 0 }, _glyphsLoader->getAtlas()->getSize());
	return false;
}

std::shared_ptr<BufferManager> TextComponent::getBufferManager() {
	return _bufferManager;
}

std::wstring TextComponent::getText() {
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