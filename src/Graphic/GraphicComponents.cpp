#include "GraphicComponents.h"

ObjectComponent::ObjectComponent() {
	_componentName = "ObjectComponent";
}
bool ObjectComponent::initialize(std::tuple<float, float> position, std::tuple<float, float> size, std::shared_ptr<BufferManager> bufferManager) {
	_position = position;
	_size = size;
	bufferManager->addBuffer(BufferType::Position, _position, _size);
}