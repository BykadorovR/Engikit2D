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

	_bufferManager->addBuffer(BufferType::Position, _position, _size, currentResolution);
	_shader = shader;
	return false;
}

bool ObjectComponent::setMember(std::string name, float value, int index) {
	OperationComponent::setMember(name, value, index);
	//if ObjectComponent wasn't initialized yet we don't need to change buffer
	if (_bufferManager)
		_bufferManager->changeBuffer(BufferType::Position, getPosition(), getSize(), currentResolution);
	else
		return true;
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

TextComponent::TextComponent() {
	_focus = 0;
	_editable = 0;

	_classVariablesFloat =
	{
		{"focus", &_focus},
		{"spacingCoeff", &_lineSpacingCoeff},
		{"scale", &_scale},
		{"editable", &_editable},
		{"cursorPosition", &_cursorPosition},
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
	_allignment = {TextAllignment::LEFT, TextAllignment::LEFT};
	_bufferManager = bufferManager;
	_cursorPosition = 0;

	//Texture buffer with temporal values, should be changed in runtime
	_bufferManager->addBuffer(BufferType::Texture, { 0, 0 }, { 0, 0 }, GlyphsLoader::instance().getAtlas()->getSize());
	return false;
}

std::shared_ptr<BufferManager> TextComponent::getBufferManager() {
	return _bufferManager;
}

bool TextComponent::setText(std::string text) {
	_cursorPosition += text.size();
#ifdef WIN32
	//All strings are stored as UTF8, so first we should convert it
	std::string UTF8String = convertMultibyteToUTF8(text);
	_text = UTF8String;
#else
	_text = text;
#endif
	return false;

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

VolumeComponent::VolumeComponent() {
	glGenVertexArrays(/*number of buffers to generate*/1, &_vao);
}

bool VolumeComponent::initialize() {
	/*
	glBindVertexArray(_vao);

	float vertices[] = {
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};
	if (_type == BufferType::Position) {
		//using int because coords and size should be in "pixels" which are int
		//if use float, some visual glitches can appear
		float startX = (float)x;
		//by default for BufferType::Position
		float startY = (float)(std::get<1>(sizeOverall) - y);
		// Order of coordinates: X, Y
		// 0   2
		// | / |
		// 1   3
		_data = { startX,                startY,
				  startX,                startY - height,
				  startX + width, startY,
				  startX + width, startY - height };
	}
	else {
		float objectWidthN = (float)width / (float)std::get<0>(sizeOverall);
		float objectHeightN = (float)height / (float)std::get<1>(sizeOverall);
		float startX = (float)x / (float)std::get<0>(sizeOverall);
		//by default for BufferType::Position
		float startY = (float)y / (float)std::get<1>(sizeOverall);
		objectHeightN *= -1; //so in data[] we will have only "+" sign everywhere
		// Order of coordinates: X, Y
		// 0   2
		// | / |
		// 1   3
		_data = { startX,                startY,
				  startX,                startY - objectHeightN,
				  startX + objectWidthN, startY,
				  startX + objectWidthN, startY - objectHeightN };
	}
	//bind our buffer to OpenGL buffer from OpenGL internal state (so like assign pointer to our buffer to OpenGL buffer in internal state), we can work only with binded buffers
	//From that point on any buffer calls we make (on the GL_ARRAY_BUFFER target) will be used to configure the currently bounded buffer, which is _vbo
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	//copies previously defined vertex data into the OpenGL buffer's memory
	//GL_DYNAMIC_DRAW: the data is changed a lot and used many times
	//IMPORTANT: we should copy data EACH time if no VAO, because VAO stores state for us
	glBufferData(GL_ARRAY_BUFFER, _data.size() * sizeof(float), &_data[0], GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indexes.size() * sizeof(int), &_indexes[0], GL_DYNAMIC_DRAW);
	if (_type == BufferType::Position) {
		glEnableVertexAttribArray(0);
		//Each vertex attribute takes its data from memory managed by a VBO and which VBO it takes its data from(you can have multiple VBOs) is determined by the VBO currently bound to GL_ARRAY_BUFFER when calling glVertexAttribPointer
		//we also can set stride as 0 because our data is tightly packed so OpenGL calculate stride by itself, but it's always better to calculate it manually
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
	}
	else {
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
	}
	*/
	return false;
}