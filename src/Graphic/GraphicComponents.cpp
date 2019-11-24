#include "GraphicComponents.h"
#include "TextureManager.h"

ObjectComponent::ObjectComponent() {
	_componentName = "ObjectComponent";
}

bool ObjectComponent::initialize(std::tuple<float, float> position, std::tuple<float, float> size, std::shared_ptr<BufferManager> bufferManager) {
	_position = position;
	_size = size;
	_buffer = bufferManager->addBuffer(BufferType::Position, _position, _size);
}

TextureComponent::TextureComponent() {
	_componentName = "TextureComponent";
}

bool TextureComponent::initialize(int textureID, std::shared_ptr<BufferManager> bufferManager) {
	_textureID = textureID;
	std::shared_ptr<TextureRaw> texture = TextureManager::instance()->getTexture(textureID);
	_buffer = bufferManager->addBuffer(BufferType::Position, , );
}