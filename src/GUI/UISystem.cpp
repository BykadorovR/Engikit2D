#include "UISystem.h"

void ClickInsideSystem::process(std::shared_ptr<ObjectComponent> objectComponent, std::shared_ptr<ClickInsideComponent> clickInsideComponent) {
	int clickX = std::get<0>(clickInsideComponent->_leftClick);
	int clickY = std::get<1>(clickInsideComponent->_leftClick);
	if (!clickX || !clickY)
		return;
	if (clickX > objectComponent->_sceneX  && clickY > objectComponent->_sceneY &&
		clickX < objectComponent->_sceneX + objectComponent->_objectWidth && clickY < objectComponent->_sceneY + objectComponent->_objectHeight &&
		clickInsideComponent->_leftClickFlag) {
		//TODO: add group component
		std::cout << "Clicked inside" << std::endl;
		clickInsideComponent->_leftClickFlag = false;
	}
		
}

// Called every game update
void ClickInsideSystem::update() {
	for (auto entity : getEntities()) {
		auto vertexObject = entity->getComponent<ObjectComponent>();
		auto clickInsideComponent = entity->getComponent<ClickInsideComponent>();
		if (vertexObject && clickInsideComponent)
			process(vertexObject, clickInsideComponent);
	}
}



void PointMoveSystem::process(std::shared_ptr<ObjectComponent> objectComponent, std::shared_ptr<PointMoveComponent> pointMoveComponent,
						 std::shared_ptr<TransformComponent> transformComponent) {
	float speed = transformComponent->_speed;
	int clickX = std::get<0>(pointMoveComponent->_leftClick);
	int clickY = std::get<1>(pointMoveComponent->_leftClick);
	if (!clickX || !clickY)
		return;

	int objectX = objectComponent->_sceneX + objectComponent->_objectWidth / 2;
	int objectY = objectComponent->_sceneY + objectComponent->_objectHeight / 2;
	float angle = atan2(clickY - objectY, clickX - objectX);
	float stepX = cos(angle) * speed;
	float stepY = sin(angle) * speed;
	if (abs(objectX - clickX) > speed || abs(objectY - clickY) > speed) {
		Matrix2D transform;
		transform.translate(stepX, stepY);
		transformComponent->setTransform(transform);

		objectComponent->_sceneX += stepX;
		objectComponent->_sceneY += stepY;
	}
}

void PointMoveSystem::update() {
	for (auto entity : getEntities()) {
		auto vertexObject = entity->getComponent<ObjectComponent>();
		auto transformComponent = entity->getComponent<TransformComponent>();
		auto pointMoveComponent = entity->getComponent<PointMoveComponent>();
		if (vertexObject && transformComponent && pointMoveComponent)
			process(vertexObject, pointMoveComponent, transformComponent);
	}
}