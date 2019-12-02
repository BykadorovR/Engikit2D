#include "GraphicComponents.h"
#include "TextureManager.h"
#include "State.h"

ObjectComponent::ObjectComponent() {
	_componentName = "ObjectComponent";
}

bool ObjectComponent::initialize(std::tuple<float, float> position, std::tuple<float, float> size, std::shared_ptr<BufferManager> bufferManager) {
	_position = position;
	_size = size;
	_buffer = bufferManager->addBuffer(BufferType::Position, _position, _size, resolution);
	_shader = std::make_shared<Shader>("../data/shaders/shader.vsh", "../data/shaders/shader.fsh");
	return false;
}

std::shared_ptr<Shader> ObjectComponent::getShader() {
	return _shader;
}

std::shared_ptr<Buffer> ObjectComponent::getBuffer() {
	return _buffer;
}

TextureComponent::TextureComponent() {
	_componentName = "TextureComponent";
}

bool TextureComponent::initialize(int textureID, std::shared_ptr<BufferManager> bufferManager) {
	_textureID = textureID;
	auto textureAtlas = TextureManager::instance()->getTextureAtlas(textureID);
	auto textureInfo = textureAtlas->getTexture(textureID);
	auto texture = std::get<0>(textureInfo);
	auto texturePosition = std::get<1>(textureInfo);
	//pos in atlas, tile size
	_buffer = bufferManager->addBuffer(BufferType::Texture, texturePosition, texture->getImageLoader()->getSize(), textureAtlas->getSize());
	return false;
}

int TextureComponent::getTextureID() {
	return _textureID;
}

std::shared_ptr<Buffer> TextureComponent::getBuffer() {
	return _buffer;
}