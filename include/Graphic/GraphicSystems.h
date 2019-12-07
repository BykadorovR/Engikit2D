#pragma once
#include "System.h"
#include "BufferManager.h"
#include "GraphicComponents.h"

class DrawSystem : public System {
private:
	std::shared_ptr<BufferManager> _bufferManager;
	void vertexUpdate(std::shared_ptr<ObjectComponent> vertexObject);
	void textureUpdate(std::shared_ptr<TextureComponent> textureObject);
	void textUpdate(std::shared_ptr<ObjectComponent> vertexObject, std::shared_ptr<TextComponent> textObject);
public:
	DrawSystem(std::shared_ptr<BufferManager> bufferManager);
	// Called every game update
	void update(std::shared_ptr<EntityManager> entityManager);
};