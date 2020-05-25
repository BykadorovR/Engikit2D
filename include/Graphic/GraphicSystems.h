#pragma once
#include "System.h"
#include "BufferManager.h"
#include "GraphicComponents.h"

class DrawSystem : public System {
private:
	void vertexUpdate(std::shared_ptr<ObjectComponent> vertexObject);
	void textureUpdate(std::shared_ptr<TextureComponent> textureObject);
	void textUpdate(std::shared_ptr<Entity> entity);
public:
	DrawSystem();
	// Called every game update
	void update();
};