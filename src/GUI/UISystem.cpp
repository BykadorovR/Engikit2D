#include "UISystem.h"

void UISystem::process(std::shared_ptr<ObjectComponent> objectComponent, std::shared_ptr<UserInteractionComponent> UIComponent) {

}

// Called every game update
void UISystem::update() {
	for (auto entity : getEntities()) {
		auto vertexObject = entity->getComponent<ObjectComponent>();
		auto UIComponent = entity->getComponent<UserInteractionComponent>();
	}
}



void MoveSystem::process(std::shared_ptr<ObjectComponent> objectComponent, std::shared_ptr<UserInteractionComponent> UIComponent,
						 std::shared_ptr<TransformComponent> transformComponent) {
	float speed = transformComponent->_speed;
	int endX = std::get<0>(UIComponent->_leftClick);
	int endY = std::get<1>(UIComponent->_leftClick);
	if (!endX || !endY)
		return;
	int startX = objectComponent->_sceneX;
	int startY = objectComponent->_sceneY;
	float distanceX = endX - startX;
	float distanceY = endY - startY;
	float stepX = distanceX * speed;
	float stepY = distanceY * speed;
	if (abs(startX - endX) > stepX || abs(startY - endY) > stepY) {
		Matrix2D transform;
		transform.translate(stepX, stepY);
		transformComponent->setTransform(transform);
	}
}

void MoveSystem::update() {
	for (auto entity : getEntities()) {
		auto vertexObject = entity->getComponent<ObjectComponent>();
		auto transformComponent = entity->getComponent<TransformComponent>();
		auto UIComponent = entity->getComponent<UserInteractionComponent>();
		if (vertexObject && transformComponent && UIComponent)
			process(vertexObject, UIComponent, transformComponent);
	}
}