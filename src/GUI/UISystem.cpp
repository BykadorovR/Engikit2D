#include "UISystem.h"

void ClickInsideSystem::process(std::shared_ptr<ObjectComponent> objectComponent, std::shared_ptr<ClickInsideComponent> clickInsideComponent,
	std::shared_ptr<GroupEntitiesComponent> groupComponent) {
	int clickX = std::get<0>(clickInsideComponent->_leftClick);
	int clickY = std::get<1>(clickInsideComponent->_leftClick);
	if (!clickX || !clickY)
		return;
	if (clickX > objectComponent->_sceneX  && clickY > objectComponent->_sceneY &&
		clickX < objectComponent->_sceneX + objectComponent->_objectWidth && clickY < objectComponent->_sceneY + objectComponent->_objectHeight &&
		clickInsideComponent->_leftClickFlag) {
		//TODO: add group component
		std::cout << "Clicked inside: group " << groupComponent->_groupNumber << " " << groupComponent->_groupName << std::endl;
		clickInsideComponent->_leftClickFlag = false;
	}

	//We should handle click inside action only once per click, so reset coords of click after first handle
	clickInsideComponent->_leftClick = { 0, 0 };
	clickInsideComponent->_rightClick = { 0, 0 };
}

// Called every game update
void ClickInsideSystem::update() {
	for (auto entity : getEntities()) {
		auto vertexObject = entity->getComponent<ObjectComponent>();
		auto clickInsideComponent = entity->getComponent<ClickInsideComponent>();
		auto groupComponent = entity->getComponent<GroupEntitiesComponent>();
		if (vertexObject && clickInsideComponent && groupComponent)
			process(vertexObject, clickInsideComponent, groupComponent);
	}
}



void PointMoveSystem::process(std::shared_ptr<ObjectComponent> objectComponent, std::shared_ptr<PointMoveComponent> pointMoveComponent,
						 std::shared_ptr<TransformComponent> transformComponent) {
	float speed = pointMoveComponent->_speed;
	int clickX = std::get<0>(pointMoveComponent->_leftClick);
	int clickY = std::get<1>(pointMoveComponent->_leftClick);
	if (!clickX || !clickY)
		return;
	if (pointMoveComponent->_move == false && clickX > objectComponent->_sceneX  && clickY > objectComponent->_sceneY &&
		clickX < objectComponent->_sceneX + objectComponent->_objectWidth && clickY < objectComponent->_sceneY + objectComponent->_objectHeight)
		return;

	pointMoveComponent->_move = true;
	int objectX = objectComponent->_sceneX + objectComponent->_objectWidth / 2;
	int objectY = objectComponent->_sceneY + objectComponent->_objectHeight / 2;
	float angle = atan2(clickY - objectY, clickX - objectX);
	float stepX = cos(angle) * speed;
	float stepY = sin(angle) * speed;
	if (abs(objectX - clickX) > speed || abs(objectY - clickY) > speed) {
		transformComponent->setTransform({stepX, stepY});
	}
	else {
		pointMoveComponent->_move = false;
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


void ClickToMoveSystem::process(std::shared_ptr<ObjectComponent> objectComponent, std::shared_ptr<ClickToMoveComponent> clickToMoveComponent,
								std::shared_ptr<TransformComponent> transformComponent) {
	int currentClickX = std::get<0>(clickToMoveComponent->_currentClick);
	int currentClickY = std::get<1>(clickToMoveComponent->_currentClick);

	if (clickToMoveComponent->_clickFlag && currentClickX > objectComponent->_sceneX  && currentClickY > objectComponent->_sceneY &&
		currentClickX < objectComponent->_sceneX + objectComponent->_objectWidth && currentClickY < objectComponent->_sceneY + objectComponent->_objectHeight) {
		
		clickToMoveComponent->_previousClick = clickToMoveComponent->_currentClick;
		clickToMoveComponent->_currentClick = { 0, 0 };
		clickToMoveComponent->_clickFlag = false;
		return;
	}

	int previousClickX = std::get<0>(clickToMoveComponent->_previousClick);
	int previousClickY = std::get<1>(clickToMoveComponent->_previousClick);

	if (clickToMoveComponent->_clickFlag && previousClickX && previousClickY) {
		//We should point ADJUSTMENT not coords
		transformComponent->setTransform({currentClickX - (objectComponent->_sceneX + objectComponent->_objectWidth / 2), 
										  currentClickY - (objectComponent->_sceneY + objectComponent->_objectHeight / 2) });
		clickToMoveComponent->_previousClick = { 0, 0 };
	}

	clickToMoveComponent->_currentClick = { 0, 0 };
	clickToMoveComponent->_clickFlag = false;
}
void ClickToMoveSystem::update() {
	for (auto entity : getEntities()) {
		auto vertexObject = entity->getComponent<ObjectComponent>();
		auto transformComponent = entity->getComponent<TransformComponent>();
		auto clickToMoveComponent = entity->getComponent<ClickToMoveComponent>();
		if (vertexObject && transformComponent && clickToMoveComponent)
			process(vertexObject, clickToMoveComponent, transformComponent);
	}
}